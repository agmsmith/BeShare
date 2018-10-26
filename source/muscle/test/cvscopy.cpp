#include <stdio.h>
#include "util/Queue.h"
#include "util/String.h"

using namespace muscle;

static void FlushAdds(String & s)
{
   if (s.Length() > 0)
   {
      printf("cvs add %s\n", s());
      s = "";
   }
}

/** This program takes a list of file paths on stdin and an input directory as an argument,
  * and creates a set of CVS commands that will update a CVS repository to contain the files
  * and directories listed on stdin.
  * The list of files can be created via "tar tf archive.tar", etc.
  *
  * Note that for new repositories, cvs import can do the same job as this utility; probably
  * better.  But this utility is useful when you need to bulk-upgrade an existing CVS 
  * repository from a non-CVS archive (e.g. if you are keeping 3rd party code in CVS
  * and want to upgrade your CVS repository to the new release)
  *
  * Note that this script does NOT handle the removal of obsolete files from your CVS
  * repository.  If you care about that, you'll have to do it by hand.
  *
  * -Jeremy Friesner (jaf@lcsaudio.com)
  *
  */
int main(int argc, char ** argv)
{
   if (argc < 2)
   {
      printf("Usage:  cvscopy input_folder <filelist.txt\n");
      return 10;
   }

   String inPath = argv[1];
   if (inPath.EndsWith("/") == false) inPath += '/';
   printf("#!/bin/sh\n");
   printf("# Creating commands to copy files from input folder [%s]\n\n", inPath());

   Queue<String> mkdirs;
   Queue<String> copies;

   char buf[2048];
   while(fgets(buf, sizeof(buf), stdin))
   {
      String next(buf);
      next=next.Trim();
      if ((next.EndsWith("/CVS") == false)&&(next.IndexOf("/CVS/") < 0))  // don't copy CVS dirs!
      {
         next.Replace("\'", "\\\'");
         if (next.EndsWith("/")) mkdirs.AddTail(next);
                            else copies.AddTail(next);
      }
   }

   for (uint32 i=0; i<mkdirs.GetNumItems(); i++) printf("mkdir \"./%s\"\n", mkdirs[i]());
   for (uint32 i=0; i<copies.GetNumItems(); i++) printf("cp \"%s%s\" \"./%s\"\n", inPath(), copies[i](), copies[i]());
  
   // Directory adds must be done separately, since if some fail we want the others to continue
   for (uint32 i=0; i<mkdirs.GetNumItems(); i++) printf("cvs add \"./%s\"\n", mkdirs[i]());

   // File adds can be batched together, since already-present files won't cause the whole command to fail
   String temp;
   for (uint32 i=0; i<copies.GetNumItems(); i++)
   {
      const int MAX_LINE_LENGTH = 2048;
      const String & next = copies[i];
      if (next.Length() + temp.Length() >= MAX_LINE_LENGTH-10) FlushAdds(temp);
      temp += String("\"./%1\" ").Arg(next);
   } 
   FlushAdds(temp);

   return 0;
}
