/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */  

#include <stdio.h>
#include "system/Thread.h"
#include "system/SetupSystem.h"

USING_NAMESPACE(muscle);

class TestThread : public Thread
{
public:
   TestThread() {/* empty */}

   virtual status_t MessageReceivedFromOwner(const MessageRef & msgRef, uint32)
   {
      if (msgRef()) {printf("Internal thread saw: "); msgRef()->PrintToStream(); return B_NO_ERROR;}
               else {printf("Internal thread exiting\n"); return B_ERROR;}
      
   }
};

// This program exercises the Thread class.
int main(void) 
{
   CompleteSetupSystem css;

   TestThread t;
   if (t.StartInternalThread() == B_NO_ERROR)
   {
      char buf[256];
      while(fgets(buf, sizeof(buf), stdin))
      {
         if (buf[0] == 'q') break;
         MessageRef msg(GetMessageFromPool(1234));
         msg()->AddString("str", buf);
         t.SendMessageToInternalThread(msg);
      }
   }

   printf("Cleaning up...\n");
   t.SendMessageToInternalThread(MessageRef());  // ask internal thread to shut down
   t.WaitForInternalThreadToExit();
   printf("Bye!\n");
   return 0;
}
