#include <string.h>
#include "dataio/TCPSocketDataIO.h"
#include "iogateway/MessageIOGateway.h"
#include "reflector/StorageReflectConstants.h"
#include "util/NetworkUtilityFunctions.h"
#include "system/SetupSystem.h"

USING_NAMESPACE(muscle);

// This program measures the response time of a MUSCLE server.
int main(int argc, char ** argv)
{
   CompleteSetupSystem css;
   if (argc > 1)
   {
      int s = Connect(argv[1], 2960, "testresponse");
      if (s >= 0)
      {
         Message pingMessage(PR_COMMAND_PING);  // we'll keep on sending this and seeing how long it takes to get back

         TCPSocketDataIO sockIO(s, false);

         MessageIOGateway ioGateway;
         ioGateway.SetDataIO(DataIORef(&sockIO, false));

         QueueGatewayMessageReceiver inQueue;
         uint64 lastThrowTime = 0;
         bool pingSent = false;
         uint64 min = (uint64)-1;
         uint64 max = 0;
         uint64 total = 0;
         uint64 count = 0;
         uint64 lastPrintTime = 0;
         while(1)
         {
            if ((pingSent == false)&&(ioGateway.AddOutgoingMessage(MessageRef(&pingMessage, false)) == B_NO_ERROR)) 
            {
               pingSent = true;
               lastThrowTime = GetRunTime64();
            }

            fd_set readSet;  FD_ZERO(&readSet);  FD_SET(s, &readSet);
            fd_set writeSet; FD_ZERO(&writeSet); if (ioGateway.HasBytesToOutput()) FD_SET(s, &writeSet);

            if (select(s+1, &readSet, &writeSet, NULL, NULL) < 0)
            {
               LogTime(MUSCLE_LOG_ERROR, "select() failed, aborting!\n");
               break;
            }

            if (FD_ISSET(s, &readSet))
            {
               if (ioGateway.DoInput(inQueue) < 0)
               {
                  LogTime(MUSCLE_LOG_ERROR, "Error reading from gateway, aborting!\n");
                  break;
               }

               MessageRef next;
               while(inQueue.RemoveHead(next) == B_NO_ERROR)
               {
                  if ((pingSent)&&(next()->what == PR_RESULT_PONG))
                  {
                     uint64 result = GetRunTime64()-lastThrowTime;
                     min = muscleMin(min, result);
                     max = muscleMax(max, result);
                     total += result;
                     count++;
                     if (OnceEvery(1000000, lastPrintTime)) LogTime(MUSCLE_LOG_INFO, "Results: min=" UINT64_FORMAT_SPEC "us max=" UINT64_FORMAT_SPEC "us avg=" UINT64_FORMAT_SPEC "us trials=" UINT64_FORMAT_SPEC "\n", min, max, total/count, count);

                     pingSent = false;  // we need to send another one now
                  }
               }
            }

            if ((FD_ISSET(s, &writeSet))&&(ioGateway.DoOutput() < 0))
            {
               LogTime(MUSCLE_LOG_ERROR, "Error writing to gateway, aborting!\n");
               break;
            }
         }
      }
   }
   else LogTime(MUSCLE_LOG_ERROR, "Usage: testresponse 192.168.0.150\n");
   return 0;
}
