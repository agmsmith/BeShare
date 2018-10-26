/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */  

#include <stdio.h>

#include "util/MiscUtilityFunctions.h"

USING_NAMESPACE(muscle);

// This program exercises the ParseFile() function.
int main(int argc, char ** argv) 
{
   if (argc < 2) 
   {
      printf("Usage:  parsefile <filename> [filename] [...]\n");
      return 5;
   }
   else
   {
      for (int i=1; i<argc; i++)
      {
         FILE * fpIn = fopen(argv[i], "r");
         if (fpIn)
         {
            Message msg;
            if (ParseFile(fpIn, msg) == B_NO_ERROR)
            {
               LogTime(MUSCLE_LOG_INFO, "Parsed contents of file [%s]:\n", argv[i]);
               msg.PrintToStream();
               printf("\n");
            }
            else LogTime(MUSCLE_LOG_ERROR, "Error parsing file [%s]\n", argv[i]);
            fclose(fpIn);
         }
         else LogTime(MUSCLE_LOG_ERROR, "Unable to open file [%s]\n", argv[i]);
      }
      return 0;
   }
}
