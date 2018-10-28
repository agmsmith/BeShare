/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */  

#include <stdio.h>

#include "zlib/ZipFileUtilityFunctions.h"

USING_NAMESPACE(muscle);

#define COMMAND_HELLO   0x1234
#define COMMAND_GOODBYE 0x4321

// This program tests the ZipFileUtilityFunctions functions
int main(int argc, char ** argv)
{
   if (argc <= 1)
   {
      printf("testzip somezipfiletoread.zip [newzipfiletowrite.zip]\n");
      return 0;
   }
   
   MessageRef msg = ReadZipFile(argv[1]);
   if (msg())
   {
      printf("Contents of [%s] as a Message are:\n", argv[1]);
      msg()->PrintToStream();

      if (argc > 2)
      {
         printf("\n\n... writing new .zip file [%s]\n", argv[2]);
         if (WriteZipFile(argv[2], *msg()) == B_NO_ERROR) printf("Creation of [%s] succeeded!\n", argv[2]);
                                                     else printf("Creation of [%s] FAILED!\n", argv[2]);
      }
   }
   else printf("Error reading .zip file [%s]\n", argv[1]);
   
   return 0;
}
