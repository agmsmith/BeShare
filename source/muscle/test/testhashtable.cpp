/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */  

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "util/Hashtable.h"
#include "util/String.h"
#include "util/StringTokenizer.h"
#include "util/TimeUtilityFunctions.h"

USING_NAMESPACE(muscle);

int _state = 0;

static void bomb(const char * fmt, ...);
void bomb(const char * fmt, ...)
{
   va_list va;
   va_start(va, fmt);
   vprintf(fmt, va);
   va_end(va);
   printf("EXITING DUE TO ERROR (state = %i)!\n", _state);
   exit(10);
}

int MyCompareFunc(const int & s1, const int & s2, void *);
int MyCompareFunc(const int & s1, const int & s2, void *)
{
   return s1-s2;
}

static int DoInteractiveTest();
static int DoInteractiveTest()
{
   Hashtable<int, String> table;

   // Prepopulate the table, for convenience
   for (int i=9; i>=0; i--)
   {
      char buf[32];
      sprintf(buf, "%i", i);
      table.Put(i, buf);
   }

   // Let's see if this reverses it
   {
      HashtableIterator<int, String> rev(table);
      int next;
      while(rev.GetNextKey(next) == B_NO_ERROR) table.MoveToFront(next);
   }

   // turn on auto-sorting
   table.SetKeyCompareFunction(MyCompareFunc);
   table.SetAutoSortMode(table.AUTOSORT_BY_KEY);

   while(true)
   {
      HashtableIterator<int, String> iter(table);

      {
         bool first = true;
         printf("\nCurrent contents: ");
         int nextKey;
         while(iter.GetNextKey(nextKey) == B_NO_ERROR)
         {
            String * nextValue = iter.GetNextValue();
            MASSERT((atoi(nextValue->Cstr()) == nextKey), "value/key mismatch A!\n");
            printf("%s%i", first?"":", ", nextKey);
            first = false;
         }
         printf(" (size=%lu)\nEnter command: ", table.GetNumItems()); fflush(stdout);
      }

      char buf[512];
      if (fgets(buf, sizeof(buf), stdin))
      {
         StringTokenizer tok(buf, " ");
         const char * arg0 = tok();
         const char * arg1 = tok(); 
         const char * arg2 = tok(); 

         char command = arg0 ? arg0[0] : '\0';

         // For extra fun, let's put a half-way-done iterator here to see what happens
         printf("Concurrent: ");
         HashtableIterator<int, String> iter(table);
         bool first = true;
         int nextKey;
         if (table.GetNumItems() > 0)
         {
            for (int i=table.GetNumItems()/2; i>=0; i--) 
            {
               MASSERT(iter.GetNextKey(nextKey) == B_NO_ERROR, "Not enough keys in table!?!?\n");
               String * nextValue = iter.GetNextValue();
               MASSERT((atoi(nextValue->Cstr()) == nextKey), "value/key mismatch B!\n");

               printf("%s%i", first?"":", ", nextKey);
               first = false;
            }
         }

         switch(command)
         { 
            case 'F':  
               if ((arg1)&&(arg2))
               {
                  printf("%s(%s %i before %i)", first?"":", ", (table.MoveToBefore(atoi(arg1),atoi(arg2)) == B_NO_ERROR) ? "Befored" : "FailedToBefored", atoi(arg1), atoi(arg2));
                  first = false;
               }
               else printf("(No arg1 or arg2!)");
            break;
            
            case 'B':  
               if ((arg1)&&(arg2))
               {
                  printf("%s(%s %i behind %i)", first?"":", ", (table.MoveToBehind(atoi(arg1),atoi(arg2)) == B_NO_ERROR) ? "Behinded" : "FailedToBehind", atoi(arg1), atoi(arg2));
                  first = false;
               }
               else printf("(No arg1 or arg2!)");
            break;
            
            case 'f':  
               if (arg1)
               {
                  printf("%s(%s %i)", first?"":", ", (table.MoveToFront(atoi(arg1)) == B_NO_ERROR) ? "Fronted" : "FailedToFront", atoi(arg1));
                  first = false;
               }
               else printf("(No arg1!)");
            break;
            
            case 'b':  
               if (arg1)
               {
                  printf("%s(%s %i)", first?"":", ", (table.MoveToBack(atoi(arg1)) == B_NO_ERROR) ? "Backed" : "FailedToBack", atoi(arg1));
                  first = false;
               }
               else printf("(No arg1!)");
            break;
            
            case 'p':  
               if (arg1)
               {
                  printf("%s(%s %i)", first?"":", ", (table.Put(atoi(arg1), arg1) == B_NO_ERROR) ? "Put" : "FailedToPut", atoi(arg1));
                  first = false;
               }
               else printf("(No arg1!)");
            break;
            
            case 'r':  
               if (arg1)
               {
                  printf("%s(%s %i)", first?"":", ", (table.Remove(atoi(arg1)) == B_NO_ERROR) ? "Removed" : "FailedToRemove", atoi(arg1));
                  first = false;
               }
               else printf("(No arg1!)");
            break;

            case 'c':  
               printf("%s(Clearing table)", first?"":", ");
               table.Clear();
               first = false;
            break;

            case 's':
               printf("%s(Sorting table)", first?"":", ");
               table.Sort();
               first = false;
            break;

            case 'q':  
               printf("%sQuitting\n", first?"":", ");
               first = false;
               return 0;
            break;
         }

         while(iter.GetNextKey(nextKey) == B_NO_ERROR)
         {
            String * nextValue = iter.GetNextValue();
            MASSERT((atoi(nextValue->Cstr()) == nextKey), "value/key mismatch C!\n");

            printf("%s%i", first?"":", ", nextKey);
            first = false;
         }
         printf("\n");
      }
      else return 0;
   }
}

