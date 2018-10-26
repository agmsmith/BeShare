/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */  

#include <stdio.h>
#include "support/MuscleSupport.h"

USING_NAMESPACE(muscle);

#define TEST(x) if (!(x)) printf("Test failed, line %i\n",__LINE__)

static const int ARRAYLEN=640000;
static int16  origArray16[ARRAYLEN];
static int32  origArray32[ARRAYLEN];
static int64  origArray64[ARRAYLEN];
static float  origArrayFloat[ARRAYLEN];
static double origArrayDouble[ARRAYLEN];

static void PrintBytes(const void * b, int num)
{
   const uint8 * b8 = (const uint8 *) b;
   for (int i=0; i<num; i++) printf("%02x ", b8[i]); printf("\n");
}

static void Fail(const char * name, const void * orig, const void * xChange, const void * backAgain, int numBytes)
{
   printf("Test [%s] failed, code is buggy!!!\n", name);
   printf("   Orig: "); PrintBytes(orig, numBytes);
   printf("   Xchg: "); PrintBytes(xChange, numBytes);
   if (backAgain) {printf("   Back: "); PrintBytes(backAgain, numBytes);}
   exit(10);
}

static void CheckSwap(const char * title, void * oldVal, void * newVal, int numBytes)
{
   const uint8 * old8 = (const uint8 *) oldVal;
   const uint8 * new8 = (const uint8 *) newVal;
   for (int i=0; i<numBytes; i++) if (old8[i] != new8[numBytes-(i+1)]) Fail(title, oldVal, newVal, NULL, numBytes);
}

