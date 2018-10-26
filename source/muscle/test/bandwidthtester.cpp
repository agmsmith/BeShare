/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

#include "dataio/TCPSocketDataIO.h"
#include "iogateway/MessageIOGateway.h"
#include "reflector/StorageReflectConstants.h"
#include "util/NetworkUtilityFunctions.h"
#include "system/SetupSystem.h"

USING_NAMESPACE(muscle);

/** Sends a stream of messages to the server, or receives them.  Prints out the average send/receive speed */
int main(int argc, char ** argv)
{
   CompleteSetupSystem css;

   char * defaultName = "localhost";
   char * hostName = (argc > 1) ? argv[1] : defaultName;
   int port = 0;

   char * colon = strchr(hostName, ':');
   if (colon)
   {
      *colon = '\0';
      port = atoi(colon+1);
   }
   if (port <= 0) port = 2960;
 
   bool send = (argc > 2)&&(strcmp(argv[2], "send") == 0);
   int s = Connect(hostName, (uint16)port, "bandwidthtester");
   if (s < 0) exit(10);

   MessageIOGateway gw;
   gw.SetDataIO(DataIORef(new TCPSocketDataIO(s, false)));
   if (send) printf("Sending bandwidthtester messages...\n");
   else
   {
      printf("Listening for bandwidthtester messages....\n");
      // Tell the server that we are interested in receiving bandwidthtester messages
      MessageRef uploadMsg(GetMessageFromPool(PR_COMMAND_SETDATA));
      uploadMsg()->AddMessage("bandwidthtester", GetMessageFromPool());
      gw.AddOutgoingMessage(uploadMsg);
   }

   // Here is a (fairly large) message that we will send repeatedly in order to bandwidthtester the server
   MessageRef sendMsgRef(GetMessageFromPool(0x666));
   sendMsgRef()->AddString(PR_NAME_KEYS, "bandwidthtester");
   sendMsgRef()->AddData("bandwidthtester test data", B_RAW_TYPE, NULL, 8000);

   fd_set readSet, writeSet;
   uint64 startTime = GetRunTime64();
   struct timeval lastPrintTime = {0, 0};
   uint32 tallyBytesSent = 0, tallyBytesReceived = 0;
   QueueGatewayMessageReceiver inQueue;
   while(true)
   {
      FD_ZERO(&readSet);
      FD_ZERO(&writeSet);

      FD_SET(s, &readSet);
      if ((send)||(gw.HasBytesToOutput())) FD_SET(s, &writeSet);

      const struct timeval printInterval = {5, 0};
      if (OnceEvery(printInterval, lastPrintTime))
      {
         uint64 now = GetRunTime64();
         if (tallyBytesSent > 0) 
         {
            if (send) LogTime(MUSCLE_LOG_INFO, "Sending at %lu bytes/second\n", tallyBytesSent/((uint32)(((now-startTime))/1000000)));
                 else LogTime(MUSCLE_LOG_INFO, "Sent %lu bytes\n", tallyBytesSent);
            tallyBytesSent = 0;
         }
         if (tallyBytesReceived > 0)
         {
            if (send) LogTime(MUSCLE_LOG_INFO, "Received %lu bytes\n", tallyBytesReceived);
                 else LogTime(MUSCLE_LOG_INFO, "Receiving at %lu bytes/second\n", tallyBytesReceived/((uint32)((now-startTime)/1000000)));
            tallyBytesReceived = 0;
         }
         startTime = now;
      }

      if (select(s+1, &readSet, &writeSet, NULL, NULL) < 0) LogTime(MUSCLE_LOG_CRITICALERROR, "bandwidthtester: select() failed!\n");
      uint64 now = GetRunTime64();
     
      if ((send)&&(gw.HasBytesToOutput() == false)) for (int i=0; i<10; i++) gw.AddOutgoingMessage(sendMsgRef);
      bool reading = FD_ISSET(s, &readSet);
      bool writing = FD_ISSET(s, &writeSet);
      int32 wroteBytes = (writing) ? gw.DoOutput() : 0;
      int32 readBytes  = (reading) ? gw.DoInput(inQueue) : 0;
      if ((readBytes < 0)||(wroteBytes < 0))
      {
         LogTime(MUSCLE_LOG_ERROR, "Connection closed, exiting.\n");
         break;
      }
      tallyBytesSent     += wroteBytes;
      tallyBytesReceived += readBytes;

      MessageRef incoming;
      while(inQueue.RemoveHead(incoming) == B_NO_ERROR) {/* ignore it, we just want to measure bandwidth */}
   }
   CloseSocket(s);
   LogTime(MUSCLE_LOG_INFO, "\n\nBye!\n");
   return 0;
}
