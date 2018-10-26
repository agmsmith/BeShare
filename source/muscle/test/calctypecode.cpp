/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */  

#include <stdio.h>
#include <string.h>

// Just a silly program to calculate the decimal expression for
// a 'TYPE' typecode constant (because Linux's gcc doesn't like
// the 'TYPE' format and gives lots of warning messages)
int main(int argc, char ** argv)
{
   if ((argc == 2)&&(strlen(argv[1])==4))
   {
      unsigned char * code = (unsigned char *) argv[1];
      unsigned long v = 0;
      unsigned long m = 1;
      for (int i=3; i>=0; i--) 
      {
         v += (m * ((long)(code[i])));
         m *= 256;
      }
      printf("%lu; // '%s' \n", v, code);
   }
   else printf("Usage Example:  calctypecode MSGG\n");
   return 0;
}