// This program checks the accuracy and measures the speed of muscle's byte-swapping macros.
int main(void) 
{
   {
      for (int i=0; i<ARRAYLEN; i++) 
      {
         int si = (i-(ARRAYLEN/2));
         origArray16[i]     = si;
         origArray32[i]     = si*((int32)1024);
         origArray64[i]     = si*((int64)1024*1024);
         origArrayFloat[i]  = si*100.0f;
         origArrayDouble[i] = si*((double)100000000.0);
      }
   }

#ifdef __BEOS__
   printf("NOTE:  USING BeOS-provided swap functions!\n");
#elif defined(MUSCLE_USE_POWERPC_INLINE_ASSEMBLY)
   printf("NOTE:  USING PowerPC inline assembly swap functions!\n");
#elif defined(MUSCLE_USE_X86_INLINE_ASSEMBLY)
   printf("NOTE:  USING x86 inline assembly swap functions!\n");
#else
   printf("NOTE:  Using unoptimized C++ swap functions.\n");
#endif

   printf("testing B_SWAP_* ...\n");
   {
      for (uint32 i=0; i<ARRAYITEMS(origArray16); i++)
      {
         {
            int16 o16 = origArray16[i];
            int16 x16 = B_SWAP_INT16(o16); CheckSwap("B_SWAP i16", &o16, &x16, sizeof(o16));
            int16 n16 = B_SWAP_INT16(x16); CheckSwap("B_SWAP i16", &x16, &n16, sizeof(x16));
            if ((muscleSwapBytes(o16) != x16) || (n16 != o16)) {Fail("B_SWAP i16", &o16, &x16, &n16, sizeof(int16));}
         }

         {
            int32 o32 = origArray32[i];
            int32 x32 = B_SWAP_INT32(o32); CheckSwap("B_SWAP i32", &o32, &x32, sizeof(o32));
            int32 n32 = B_SWAP_INT32(x32); CheckSwap("B_SWAP i32", &x32, &n32, sizeof(x32));
            if ((muscleSwapBytes(o32) != x32) || (n32 != o32)) {Fail("B_SWAP i32", &o32, &x32, &n32, sizeof(int32));}
         }

         {
            int64 o64 = origArray64[i];
            int64 x64 = B_SWAP_INT64(o64); CheckSwap("B_SWAP i64", &o64, &x64, sizeof(o64));
            int64 n64 = B_SWAP_INT64(x64); CheckSwap("B_SWAP i64", &x64, &n64, sizeof(x64));
            if ((muscleSwapBytes(o64) != x64) || (n64 != o64)) {Fail("B_SWAP i64", &o64, &x64, &n64, sizeof(int64));}
         }

         {
            float ofl = origArrayFloat[i];
            float xfl = B_SWAP_FLOAT(ofl); CheckSwap("B_SWAP float", &ofl, &xfl, sizeof(ofl));
            float nfl = B_SWAP_FLOAT(xfl); CheckSwap("B_SWAP float", &xfl, &nfl, sizeof(xfl));
            if ((muscleSwapBytes(ofl) != xfl) || (nfl != ofl)) {Fail("B_SWAP float", &ofl, &xfl, &nfl, sizeof(float));}
         }

         {
            double odb = origArrayDouble[i];
            double xdb = B_SWAP_DOUBLE(odb); CheckSwap("B_SWAP double", &odb, &xdb, sizeof(odb));
            double ndb = B_SWAP_DOUBLE(xdb); CheckSwap("B_SWAP double", &xdb, &ndb, sizeof(xdb));
            if ((muscleSwapBytes(odb) != xdb) || (ndb != odb)) {Fail("B_SWAP double", &odb, &xdb, &ndb, sizeof(double));}
         }
      }
   }

   printf("testing B_HOST_TO_LENDIAN_* ...\n");
   {
      for (uint32 i=0; i<ARRAYITEMS(origArray16); i++)
      {
         {
            int16 o16 = origArray16[i];
            int16 x16 = B_HOST_TO_LENDIAN_INT16(o16);
            int16 n16 = B_LENDIAN_TO_HOST_INT16(x16);
            if (n16 != o16) {Fail("HOST_TO_LENDIAN i16", &o16, &x16, &n16, sizeof(int16));}
         }

         {
            int32 o32 = origArray32[i];
            int32 x32 = B_HOST_TO_LENDIAN_INT32(o32);
            int32 n32 = B_LENDIAN_TO_HOST_INT32(x32);
            if (n32 != o32) {Fail("HOST_TO_LENDIAN i32", &o32, &x32, &n32, sizeof(int32));}
         }

         {
            int64 o64 = origArray64[i];
            int64 x64 = B_HOST_TO_LENDIAN_INT64(o64);
            int64 n64 = B_LENDIAN_TO_HOST_INT64(x64);
            if (n64 != o64) {Fail("HOST_TO_LENDIAN i64", &o64, &x64, &n64, sizeof(int64));}
         }

         {
            float ofl = origArrayFloat[i];
            float xfl = B_HOST_TO_LENDIAN_FLOAT(ofl);
            float nfl = B_LENDIAN_TO_HOST_FLOAT(xfl);
            if (nfl != ofl) {Fail("HOST_TO_LENDIAN float", &ofl, &xfl, &nfl, sizeof(float));}
         }

         {
            double odb = origArrayDouble[i];
            double xdb = B_HOST_TO_LENDIAN_DOUBLE(odb);
            double ndb = B_LENDIAN_TO_HOST_DOUBLE(xdb);
            if (ndb != odb) {Fail("HOST_TO_LENDIAN double", &odb, &xdb, &ndb, sizeof(double));}
         }
      }
   }

   printf("testing B_LENDIAN_TO_HOST_* ...\n");
   {
      for (uint32 i=0; i<ARRAYITEMS(origArray16); i++)
      {
         {
            int16 o16 = origArray16[i];
            int16 x16 = B_LENDIAN_TO_HOST_INT16(o16);
            int16 n16 = B_HOST_TO_LENDIAN_INT16(x16);
            if (n16 != o16) {Fail("LENDIAN_TO_HOST i16", &o16, &x16, &x16, sizeof(int16));}
         }

         {
            int32 o32 = origArray32[i];
            int32 x32 = B_LENDIAN_TO_HOST_INT32(o32);
            int32 n32 = B_HOST_TO_LENDIAN_INT32(x32);
            if (n32 != o32) {Fail("LENDIAN_TO_HOST i32", &o32, &x32, &n32, sizeof(int32));}
         }

         {
            int64 o64 = origArray64[i];
            int64 x64 = B_LENDIAN_TO_HOST_INT64(o64);
            int64 n64 = B_HOST_TO_LENDIAN_INT64(x64);
            if (n64 != o64) {Fail("LENDIAN_TO_HOST i64", &o64, &x64, &n64, sizeof(int64));}
         }

         {
            float ofl = origArrayFloat[i];
            float xfl = B_LENDIAN_TO_HOST_FLOAT(ofl);
            float nfl = B_HOST_TO_LENDIAN_FLOAT(xfl);
            if (nfl != ofl) {Fail("LENDIAN_TO_HOST float", &ofl, &xfl, &nfl, sizeof(float));}
         }

         {
            double odb = origArrayDouble[i];
            double xdb = B_LENDIAN_TO_HOST_DOUBLE(odb);
            double ndb = B_HOST_TO_LENDIAN_DOUBLE(xdb);
            if (ndb != odb) {Fail("LENDIAN_TO_HOST double", &odb, &xdb, &ndb, sizeof(double));}
         }
      }
   }

   printf("testing B_HOST_TO_BENDIAN_* ...\n");
   {
      for (uint32 i=0; i<ARRAYITEMS(origArray16); i++)
      {
         {
            int16 o16 = origArray16[i];
            int16 x16 = B_HOST_TO_BENDIAN_INT16(o16);
            int16 n16 = B_BENDIAN_TO_HOST_INT16(x16);
            if (n16 != o16) {Fail("HOST_TO_BENDIAN i16", &o16, &x16, &n16, sizeof(int16));}
         }

         {
            int32 o32 = origArray32[i];
            int32 x32 = B_HOST_TO_BENDIAN_INT32(o32);
            int32 n32 = B_BENDIAN_TO_HOST_INT32(x32);
            if (n32 != o32) {Fail("HOST_TO_BENDIAN i32", &o32, &x32, &n32, sizeof(int32));}
         }

         {
            int64 o64 = origArray64[i];
            int64 x64 = B_HOST_TO_BENDIAN_INT64(o64);
            int64 n64 = B_BENDIAN_TO_HOST_INT64(x64);
            if (n64 != o64) {Fail("HOST_TO_BENDIAN i64", &o64, &x64, &n64, sizeof(int64));}
         }

         {
            float ofl = origArrayFloat[i];
            float xfl = B_HOST_TO_BENDIAN_FLOAT(ofl);
            float nfl = B_BENDIAN_TO_HOST_FLOAT(xfl);
            if (nfl != ofl) {Fail("HOST_TO_BENDIAN float", &ofl, &xfl, &nfl, sizeof(float));}
         }

         {
            double odb = origArrayDouble[i];
            double xdb = B_HOST_TO_BENDIAN_DOUBLE(odb);
            double ndb = B_BENDIAN_TO_HOST_DOUBLE(xdb);
            if (ndb != odb) {Fail("HOST_TO_BENDIAN double", &odb, &xdb, &ndb, sizeof(double));}
         }
      }
   }

   printf("testing B_BENDIAN_TO_HOST_* ...\n");
   {
      for (uint32 i=0; i<ARRAYITEMS(origArray16); i++)
      {
         {
            int16 o16 = origArray16[i];
            int16 x16 = B_BENDIAN_TO_HOST_INT16(o16);
            int16 n16 = B_HOST_TO_BENDIAN_INT16(x16);
            if (n16 != o16) {Fail("BENDIAN_TO_HOST i16", &o16, &x16, &n16, sizeof(int16));}
         }

         {
            int32 o32 = origArray32[i];
            int32 x32 = B_BENDIAN_TO_HOST_INT32(o32);
            int32 n32 = B_HOST_TO_BENDIAN_INT32(x32);
            if (n32 != o32) {Fail("BENDIAN_TO_HOST i32", &o32, &x32, &n32, sizeof(int32));}
         }

         {
            int64 o64 = origArray64[i];
            int64 x64 = B_BENDIAN_TO_HOST_INT64(o64);
            int64 n64 = B_HOST_TO_BENDIAN_INT64(x64);
            if (n64 != o64) {Fail("BENDIAN_TO_HOST i64", &o64, &x64, &n64, sizeof(int64));}
         }

         {
            float ofl = origArrayFloat[i];
            float xfl = B_BENDIAN_TO_HOST_FLOAT(ofl);
            float nfl = B_HOST_TO_BENDIAN_FLOAT(xfl);
            if (nfl != ofl) {Fail("BENDIAN_TO_HOST float", &ofl, &xfl, &nfl, sizeof(float));}
         }

         {
            double odb = origArrayDouble[i];
            double xdb = B_BENDIAN_TO_HOST_DOUBLE(odb);
            double ndb = B_HOST_TO_BENDIAN_DOUBLE(xdb);
            if (ndb != odb) {Fail("BENDIAN_TO_HOST double", &odb, &xdb, &ndb, sizeof(double));}
         }
      }
   }

   printf("Correctness test complete.\n");

   printf("Now doing speed testing....\n");

   static const int NUM_ITERATIONS = 500;

   // int16 speed test
   {
      int16 bigArray16[16384];
      {for (uint32 i=0; i<ARRAYITEMS(bigArray16); i++) bigArray16[i] = (int16)i;}

      uint64 beginTime = GetCurrentTime64();
      {
         for (uint32 i=0; i<NUM_ITERATIONS; i++)
         {
            for (uint32 j=0; j<ARRAYITEMS(bigArray16); j++) bigArray16[j] = B_SWAP_INT16(bigArray16[j]);
         }
      }
      uint64 endTime = GetCurrentTime64(); printf("B_SWAP_INT16 exercise took " UINT64_FORMAT_SPEC" ms to complete.\n", (endTime-beginTime)/1000);
   }

   // int32 speed test
   {
      uint32 bigArray32[16384];
      {for (uint32 i=0; i<ARRAYITEMS(bigArray32); i++) bigArray32[i] = i;}

      uint64 beginTime = GetCurrentTime64();
      {
         for (uint32 i=0; i<NUM_ITERATIONS; i++)
         {
            for (uint32 j=0; j<ARRAYITEMS(bigArray32); j++) bigArray32[j] = B_SWAP_INT32(bigArray32[j]);
         }
      }
      uint64 endTime = GetCurrentTime64();
      printf("B_SWAP_INT32 exercise took " UINT64_FORMAT_SPEC" ms to complete.\n", (endTime-beginTime)/1000);
   }

   // int64 speed test
   {
      uint64 bigArray64[16384];
      {for (uint32 i=0; i<ARRAYITEMS(bigArray64); i++) bigArray64[i] = i;}

      uint64 beginTime = GetCurrentTime64();
      {
         for (uint32 i=0; i<NUM_ITERATIONS; i++)
         {
            for (uint32 j=0; j<ARRAYITEMS(bigArray64); j++) bigArray64[j] = B_SWAP_INT64(bigArray64[j]);
         }
      }
      uint64 endTime = GetCurrentTime64();
      printf("B_SWAP_INT64 exercise took " UINT64_FORMAT_SPEC" ms to complete.\n", (endTime-beginTime)/1000);
   }

   // floating point speed test
   {
      float bigArray32[16384];
      {for (uint32 i=0; i<ARRAYITEMS(bigArray32); i++) bigArray32[i] = (float)i;}

      uint64 beginTime = GetCurrentTime64();
      {
         for (uint32 i=0; i<NUM_ITERATIONS; i++)
         {
            for (uint32 j=0; j<ARRAYITEMS(bigArray32); j++) bigArray32[j] = B_SWAP_FLOAT(bigArray32[j]);
         }
      }
      uint64 endTime = GetCurrentTime64();
      printf("B_SWAP_FLOAT exercise took " UINT64_FORMAT_SPEC" ms to complete.\n", (endTime-beginTime)/1000);
   }

   // double precision floating point speed test
   {
      double bigArray64[16384];
      {for (uint32 i=0; i<ARRAYITEMS(bigArray64); i++) bigArray64[i] = (double)i;}

      uint64 beginTime = GetCurrentTime64();
      {
         for (uint32 i=0; i<NUM_ITERATIONS; i++)
         {
            for (uint32 j=0; j<ARRAYITEMS(bigArray64); j++) bigArray64[j] = B_SWAP_DOUBLE(bigArray64[j]);
         }
      }
      uint64 endTime = GetCurrentTime64();
      printf("B_SWAP_DOUBLE exercise took " UINT64_FORMAT_SPEC" ms to complete.\n", (endTime-beginTime)/1000);
   }

   return 0;
}
