/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */  

#include <stdio.h>
#include "dataio/RS232DataIO.h"

USING_NAMESPACE(muscle);

#define TEST(x) if ((x) != B_NO_ERROR) printf("Test failed, line %i\n",__LINE__)
#define TESTSIZE(x) if ((x) < 0) printf("Test failed, line %i\n",__LINE__)

int main(int argc, char ** argv) 
{
   RS232DataIO io("/dev/ttyS0", 38400, true);
   if (io.IsPortAvailable())
   {
      char buf[1024];
      int32 ret = 1;
      int c = 0;
      while(ret >= 0)
      {
         ret = io.Read(buf, sizeof(buf));
         printf("Read %li bytes: [", ret);
         for (uint32 i=0; i<ret; i++) printf("%02x ", buf[i]);
         printf("]\n");
         printf("aka [");
         for (uint32 j=0; j<ret; j++) printf("%c", buf[j]); printf("\n");

         if (++c == 20)
         {
            uint8 bytes[] = {0xF0, 0x1F, 0x7E, 0x00, 0x3F, 0x0A, 0x00, 0x1A, 0xF7};
            printf("Sent %li/%li bytes\n", io.Write(bytes, sizeof(bytes)), sizeof(bytes));
            c = 0;
         }
      }
      printf("Done!\n");
   }
   else printf("Couldn't open port!\n");

   return 0;
}
