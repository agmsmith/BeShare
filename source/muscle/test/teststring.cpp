/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */  

#include <stdio.h>

#include "message/Message.h"
#include "util/String.h"
#include "util/MiscUtilityFunctions.h"
#include "util/NetworkUtilityFunctions.h"
#include "support/Tuple.h"  // make sure the template operators don't mess us up

USING_NAMESPACE(muscle);

#define TEST(x) if (!(x)) printf("Test failed, line %i\n",__LINE__)

// This program exercises the String class.
int main(void) 
{
#ifdef TEST_PARSE_ARGS
   while(1)
   {
      char base[512];  printf("Enter string: "); fflush(stdout); gets(base);

      Message args;
      if (ParseArgs(base, args) == B_NO_ERROR)
      {
         printf("Parsed: "); args.PrintToStream();
      }
      else printf("Parse failed!\n");
   }
#endif

#ifdef TEST_REPLACE_METHOD
   while(1)
   {
      char base[512];      printf("Enter string:    "); fflush(stdout); gets(base);
      char replaceMe[512]; printf("Enter replaceMe: "); fflush(stdout); gets(replaceMe);
      char withMe[512];    printf("Enter withMe:    "); fflush(stdout); gets(withMe);

      String b(base);
      int32 ret = b.Replace(replaceMe, withMe);
      printf("%li: Afterwards, [%s] (%lu)\n", ret, b(), b.Length());
   }
#endif

   int16 dozen = 13;
   String aString = String("%1 is a %2 %3").Arg(dozen).Arg("baker's dozen").Arg(3.14159);
   printf("arg string = [%s]\n", aString());

   String x;
   x = '5';
   printf("x=[%s]\n", x());

   String temp;
   temp.SetCstr("1234567890", 3);
   printf("123=[%s]\n", temp());
   temp.SetCstr("1234567890");
   printf("%s\n", temp());

   String rem("Hello sailor");
   printf("[%s]\n", (rem+"maggot"-"sailor")());
   rem -= "llo";
   printf("[%s]\n", rem());
   rem -= "xxx";
   printf("[%s]\n", rem());
   rem -= 'H';
   printf("[%s]\n", rem());
   rem -= 'r';
   printf("[%s]\n", rem());
   rem -= rem;
   printf("[%s]\n", rem());

   String test = "hello";
   test = test + " and " + " goodbye " + '!';
   printf("test=[%s]\n", test());
   return 0;
}