static void CheckTable(Hashtable<int, bool> & table, uint32 numItems, bool backwards)
{
   uint32 count = 0;
   int last = backwards ? RAND_MAX : 0;
   int key;
   HashtableIterator<int, bool> iter(table,backwards?HTIT_FLAG_BACKWARDS:0);
   while(iter.GetNextKey(key) == B_NO_ERROR) 
   {
      if (backwards ? (last < key) : (last > key)) printf("ERROR!  Sort out of order in %s traversal!!!!\n", backwards?"backwards":"forwards");
      key = last;
      count++;
   }
   if (count != numItems) printf("ERROR!  Count is different!  %lu vs %lu\n", count, numItems);
}



// This program exercises the Hashtable class.
int main(int argc, char ** argv)
{
   if ((argc > 1)&&(strcmp(argv[1], "inter") == 0)) return DoInteractiveTest();

   // Test the sort algorithm for efficiency and correctness
   {
      printf("Preparing large table for sort...\n");

      const uint32 numItems = 100000;
      Hashtable<int, bool> table;
      for (uint32 i=0; i<numItems; i++) table.Put((int)rand(), true);
      uint32 actualNumItems = table.GetNumItems();  // may be smaller than numItems, due to duplicate values!

      printf("Sorting...\n");
      uint64 start = GetRunTime64();
      table.SortByKey(MyCompareFunc);
      uint64 end = GetRunTime64();

      printf("Time to sort %lu items: " UINT64_FORMAT_SPEC "ms\n", numItems, (end-start)/1000);

      // Check the resulting sorted table for correctness in both directions
      CheckTable(table, actualNumItems, false);
      CheckTable(table, actualNumItems, true);
   }

   Hashtable<String, String> table;
   {
      table.Put("Hello", "World");
      table.Put("Peanut Butter", "Jelly");
      table.Put("Ham", "Eggs");
      table.Put("Pork", "Beans");
      table.Put("Slash", "Dot");
      table.Put("Data", "Mining");  
      table.Put("TestDouble", "Play");
      table.Put("Abbot", "Costello");
      table.Put("Laurel", "Hardy");
      table.Put("Thick", "Thin");
      table.Put("Butter", "Parkay");
      table.Put("Total", "Carnage");
      table.Put("Summer", "Time");
      table.Put("Terrible", "Twos");

      printf("String table has %i entries in it...\n", table.GetNumItems());

      {
         printf("Test partial backwards iteration\n");
         HashtableIterator<String, String> iter(table, "Slash", HTIT_FLAG_BACKWARDS);
         String key, val;
         while(iter.GetNextKeyAndValue(key, val) == B_NO_ERROR) printf("[%s] <-> [%s]\n", key(), val());
      }

      String lookup;
      if (table.Get(String("Hello"), lookup) == B_NO_ERROR) printf("Hello -> %s\n", lookup());
                                                       else printf("Lookup 1 failed.\n");
      if (table.Get(String("Peanut Butter"), lookup) == B_NO_ERROR) printf("Peanut Butter -> %s\n", lookup());
                                                               else printf("Lookup 2 failed.\n");


      HashtableIterator<String, String> st(table);
      String nextKeyString;
      String nextValueString;

      printf("Testing normal traveral with GetNextKey() and Get()\n");
      while(st.GetNextKey(nextKeyString) == B_NO_ERROR)
      {
         status_t ret = table.Get(nextKeyString, nextValueString);
         printf("t1 = %i %s: %s -> %s\n", st.HasMoreKeys(), (ret == B_NO_ERROR) ? "OK" : "ERROR", nextKeyString(), nextValueString());
      }

      printf("Testing normal traveral with GetNextKey() and GetNextValue()\n");
      st = table.GetIterator();
      while(st.GetNextKey(nextKeyString) == B_NO_ERROR)
      {
         status_t ret = st.GetNextValue(nextValueString);
         printf("t2 = %i %s: %s -> %s\n", st.HasMoreKeys(), (ret == B_NO_ERROR) ? "OK" : "ERROR", nextKeyString(), nextValueString());
      }

      printf("Testing delete-as-you-go traveral\n");
      st = table.GetIterator();
      while(st.GetNextKey(nextKeyString) == B_NO_ERROR)
      {
         status_t ret = st.GetNextValue(nextValueString);
         printf("t3 = %i %s: %s -> %s (tableSize=%lu)\n", st.HasMoreKeys(), (ret == B_NO_ERROR) ? "OK" : "ERROR", nextKeyString(), nextValueString(), table.GetNumItems());
         table.Remove(nextKeyString);
#if 0
         {
            HashtableIterator<String,String> st2(table);
            while(st2.GetNextKey(nextKeyString) == B_NO_ERROR)
            {
               status_t ret = st2.GetNextValue(nextValueString);
               printf("  tx = %i %s: %s -> %s\n", st2.HasMoreKeys(), (ret == B_NO_ERROR) ? "OK" : "ERROR", nextKeyString(), nextValueString());
            }
         }
#endif
      }

      printf("Testing another normal traversal with GetNextKey() and Get(), should be empty\n");
      while(st.GetNextKey(nextKeyString) == B_NO_ERROR)
      {
         status_t ret = table.Get(nextKeyString, nextValueString);
         printf("t1 = %i %s: %s -> %s\n", st.HasMoreKeys(), (ret == B_NO_ERROR) ? "OK" : "ERROR", nextKeyString(), nextValueString());
      }

      Hashtable<uint32, const char *> sillyTable;
      sillyTable.Put(15, "Fifteen");
      sillyTable.Put(100, "One Hundred");
      sillyTable.Put(150, "One Hundred and Fifty");
      sillyTable.Put(200, "Two Hundred");
      sillyTable.Put((uint32)-1, "2^31 - 1!");
      if (sillyTable.ContainsKey((uint32)-1) == false) bomb("large value failed!");

      // Test the GetNextKeyAndValue() methods
      {
         {
            uint32 nextKey;
            const char * nextValue;
            printf("GNKAV 1:\n");
            HashtableIterator<uint32, const char *> iter(sillyTable);
            while(iter.GetNextKeyAndValue(nextKey, nextValue) == B_NO_ERROR) printf("%lu->%s\n", nextKey, nextValue);
         }
         {
            uint32 nextKey;
            const char ** nextValue;
            printf("GNKAV 2:\n");
            HashtableIterator<uint32, const char *> iter(sillyTable);
            while(iter.GetNextKeyAndValue(nextKey, nextValue) == B_NO_ERROR) printf("%lu->%s\n", nextKey, *nextValue);
         }
         {
            const uint32 * nextKey;
            const char * nextValue;
            printf("GNKAV 3:\n");
            HashtableIterator<uint32, const char *> iter(sillyTable);
            while(iter.GetNextKeyAndValue(nextKey, nextValue) == B_NO_ERROR) printf("%lu->%s\n", *nextKey, nextValue);
         }
         {
            const uint32 * nextKey;
            const char ** nextValue;
            printf("GNKAV 4:\n");
            HashtableIterator<uint32, const char *> iter(sillyTable);
            while(iter.GetNextKeyAndValue(nextKey, nextValue) == B_NO_ERROR) printf("%lu->%s\n", *nextKey, *nextValue);
         }
      }

      const char * temp;
      sillyTable.Get(100, temp);
      sillyTable.Get(101, temp); // will fail
      printf("100 -> %s\n",temp);

      printf("Entries in sillyTable:\n");
      HashtableIterator<uint32, const char *> it(sillyTable);

      uint32 nextKey;
      const char * nextValue;
      while(it.GetNextKey(nextKey) == B_NO_ERROR)
      {
         status_t ret = sillyTable.Get(nextKey, nextValue);
         printf("%i %s: %u -> %s\n", it.HasMoreKeys(), (ret == B_NO_ERROR) ? "OK" : "ERROR", nextKey, nextValue);
      }
      printf("XXX %i\n", it.HasMoreKeys());

      printf("... and by value:\n");
      while(it.GetNextValue(nextValue) == B_NO_ERROR) printf("%i value: %s\n", it.HasMoreValues(), nextValue);
      printf("YYY %i\n", it.HasMoreValues());
   }
   table.Clear();   

   printf("Begin torture test!\n");
   _state = 4;
   bool fastClear = false;
   while(true)
   {
      Hashtable<String, uint32> t;
      for (int numEntries=1; numEntries < 100000; numEntries++)
      {
         int half = numEntries/2;
         bool ok = true;

         printf("%i ", numEntries); fflush(stdout);
         _state = 5;
         {
            for(int i=0; i<numEntries; i++)
            {
               char temp[300];
               sprintf(temp, "%i", i);
               if (t.Put(temp, i) != B_NO_ERROR)
               {
                  printf("Whoops, (hopefully simulated) memory failure!  (Put(%i/%lu) failed) ... recovering\n", i, numEntries);

                  ok = false;
                  numEntries--;  // let's do this one over
                  half = i;    // so the remove code won't freak out about not everything being there
                  break;
               }
            }
         }

         if (ok)
         {
            //printf("Checking that all entries are still there...\n");
            _state = 6;
            {
               if (t.GetNumItems() != numEntries) bomb("ERROR, WRONG SIZE %i vs %i!\n", t.GetNumItems(), numEntries);
               for (int i=numEntries-1; i>=0; i--)
               {
                  char temp[300];
                  sprintf(temp, "%i", i);
                  uint32 tv;
                  if (t.Get(temp, tv) != B_NO_ERROR) bomb("ERROR, MISSING KEY [%s]\n", temp);
                  if (tv != i) bomb("ERROR, WRONG KEY %s != %lu!\n", temp, tv);
               }
            }
      
            //printf("Iterating through table...\n");
            _state = 7;
            {
               uint32 count = 0;
               HashtableIterator<String, uint32> iter(t);
               String nextKey;
               while(iter.GetNextKey(nextKey) == B_NO_ERROR)
               {
                  char buf[300];
                  sprintf(buf, "%i", count);
                  if (nextKey != buf) bomb("ERROR:  iteration was wrong, item %lu was [%s] not [%s]!\n", count, nextKey(), buf);

                  uint32 temp;
                  (void) iter.GetNextValue(temp);
                  if (temp != count) bomb("ERROR:  iteration value was wrong, item %lu was %lu not %lu!i!\n", count, temp, count);

                  count++;
               }
            }

            //printf("Removing the second half of the entries...\n");
            _state = 8;
            {
               for (int i=half; i<numEntries; i++)
               {
                  char temp[300];
                  sprintf(temp, "%i", i);
                  uint32 tv;
                  if (t.Remove(temp, tv) != B_NO_ERROR) bomb("ERROR, MISSING REMOVE KEY [%s] A\n", temp);
                  if (tv != i) bomb("ERROR, REMOVE WAS WRONG VALUE %lu\n", tv);
               }
            }

            //printf("Iterating over only first half...\n");
            _state = 9;
            {
               uint32 sum = 0; 
               for (int i=0; i<half; i++) sum += i;

               uint32 count = 0, checkSum = 0;
               HashtableIterator<String, uint32> iter(t);
               String nextKey;
               while(iter.GetNextKey(nextKey) == B_NO_ERROR) 
               {
                  count++;
                  uint32 temp;
                  (void) iter.GetNextValue(temp);
                  checkSum += temp;
               }
               if (count != half) bomb("ERROR: Count mismatch %i vs %i!\n", count, numEntries);
               if (checkSum != sum)     bomb("ERROR: Sum mismatch %i vs %i!\n", sum, checkSum);
            }
         }

         //printf("Clearing Table (%s)\n", fastClear ? "Quickly" : "Slowly"); 
         _state = 10;
         if (fastClear) t.Clear();
         else
         {
            for (int i=0; i<half; i++)
            {
               char temp[300];
               sprintf(temp, "%i", i);
               uint32 tv;
               if (t.Remove(temp, tv) != B_NO_ERROR) 
               {
                  String blahKey;
                  uint32 blah;
                  HashtableIterator<String, uint32> p(t);
                  while(p.GetNextKey(blahKey) == B_NO_ERROR)
                  {
                     if (p.GetNextValue(blah) == B_NO_ERROR) printf("[%s]->%i ", blahKey(), blah);
                                                        else printf("[%s]->(no value!)\n", blahKey());
                  }
                  printf("\n");
                  bomb("ERROR, MISSING REMOVE KEY [%s] (%i/%i) B\n", temp, i, half);
               }
               if (tv != i) bomb("ERROR, REMOVE WAS WRONG VALUE %lu\n", tv);
            }
         }

         HashtableIterator<String, uint32> paranoia(t);
         if (paranoia.GetNextKey()) bomb("ERROR, ITERATOR CONTAINED ITEMS AFTER CLEAR!\n");

         if (t.GetNumItems() != 0) bomb("ERROR, SIZE WAS NON-ZERO (%lu) AFTER CLEAR!\n", t.GetNumItems());
         fastClear = !fastClear;
      }
      printf("Finished torture test successfully, testing again...\n");
   }
}
