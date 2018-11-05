#include <time.h>

#ifdef __BEOS__
#include <be/kernel/OS.h>
#endif

#include "User.h"

#include "VoidBot.h"

const uint32 kChatHistoryDepth = 50000;
	// Lots of history for IRC etc.  Figure a maximum of 800MB in BeOS
	// allocatable memory, each line of chat is 1000 characters (probably
	// much less), that gives us 800000 lines of text.  But with memory
	// fragmentation, and not wanting to send huge amounts of text back
	// to the user, limit it to this.

const uint32 kDefaultChatHistory = 25;

//-------------------------------------------------------------------------------
VoidBot::VoidBot()
        :BeShareBot::Bot("Atrus"),
         fHandled(false),
         fSendPublic(false),
         fPrivateCommand(false)
{ 
	String BootMessageText("Rebooted on ");
	BootMessageText += Time();
	fChatHistory.push_front(BootMessageText);
}
//-------------------------------------------------------------------------------
void VoidBot::ReceivedChatMessage(const char *SessionID, const char *Message)
{
	BeShareBot::User* usr = fData.UserByID(SessionID);
	if( NULL != usr )
	{
		if ( false == fPrivateCommand )
		{
			String lastwords("saying \"");
			lastwords += Message;
			lastwords += "\" on ";
			lastwords += Time();
			lastwords += " [";
			lastwords += NetTime();
			lastwords += "]";
	
			fLastWords[usr->Name()] = lastwords;
		}
		
		String command(Message);
		
		if( IsBotCommand(command) ) 
		{
			Command(SessionID, command);
		}
		
		ReportMessageExistence(SessionID, fData.UserByID(SessionID)->Name());
		

		if( false == fPrivateCommand )
		{
			// page of chat
			String chat("[");
			chat += Time();
			chat += "] (";
			chat += SessionID;
			chat += ") ";
			chat += usr->Name();
			chat += ": ";
			chat += command;
				
			fChatHistory.push_front(chat);
			if( fChatHistory.size() > kChatHistoryDepth )
			{
				fChatHistory.pop_back();
			}
		}
	}
}
//-------------------------------------------------------------------------------
void VoidBot::ReceivedPrivateMessage(const char* SessionID, const char* Message)
{
	printf("Private Message received from %s[%s]: %s\n", fData.UserByID(SessionID)->Name(), SessionID, Message);
	fflush (stdout); // Avoid long buffering delays if logging to a file.

	fPrivateCommand = true;
	ReceivedChatMessage(SessionID, Message);
	fPrivateCommand = false;
	
	if( false == fHandled )
	{
		Bot::ReceivedPrivateMessage(SessionID, Message);
	}
}
//-------------------------------------------------------------------------------
void
VoidBot::Command(const char* SessionID, const String& Command)
{
	fHandled = true;
	if ( !PublicCommands(SessionID, Command) )
	{
		if( !PrivateCommands(SessionID, Command) )
		{
			SendPrivateMessage(SessionID, "Use the 'help' command to list my capabilities.");
			fHandled = false;
		}
	}
}
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
bool VoidBot::PublicCommands(const char* SessionID, const String& Command)
{
// todo: these should all be busted out into thier own functions

	// "Message For" command
	String keyword("message for");
	int32 pos = WholeWordPos(Command, keyword.Cstr());
	if( B_ERROR != pos )
	{
		if( 2048 < Command.Length() )
		{
			SendMessage(SessionID, "There is _no way_ I can remember all that, please phrase the message shorter.");
			return true;
		}
		
		pos += keyword.Length();
		String name(GetNick(Command, pos));
		if( 0 == name.Length() )
		{
			SendMessage(SessionID, "Please specify a nick!");
			return true;
		}
		
		if( B_ERROR != name.IndexOf(' ') )
		{
			// name with space, adjust for delimiter chars.
			pos += 2;
		}
		
		pos += name.Length() + 1;
		
		if( (int32)Command.Length() < (pos - 1) )
		{
			SendMessage(SessionID, "Please specify a message.");
			return true;
		}
		
		String message("Message from ");
		message += fData.UserByID(SessionID)->Name();
		message += " Left at";
		message += " [";
		message += Time();
		message += "]";
		message += ": ";
		message += Command.Substring(pos);
		fMessagesForName[name].push_back(message);
		SendPrivateMessage(SessionID, "Message sent.");
		fReportedNewMessages[name] = false;
		return true;
	}
	
	// "Seen" command
	keyword = "seen";
	pos = WholeWordPos(Command, keyword.Cstr());
	if( -1 != pos )
	{
		pos += keyword.Length();
		
		String name(GetNick(Command, pos));
		if( 0 == name.Length() )
		{
			SendMessage(SessionID, "Please specify a nick!");
			return true;
		}
		
		if( fLastWords.find(name.Cstr()) != fLastWords.end() )
		{
			String info(name);
			info += " was last seen ";
			info += fLastWords[name.Cstr()];
			SendMessage(SessionID, info.Cstr());
		}
		else
		{
			String negatory("No, I haven't seen ");
			negatory += name;
			SendMessage(SessionID, negatory.Cstr());
		}
		return true;
	}
	
	//*****************<FORTUNE ADDED BY MISZA>************************//
	pos = WholeWordPos(Command, "fortune");
	if(-1 != pos)
	{
	FILE *fp = popen ("fortune", "r");
	if(fp != NULL)
	{
		char ch;
		String cool= "\n";
	
		while((ch = fgetc(fp))!= EOF)
		{
			cool += ch;
		}
	
    	fclose(fp);
    	if(cool != "\n")
   		{	cool = cool.Substring(0,(cool.Length()-1));
    		SendPrivateMessage(SessionID,cool.Cstr());
    	}
   	}
   	else
    	fclose(fp);
	return true;
	}
	//*****************</FORTUNE ADDED BY MISZA>************************//
	
	// "GMTime" command
	pos = WholeWordPos(Command, "gmtime");
	if( -1 != pos )
	{
		String info("The time is: ");
		info += Time();
		SendMessage(SessionID, info.Cstr());
		return true;
	}

	// "Time" command
	pos = WholeWordPos(Command, "time");
	if( -1 != pos )
	{
		String info("The time is: ");
		info += MyTime();
		SendMessage(SessionID, info.Cstr());
		return true;
	}

	// "iTime" command
	pos = WholeWordPos(Command, "itime");
	if( -1 != pos )
	{
		String info("The time is: ");
		info += NetTime();
		SendMessage(SessionID, info.Cstr());
		return true;
	}
	
	// "Atrus" Prevent messages to bebop
	keyword = "message for bebop";
	pos = WholeWordPos(Command, keyword.Cstr());
	if( B_ERROR != pos )
	{
		SendPrivateMessage(SessionID, "You didn't say the magic word...");
		                              
			return true;
	}

	// "Atrus" Prevent messages to Atrus
	keyword = "message for Atrus";
	pos = WholeWordPos(Command, keyword.Cstr());
	if( B_ERROR != pos )
	{
		SendPrivateMessage(SessionID, "You didn't say the magic word...");
		                              
			return true;
	}

	// "Atrus" Prevent messages to Bubbles
	keyword = "message for Bubbles";
	pos = WholeWordPos(Command, keyword.Cstr());
	if( B_ERROR != pos )
	{
		SendPrivateMessage(SessionID, "You didn't say the magic word...");
		                              
			return true;
	}

	// "Atrus" Prevent messages to FAQbot
	keyword = "message for FAQbot";
	pos = WholeWordPos(Command, keyword.Cstr());
	if( B_ERROR != pos )
	{
		SendPrivateMessage(SessionID, "You didn't say the magic word...");
		                              
			return true;
	}

	// "Messages" command
	pos = WholeWordPos(Command, "messages");
	if( B_ERROR != pos )
	{
		bool messages = false;
		uint32 quantity = 0;
		const char* name = fData.UserByID(SessionID)->Name();
		
		if( fMessagesForName.find(name) != fMessagesForName.end() )
		{
			quantity = fMessagesForName[name].size();
			
			if( quantity != 0 )
			{
				messages = true;
			}
		}
		
		if( true == messages )
		{	
			for( uint32 i = 0; i < quantity; i++ )
			{
				SendPrivateMessage(SessionID, fMessagesForName[name][i].Cstr());
			}
			
			fMessagesForName[name].clear();
			fReportedNewMessages[name] = true;
		}
		else
		{	
			String reply("Sorry ");
			reply += fData.UserByID(SessionID)->Name();
			reply += ", you have no new messages.";
			SendPrivateMessage(SessionID, reply.Cstr());
		}
				
		return true;
	}
	
	// "Version" command
	pos = WholeWordPos(Command, "version");
	if( B_ERROR != pos )
	{
		// The version string is made up of MAJOR_RELEASE.MINOR_RELEASE.BUG_FIXES
		SendPrivateMessage(SessionID, "I am at version 2.5.0 (MUSCLE " MUSCLE_VERSION_STRING "), "
			"built on " __DATE__ " at " __TIME__ ".");
		return true;
	}

	// "catsup" and "catslo" commands to play back stored chat history,
	// "catsup n" does last n lines of chat, "catslo n" does last n lines
	// of local chat excluding text from users with # at the start of
	// their name (like #haiku, the IRC bridge bot).
	bool isLo = false;
	pos = WholeWordPos(Command, "catsup");
	if( B_ERROR == pos )
	{
		pos = WholeWordPos(Command, "catslo");
		isLo = true;
	}
	if( B_ERROR != pos )
	{
		// Figure out how many lines of chat to return
		pos++;
		pos += strlen("catsup"); // Same length as catslo.
		int lines = kDefaultChatHistory; // Needs to be signed.
		if( (uint32)pos < Command.Length() )
		{
			lines = atoi(&(Command.Cstr()[pos]));
			if( lines <= 0)
				lines = kDefaultChatHistory;
		}

		// Go through the history in newest to oldest order,
		// prepending text to the output text if it matches
		// our requirements.  Stop when we have enough text.
		String chatText;
		int historySize = fChatHistory.size();
		for( int i = 0; i < historySize && lines > 0 ; i++ )
		{
			// See if we should reject this line of text.
			if (isLo)
			{
				// Look for the userid in a chat line, which looks like:
				// [Mon Nov 5 22:51:22 GMT] (54) #haiku: chatter text
				const char *testPntr = fChatHistory[i].Cstr();
				while( *testPntr != 0 && *testPntr != ')' )
					testPntr++;
				if( testPntr[0] == ')' && testPntr[1] == ' ' && testPntr[2] == '#' )
					continue; // Skip this line, has a userid with #.
			}

			chatText = chatText.Prepend("\n");
			chatText = chatText.Prepend(fChatHistory[i]);
			lines--;
		}

		SendPrivateMessage(SessionID, chatText.Cstr());
		return true;
	}

	// "Email" command
	keyword = "email";
	pos = WholeWordPos(Command, keyword.Cstr());
	if( -1 != pos )
	{
		pos += keyword.Length();
		
		String name(GetNick(Command, pos));
		if( 0 == name.Length() )
		{
			SendPrivateMessage(SessionID, "Please specify a nick!");
			return true;
		}
		
		{
			String fullname= "./BotData/email/";
			fullname += name;
			if(!SendTextfile(SessionID, fullname.Cstr(), false))
			{
				SendPrivateMessage(SessionID, "No email address found for that nick!");
			}
			return true;
		}
	}

	// "Gimme" command (v.2)
	keyword = "gimme";
	pos = WholeWordPos(Command, keyword.Cstr());
	if( -1 != pos )
	{
		pos += keyword.Length();
		
		String name(GetNick(Command, pos));
		if( 0 == name.Length() )
		{
			SendPrivateMessage(SessionID, "Please specify help page!");
			return true;
		}
		
		{
			String fullname="./BotData/faq/";
			fullname += name;
			return SendTextfile(SessionID, fullname.Cstr(), false);
		}
	}

	// Do all one word commands (aka send text file with same name to user)
	String name(Command.Substring( Command.IndexOf(" ") + 1));
	if ( 0 < name.Length() )
	{
		String fullname="./BotData/commands/";
		fullname += name.Cstr();
		return SendTextfile(SessionID, fullname.Cstr(), false);
	}
	return false; // Command not recognised.
}
//-------------------------------------------------------------------------------
bool VoidBot::PrivateCommands(const char* SessionID, const String& Command)
{
	if( true == fPrivateCommand )
	{
		// "Public" command
		int32 pos = WholeWordPos(Command, "public");
		if( -1 != pos )
		{
			SendPrivateMessage(SessionID, "I am now in public mode, I hope you are happy.");
			fSendPublic = true;
			return true;
		}		
	
		// "Private" command
		pos = WholeWordPos(Command, "private");
		if( -1 != pos )
		{
			SendPrivateMessage(SessionID, "I am now in private mode, much better.");
			fSendPublic = false;
			return true;
		}		
	
		// more stuff
	}

	
	return false;
}
//-------------------------------------------------------------------------------
bool VoidBot::SendTextfile(const char* SessionID, const char* Message, bool motdMessage)
{
	FILE *fp = fopen(Message, "r");
	if(fp != NULL)
	{
		char temp[2048];
		String cool="\n";

		while(fgets(temp, sizeof(temp), fp))
		{
			cool += temp;
			// Message is longer than 1900 bytes.
			if((cool != "\n") && ( cool.Length() > 1900 ))
			{
				if (motdMessage)
				{
					SendMOTDMessage(SessionID, cool.Cstr());
				}
				else
				{
					SendMessage(SessionID, cool.Cstr());
				}
				cool = "\n";
			}

		}

		// Message was shorter than 1900 bytes
		// In other words this is the last line of file
		if(cool != "\n")
		{
			if (motdMessage)
			{
				SendMOTDMessage(SessionID, cool.Cstr());
			}
			else
			{
				SendMessage(SessionID, cool.Cstr());
			}
		}
			
		fclose(fp);
		return true;
	}
	else
	{
		// No file found
//		String Oops = "Oops, unable to open file: ";
//		Oops += Message;
//		SendMessage(SessionID, Oops.Cstr());
		return false;
	}	
}
//-------------------------------------------------------------------------------
void VoidBot::SendMessage(const char* SessionID, const char* Message)
{
	if( (true == fSendPublic) && (false == fPrivateCommand) )
	{
		SendPublicMessage(Message);
	}
	else
	{
		SendPrivateMessage(SessionID, Message); 
	}
}
//-------------------------------------------------------------------------------
void 
VoidBot::UserLoggedInOrChangedName(const char *SessionID, const char *Name)
{
	// Keep list of UserIDs. If UserID is not found then it is new user
	if( fLogged.find(SessionID) == fLogged.end() )
	{
		// If user has bigger SessionID than bot has, then send message.
		if( atol(SessionID) > atol(fData.SessionID()) )
		{
			char Line[1024];
			String cool;
			FILE *fp;

			// Send special message for one user only.  Not private, since it
			// beeps.  Append a fortune to it too.
  			fp = fopen("./BotData/motd/motd", "r");
			if (fp != NULL)
			{
				while (fgets(Line, sizeof(Line)-1, fp) != NULL)
					cool += Line;
		    	fclose(fp);
		   	}

            // Append the output of the "fortune" program.
			fp = popen ("fortune", "r");
			if (fp != NULL)
			{
				while (fgets(Line, sizeof(Line)-1, fp) != NULL)
					cool += Line;
		    	fclose(fp);
		   	}

		   	// Remove trailing spaces (hopefully newlines too), and leading ones.
			SendMOTDMessage(SessionID, cool.Trim().Cstr());
		}
		String log("L");
		fLogged[SessionID] = log;
	}

	if( fLastWords.find(Name) == fLastWords.end() )
	{	
		String log("logging in on ");
		log += Time();
		log += " [";
		log += NetTime();
		log += "]";
		log += ", and hasn't said anything yet.";
		fLastWords[Name] = log;
		fReportedNewMessages[Name] = false;
	}
	else
	{
		// set last words of previous nick
		const char* prevname = fData.UserByID(SessionID)->Name();
		String log(fLastWords[prevname]);
		log += ", and changed to ";
		log += Name;
		log += " on ";
		log += Time();
		log += " [";
		log += NetTime();
		log += "]";
		fLastWords[prevname] = log;
		
		// set log for new nick
		log = "changing from ";
		log += prevname;
		log += ", and hasn't said anything yet";
		fLastWords[Name] = log;
		fData.UserByID(SessionID)->Name(String(Name).ToLowerCase().Cstr());
	}
	
	// message reporting
	ReportMessageExistence(SessionID, Name);
}
//-------------------------------------------------------------------------------
const char* VoidBot::OwnerName()
{
	return "AGMS mailto:agmsmith@ncf.ca";
}
//-------------------------------------------------------------------------------
void 
VoidBot::UserLoggedOut(const char*)
{
	// do we care?
}
//-------------------------------------------------------------------------------
String VoidBot::MyTime()
{
	time_t timer = time(NULL);
	String result(asctime(localtime(&timer)));
	result[result.Length() - 1] = ' ';
	result += "My Local Time";
	
	return result;
}
//-------------------------------------------------------------------------------

