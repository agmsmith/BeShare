/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */  

#include <stdio.h>
#include "system/SystemInfo.h"
#include "system/SetupSystem.h"

USING_NAMESPACE(muscle);

static String GetSystemPathAux(uint32 whichPath)
{
   String ret;
   if (GetSystemPath(whichPath, ret) != B_NO_ERROR) ret = "<error>";
   return ret;
}

// This program prints out the results returned by the SystemInfo functions.  The implementation
// of these functions is very OS-specific, so this is a helpful way to test them on each platform
// to ensure they are working properly.
int main(void) 
{
   CompleteSetupSystem css;

   printf("TestSystemInfo:  OS name  = [%s]\n", GetOSName());
   printf("                 file sep = [%s]\n", GetFilePathSeparator());
   printf("                 cur path = [%s]\n", GetSystemPathAux(SYSTEM_PATH_CURRENT)());
   printf("                 exe path = [%s]\n", GetSystemPathAux(SYSTEM_PATH_EXECUTABLE)());
   printf("                 tmp path = [%s]\n", GetSystemPathAux(SYSTEM_PATH_TEMPFILES)());
   return 0;
}
