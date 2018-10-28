/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */  

#include <stdio.h>
#include "support/MuscleSupport.h"

USING_NAMESPACE(muscle);

template<typename T> inline void testWidth(const T & val, int expectedSize, const char * name)
{
   printf("%s: size=%i expected %i (%s)\n", name, sizeof(val), expectedSize, (sizeof(val) == expectedSize) ? "pass" : "ERROR, WRONG SIZE!");
}

// This program makes sure that the MUSCLE typedefs have the proper bit-widths.
int main(int, char **) 
{
   printf("Testing MUSCLE typedefs to make sure they are defined to the correct bit-widths...\n");
   {int8   i = 0; testWidth(i, 1, "  int8");}
   {uint8  i = 0; testWidth(i, 1, " uint8");}
   {int16  i = 0; testWidth(i, 2, " int16");}
   {uint16 i = 0; testWidth(i, 2, "uint16");}
   {int32  i = 0; testWidth(i, 4, " int32");}
   {uint32 i = 0; testWidth(i, 4, "uint32");}
   {int64  i = 0; testWidth(i, 8, " int64");}
   {uint64 i = 0; testWidth(i, 8, "uint64");}
   {float  i = 0; testWidth(i, 4, " float");}
   {double i = 0; testWidth(i, 8, "double");}
   printf("Testing complete.\n");
   return 0;
}
