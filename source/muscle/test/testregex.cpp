/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */  

#include <stdio.h>
#include "regex/StringMatcher.h"

USING_NAMESPACE(muscle);

// Just some quick testing of the StringMatcher class...
int main(int argc, char ** argv) 
{
   if (argc <= 1)
   {
      printf("Usage:  testregex 'pattern' 'str1' 'str2' [...]\n");
      exit(5);
   }

   StringMatcher sm(argv[1]);
   printf("Testing pattern: \"%s\"%s\n", argv[1], sm.IsPatternUnique()?" (UNIQUE)":"");
   for (int i=2; i<argc; i++) printf("String [%s] %s the pattern.\n", argv[i], sm.Match(argv[i]) ? "matches" : "doesn't match");

   return(0);
}