String VoidBot::Time()
{
	time_t timer = time(NULL);
	String result(asctime(gmtime(&timer)));
	result[result.Length() - 1] = ' ';
	result += "GMT";
	
	return result;
}
//-------------------------------------------------------------------------------
String VoidBot::NetTime()
{
	char buff[32];	// blea =P
	sprintf(buff, "@%ld", (long int) (((SecondsSinceJan1970() + 3600) % 86400) / 86.4));
	 
	return buff;
}
//-------------------------------------------------------------------------------
time_t VoidBot::SecondsSinceJan1970()
{
	tm itstart;							// set up the base time of 

		itstart.tm_year=70;				// january 1, 1970, 00:00:00
		itstart.tm_mday=1;
		itstart.tm_mon=0;
		itstart.tm_hour=0;
		itstart.tm_min=0;
		itstart.tm_sec=0;
		itstart.tm_wday=4;				// Happens to be a thursday
		itstart.tm_yday=0;
		itstart.tm_isdst=0;

	time_t its = mktime(&itstart);		// convert this to time_t form
	time_t now = time(NULL);			// get current time, in time_t
	time_t gwtime = mktime(gmtime(&now));
	
	return (time_t)difftime(gwtime, its);	// get difference twxt gmtime and its
}
//-------------------------------------------------------------------------------
bool VoidBot::IsBotCommand(const String& Command)
{
	bool result = false;
	if( (0 == WholeWordPos(Command, Name())) || fPrivateCommand )
	{
		result = true;
	}
	
	return result;
}
//-------------------------------------------------------------------------------
int VoidBot::WholeWordPos(const String& WordIn, const char* Word)
{
	int result = WordIn.IndexOfIgnoreCase(Word);
	if( -1 != result )
	{
		if( 0 == result )
		{
			// beginning OK
		}
		else
		{
			if( ' ' == WordIn[result - 1] )
			{
				// beginning OK
			}
			else
			{
				// beginning not OK.
				return -1;
			}
		}
		
		uint32 endpos = result + strlen(Word);  
		if( WordIn.Length() > endpos )
		{
			if( ' ' == WordIn[endpos] )
			{
				// end OK.
			}
			else
			{
				// end not OK
				return -1;
			} 
		}
		else
		{
			// end ok.
		}	
	}
	
	return result;
}
//-------------------------------------------------------------------------------
String VoidBot::GetNick(const String& From, int32 StartFrom)
{
	int32 cmdlen = From.Length();
	
	if( cmdlen <= StartFrom )
	{
		return "";
	}
	
	// chew white space
	while( StartFrom < cmdlen )
	{
		if( (From[StartFrom] == ' ') )
		{
			++StartFrom;
		}
		else
		{
			break;
		}
	}
	
	char breakchar = ' ';
	const char delimiter = '\'';
	
	if( delimiter == From[StartFrom] )
	{
		breakchar = delimiter;
		++StartFrom;
	}
	 
	String name;
	while( StartFrom < cmdlen )
	{
		if( (From[StartFrom] == breakchar) ||
		    (From[StartFrom] == '?')
		  )
		{
			break;
		}
		name += From[StartFrom];
		++StartFrom;
	}
	
	return name.ToLowerCase();
}
//-------------------------------------------------------------------------------
void VoidBot::ReportMessageExistence(const char* SessionID, const char* Name)
{
	if( (fMessagesForName.find(Name) != fMessagesForName.end()) &&
	    (fMessagesForName[Name].size() > 0) &&
	    (false == fReportedNewMessages[Name])
	  )
	{
		String notice("Message");
		
		if( fMessagesForName[Name].size() > 1 )
		{
			notice += "s";
		}
		
		notice += " for you Sir!";
		
		SendPrivateMessage(SessionID, notice.Cstr());
		fReportedNewMessages[Name] = true;
	}
}
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
