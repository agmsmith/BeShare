// BeShare Bot class.
//
// By Alan Ellis
//
// Origial 'C' framework by Jeremy Friesner (jaf@lcsaudio.com).
// Enormous portions of the code ripped off from there.
// Public domain code.  Hack away!

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

#include "muscle/dataio/TCPSocketDataIO.h"
#include "reflector/StorageReflectConstants.h"
#include "regex/PathMatcher.h"
#include "util/NetworkUtilityFunctions.h"
#include "util/StringTokenizer.h"

#include "BeShareDefs.h"
#include "User.h"

#include "Bot.h"

namespace BeShareBot
{

//-------------------------------------------------------------------------------
Bot::Bot(const char* Name = "Atrus") :
        fData(Name), fQuitCommand("begone")
{
}
//-------------------------------------------------------------------------------
Bot::~Bot()
{
}
//-------------------------------------------------------------------------------
void Bot::Quit(int Code)
{
    fData.quit = true;
    fData.exitCode = Code;
}
//-------------------------------------------------------------------------------
int Bot::Setup(int ArgC, char **ArgV, int& Socket)
{
    int result = 0;

    if (ArgC < 2)
    {
        LogTime(MUSCLE_LOG_INFO,
                "Usage:  sharebot hostname[:port] [BotName] [--quit-command <command>]\n");
        return 5;
    }

    int port = 0;
    char * hostName = ArgV[1];
    char * colon = strrchr(hostName, ':');
    if (colon)
    {
        *colon = '\0';
        port = atoi(colon + 1);
    }

    Socket = Connect(hostName, port ? port : 2960, fData.Name(), false);
    if (Socket < 0)
    {
        return 10;
    }

    delete fData.Gateway();  // avoid memory leak --jaf

    MessageIOGateway * gw = new MessageIOGateway;
    gw->SetDataIO(DataIORef(new TCPSocketDataIO(Socket, false)));
    fData.Gateway(gw);

    // set up messages

    // Ask the server what our host name is
    fData.Gateway()->AddOutgoingMessage(
            GetMessageFromPool(PR_COMMAND_GETPARAMETERS));

    // Subscribe to watch all arriving BeShare users and what they say
    MessageRef watchBeShareUsers = GetMessageFromPool(PR_COMMAND_SETPARAMETERS);
    watchBeShareUsers()->AddBool("SUBSCRIBE:beshare/name", true);
    //   		watchBeShareUsers()->AddBool(PR_NAME_SUBSCRIBE_QUIETLY, true);  // so as not to spam when we are startup up
    fData.Gateway()->AddOutgoingMessage(watchBeShareUsers);

    // kinda silly ...
    const char* name = fData.Name();

    if (ArgC > 2)
    {
        name = ArgV[2];
    }

    fData.Name(name);

    if (ArgC > 4)
    {
        if (0 == strcmp(ArgV[3], "--quit-command"))
        {
            fQuitCommand = ArgV[4];
        }
    }
    return result;
}
//-------------------------------------------------------------------------------
int Bot::Run(int ArgC, char **ArgV)
{
    int socket = 0;
    int result = Setup(ArgC, ArgV, socket);
    if (result)
    {
        return result;
    }

    // todo: this should be broken into separate functions, and
    //       there needs to be a way to test the connection and reconnect if
    //       necessary.

    fd_set readSet, writeSet;
    while (!fData.quit)
    {
        FD_ZERO(&readSet);
        FD_ZERO(&writeSet);

        int maxfd = socket;
        FD_SET(socket, &readSet);

        if (fData.Gateway()->HasBytesToOutput())
        {
            FD_SET(socket, &writeSet);
        }

        if (select(maxfd + 1, &readSet, &writeSet, NULL, NULL) < 0)
        {
            LogTime(MUSCLE_LOG_CRITICALERROR,
                    "BeShareBot::Bot : select() failed!\n");
            return -1;
        }

        bool reading = FD_ISSET(socket, &readSet);
        bool writing = FD_ISSET(socket, &writeSet);
        bool writeError = ((writing) && (fData.Gateway()->DoOutput() < 0));
        bool readError = ((reading) && (fData.Gateway()->DoInput(*this) < 0));
        if ((readError) || (writeError))
        {
            LogTime(MUSCLE_LOG_ERROR,
                    "Connection to server closed, reconnecting...\n");
            socket = -1; // gateway will close the socket when we delete the gateway in Setup()
            while (socket < 0)
            {
                (void) Setup(ArgC, ArgV, socket);
                if (socket < 0)
                {
                    LogTime(MUSCLE_LOG_ERROR,
                            "Oops, reconnect failed!  I'll wait 5 seconds, then try again.\n");
                    Snooze64(5000000);
                }
            }
        }
    }
    delete fData.Gateway();
    return fData.exitCode;
}
//-------------------------------------------------------------------------------
void Bot::Name(const char *TheName)
{
    fData.Name(TheName);
}
//-------------------------------------------------------------------------------
const char*
Bot::Name()
{
    return fData.Name();
}
//-------------------------------------------------------------------------------
void Bot::SendPublicMessage(const char *Message)
{
    MessageRef chatMessage = GetMessageFromPool(NET_CLIENT_NEW_CHAT_TEXT);
    chatMessage()->AddString(PR_NAME_KEYS, "/*/*/beshare"); // send message to all BeShare clients...
    chatMessage()->AddString("session", fData.SessionID());
    chatMessage()->AddString("text", Message);
    fData.Gateway()->AddOutgoingMessage(chatMessage);
}
//-------------------------------------------------------------------------------
void Bot::SendPrivateMessage(const char *SessionID, const char *Message)
{
    MessageRef chatMessage = GetMessageFromPool(NET_CLIENT_NEW_CHAT_TEXT);
    String toString("/*/");  // send message to all hosts...
    toString += SessionID; // who have the given sessionID (or "*" == all session IDs)
    toString += "/beshare";          // and are beshare sessions.
    chatMessage()->AddString(PR_NAME_KEYS, toString.Cstr());
    chatMessage()->AddString("session", fData.SessionID());
    chatMessage()->AddString("text", Message);
    chatMessage()->AddBool("private", true);
    fData.Gateway()->AddOutgoingMessage(chatMessage);
}
//-------------------------------------------------------------------------------
void Bot::SendMOTDMessage(const char *SessionID, const char *Message)
{
    MessageRef chatMessage = GetMessageFromPool(NET_CLIENT_NEW_CHAT_TEXT);
    String toString("/*/");  // send message to all hosts...
    toString += SessionID; // who have the given sessionID (or "*" == all session IDs)
    toString += "/beshare";          // and are beshare sessions.
    chatMessage()->AddString(PR_NAME_KEYS, toString.Cstr());
    chatMessage()->AddString("session", fData.SessionID());
    chatMessage()->AddString("text", Message);
    fData.Gateway()->AddOutgoingMessage(chatMessage);
}
//-------------------------------------------------------------------------------
void Bot::HandleResultData(const MessageRef& msgRef)
{
    Message * msg = msgRef();
    MessageFieldNameIterator it = msg->GetFieldNameIterator(B_MESSAGE_TYPE);
    String fieldText;
    while (it.GetNextFieldName(fieldText) == B_NO_ERROR)
    {
        int pathDepth = GetPathDepth(fieldText.Cstr());
        if (pathDepth >= USER_NAME_DEPTH)
        {
            MessageRef tempRef;
            if (msg->FindMessage(fieldText.Cstr(), tempRef) == B_NO_ERROR)
            {
                switch (pathDepth)
                {
                case USER_NAME_DEPTH:
                {
                    HandleNameInfo(fieldText.Cstr(), tempRef);
                    break;
                }
                }
            }
        }
    }
    // Look for sub-messages that indicate that nodes were removed from the tree
    String nodepath;
    for (int i = 0;
            (msg->FindString(PR_NAME_REMOVED_DATAITEMS, i, nodepath)
                    == B_NO_ERROR); i++)
    {
        int pathDepth = GetPathDepth(nodepath.Cstr());
        if (pathDepth >= USER_NAME_DEPTH)
        {
            String sessionID = GetPathClause(SESSION_ID_DEPTH, nodepath.Cstr());
            sessionID = sessionID.Substring(0, sessionID.IndexOf('/'));

            switch (GetPathDepth(nodepath.Cstr()))
            {
            case USER_NAME_DEPTH:
                if (strncmp(GetPathClause(USER_NAME_DEPTH, nodepath.Cstr()),
                        "name", 4) == 0)
                {
                    UserLoggedOut(sessionID.Cstr());
                }
            }
        }
    }

}
//-------------------------------------------------------------------------------
void Bot::HandleNameInfo(const char* FieldText, const MessageRef& Message)
{
    String sessionID = GetPathClause(SESSION_ID_DEPTH, FieldText);
    sessionID = sessionID.Substring(0, sessionID.IndexOf('/'));

    const char * nodeName = GetPathClause(USER_NAME_DEPTH, FieldText);
    if (strncmp(nodeName, "name", 4) == 0)
    {
        bool isBot = false;  // default value
        Message.GetItemPointer()->FindBool("bot", &isBot);  // set if present

        const char* name;
        if (Message.GetItemPointer()->FindString("name", &name) == B_NO_ERROR)
        {
            User usr(sessionID.Cstr(), String(name).ToLowerCase().Cstr(),
                    isBot);
            fData.AddUser(usr);
            UserLoggedInOrChangedName(sessionID.Cstr(),
                    String(name).ToLowerCase().Cstr());
        }
    }
}
//-------------------------------------------------------------------------------
void Bot::HandleChatText(const Message* Message)
{
    bool isBot = true;
    if (B_ERROR == Message->FindBool("bot", &isBot))
    {
        isBot = false;
    }

    bool botToBotMessage = false;
    Message->FindBool("btbmessage", &botToBotMessage);

    if (isBot && (!botToBotMessage))
    {
        // Ignore Bots
        return;
    }

    const char *text;
    const char *session;
    if ((Message->FindString("text", &text) == B_NO_ERROR)
            && (Message->FindString("session", &session) == B_NO_ERROR))
    {
        if (Message->HasName("private") || botToBotMessage)
        {
            if (false == CheckOwnerMessage(session, text))
            {
                ReceivedPrivateMessage(session, text);
            }
        }
        else
        {
            ReceivedChatMessage(session, text);
        }
    }
}
//-------------------------------------------------------------------------------
bool Bot::CheckOwnerMessage(const char *SessionID, const char *Message)
{
    bool result = false;

    if (!strcmp("owner", Message))
    {
        SendPrivateMessage(SessionID, OwnerName());
        //		SendTextfile(SessionID, "./commands/owner", false);
        result = true;
    }

    return result;
}
//-------------------------------------------------------------------------------
void Bot::ReceivedPrivateMessage(const char *, const char *Command)
{
    if (strstr(Command, fQuitCommand.Cstr()))
    {
        SendPublicMessage("I'm outta here!");
        Quit(0);
    }
}
//-------------------------------------------------------------------------------
/*
 void
 Bot::UserLoggedInOrChangedName(const char *SessionID, const char *Name)
 {
 }
 //-------------------------------------------------------------------------------
 void
 Bot::UserLoggedOut(const char *SessionID)
 {
 }
 //-------------------------------------------------------------------------------
 */
void Bot::MessageReceivedFromGateway(const MessageRef &incoming, void *)
{
    switch (incoming.GetItemPointer()->what)
    {
    case PR_RESULT_DATAITEMS:
        HandleResultData(incoming);
        break;

    case NET_CLIENT_NEW_CHAT_TEXT:
    {
        // Someone has sent a line of chat text to display
        HandleChatText(incoming.GetItemPointer());
        break;
    }

    case PR_RESULT_PARAMETERS:
    {
        const char * sessionRoot;
        if (incoming.GetItemPointer()->FindString(PR_NAME_SESSION_ROOT,
                &sessionRoot) == B_NO_ERROR)
        {
            // session root is of form "/hostname/sessionID"; parse these out
            const char * lastSlash = strrchr(sessionRoot, '/');
            if (lastSlash)
            {
                fData.SessionID(lastSlash + 1);
            }
        }
        break;
    }
    }
}

}	// NS BeShare

