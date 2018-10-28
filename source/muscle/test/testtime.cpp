/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */  

#include <stdio.h>
#include "system/SetupSystem.h"
#include "util/MiscUtilityFunctions.h"

USING_NAMESPACE(muscle);

static float GetDiffHours(uint64 t1, uint64 t2) {return (float) ((double)((int64)t1 - (int64) t2))/(60.0 * 60.0 * 1000000.0);}

// This program is used to test out muscle's time/date interpretation functions
int main(int argc, char ** argv) 
{
   CompleteSetupSystem css;

   Message argsMsg;
   ParseArgs(argc, argv, argsMsg);
   HandleStandardDaemonArgs(argsMsg);

   uint64 epoch = 0;
   printf("epoch time (UTC) = %s\n", GetHumanReadableTimeString(epoch, MUSCLE_TIMEZONE_UTC)());
   printf("epoch time (loc) = %s\n", GetHumanReadableTimeString(epoch, MUSCLE_TIMEZONE_LOCAL)());

   uint64 nowLocal = GetCurrentTime64(MUSCLE_TIMEZONE_LOCAL);
   String nowLocalStr = GetHumanReadableTimeString(nowLocal, MUSCLE_TIMEZONE_LOCAL);
   printf("NOW (Local) = " UINT64_FORMAT_SPEC " = %s\n", nowLocal, nowLocalStr());
   uint64 reparsedLocal = ParseHumanReadableTimeString(nowLocalStr(), MUSCLE_TIMEZONE_LOCAL);
   printf("   reparsed = " UINT64_FORMAT_SPEC " (diff=%.1f hours)\n", reparsedLocal, GetDiffHours(reparsedLocal, nowLocal));

   uint64 nowUTC = GetCurrentTime64(MUSCLE_TIMEZONE_UTC);
   String nowUTCStr = GetHumanReadableTimeString(nowUTC, MUSCLE_TIMEZONE_LOCAL);
   printf("NOW (UTC)   = " UINT64_FORMAT_SPEC " = %s\n            (or, in local terms, %s)\n", nowUTC, nowUTCStr(), GetHumanReadableTimeString(nowUTC, MUSCLE_TIMEZONE_UTC)());

   uint64 reparsedUTC = ParseHumanReadableTimeString(nowUTCStr(), MUSCLE_TIMEZONE_LOCAL);
   printf("   reparsed = " UINT64_FORMAT_SPEC " (diff=%.1f hours)\n", reparsedUTC, GetDiffHours(reparsedUTC, nowUTC));

   printf("The offset between local time and UTC is %.1f hours.\n", GetDiffHours(nowLocal, nowUTC));
   return 0;
}
