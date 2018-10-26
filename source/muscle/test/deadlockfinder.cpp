/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */  

#include <stdio.h>

#include "util/Hashtable.h"
#include "util/Queue.h"
#include "util/String.h"
#include "util/StringTokenizer.h"
#include "util/MiscUtilityFunctions.h"
#include "util/NetworkUtilityFunctions.h"

// This program is useful for finding potential synchronization deadlocks in your multi-threaded
// application.  To use it, instrument your program by adding PLOCK() calls before each of your
// Mutex::Lock() calls, and PUNLOCK() calls before each of your Mutex::Unlock() calls, like this:
//
// [... in MyFile.cpp somewhere ...]
//   Mutex myMutex;
//   PLOCK("myMutex", &myMutex);             // INSERT THIS LINE FOR DEBUGGING WITH DEADLOCKFINDER
//   if (myMutex.Lock() == B_NO_ERROR)
//   {
//      .... [do some stuff]
//      PUNLOCK("myMutex", &myMutex);        // INSERT THIS LINE FOR DEBUGGING WITH DEADLOCKFINDER
//      myMutex.Unlock();
//   }
//
// Then run  your program and have it output stdout to a file, like this;
//
// ./mymultithreadedprogram > outfile
//
// Once you have exercised your program in the normal manner, run deadlockfinder like this:
//
// ./deadlockfinder <outfile
//
// where outfile is the output file your program generated to stdout.  Then deadlockfinder will
// parse through the output and build up a catalog of all the locking sequences that were used.
// When it is done, it will print out all the unique multi-lock locking sequences, and you can then go
// through them and make sure that all the locks were always locked in a well-defined order.
// For example, if you saw output like this:
//
// SEQUENCE 25/50:
//     0.  LockA & 0x102ec580 & MyFile.cpp:239
//     1.  LockB & 0x101b0dfc & AnotherFile.cpp:141
//
// [...]
//
// SEQUENCE 37/50:
//     0.  LockB & 0x102ec580 & MyFile.cpp:239
//     1.  LockA & 0x101b0dfc & AnotherFile.cpp:141
//
// That would indicate a potential deadlock condition, since the two locks were not
// always being locked in the same order.  (i.e. thread 1 might lock LockA, then thread 2
// might lock LockB, and then thread 1 would try to lock LockB and block forever, while
// thread 2 would try to lock LockA and block forever... the result being that some or
// all of your program would hang.


USING_NAMESPACE(muscle);

static void PrintState(const Hashtable<int, Queue<String> > & state)
{
   printf("--------- Begin Current state ------------\n");
   int nextThread;
   const Queue<String> * nextQ;
   HashtableIterator<int, Queue<String> > iter(state);
   while(iter.GetNextKeyAndValue(nextThread, nextQ) == B_NO_ERROR)
   {
      printf("  Thread %i:\n", nextThread);
      for (uint32 i=0; i<nextQ->GetNumItems(); i++) printf("    %lu. %s\n", i, (*nextQ)[i]());;
   }
   printf("--------- End Current state ------------\n");
}

static bool LogsMatchExceptForLocation(const Queue<String> & q1, const Queue<String> & q2)
{
   if (q1.GetNumItems() != q2.GetNumItems()) return false;

   for (uint32 i=0; i<q1.GetNumItems(); i++)
   {
      String s1 = q1[i]; {int32 lastAmp = s1.LastIndexOf('&'); s1 = s1.Substring(0, lastAmp);}
      String s2 = q2[i]; {int32 lastAmp = s2.LastIndexOf('&'); s2 = s2.Substring(0, lastAmp);}
      if (s1 != s2) return false;
   }
   return true;
}

// This program reads debug output to look for potential deadlocks
int main(void) 
{
   Queue<Queue<String> > maxLogs;

   Hashtable<int, Queue<String> > curLockState;
   char buf[1024];
   while(fgets(buf, sizeof(buf), stdin))
   {
      String s = buf; s = s.Trim();
      StringTokenizer tok(s());
      String threadID = tok();
      String action   = tok();
      if ((action == "plock")||(action == "punlock"))
      {
         String mutex    = tok(); if (mutex.StartsWith("m=")) mutex = mutex.Substring(2);
         String name     = tok(); if (name.StartsWith("[")) name = name.Substring(1); if (name.EndsWith("]")) name = name.Substring(0, name.Length()-1);
         String location = tok();

         //printf("thread=[%s] action=[%s] mutex=[%s] name=[%s] location=[%s]\n", threadID(), action(), mutex(), name(), location());
         String lockDesc = name + " & " + mutex + " & " + location;

         int thread = atoi(threadID());
         if (action == "plock")
         {
            Queue<String> * q = curLockState.GetOrPut(thread, Queue<String>());
            q->AddTail(lockDesc);

            if (q->GetNumItems() > 1)
            {
               // See if we have this queue logged anywhere already
               bool foundLog = false;
               for (uint32 i=0; i<maxLogs.GetNumItems(); i++)
               {
                  if (LogsMatchExceptForLocation(maxLogs[i], *q))
                  {
                     foundLog = true;
                     break;
                  }
               }
               if (foundLog == false) maxLogs.AddTail(*q);
            }
         }
         else
         {
            Queue<String> * q = curLockState.Get(thread);
            if (q)
            {
               // Find the last instance of this mutex in our list
               String lockName = lockDesc; {int32 lastAmp = lockName.LastIndexOf('&'); lockName = lockName.Substring(0, lastAmp);}
               bool foundLock = false;
               for (int32 i=q->GetNumItems()-1; i>=0; i--)
               {
                  String s = (*q)[i]; {int32 lastAmp = s.LastIndexOf('&'); s = s.Substring(0, lastAmp);}
                  if (s == lockName)
                  {
                     foundLock = true;
                     q->RemoveItemAt(i);
                     break;
                  }
               }
               if (foundLock == false) printf("ERROR:  thread %i is unlocking a lock he never locked!!! [%s]\n", thread, lockDesc());
               if (q->GetNumItems() == 0) curLockState.Remove(thread);
            }
            else printf("ERROR:  thread %i is unlocking when he has nothing locked!!! [%s]\n", thread, lockDesc());
         }
         //PrintState(curLockState);
      }
   }

   printf("------------------- BEGIN LOCK SEQUENCES -----------------\n");
   for (uint32 i=0; i<maxLogs.GetNumItems(); i++)
   {
      printf("\nUNIQUE SEQUENCE %lu/%lu:\n", i, maxLogs.GetNumItems());
      const Queue<String> & seq = maxLogs[i];
      if (seq.GetNumItems() > 1) for (uint32 j=0; j<seq.GetNumItems(); j++) printf("   %lu.  %s\n", j, seq[j]());
   }
   printf("\n------------------- END LOCK SEQUENCES -----------------\n");
   return 0;
}
