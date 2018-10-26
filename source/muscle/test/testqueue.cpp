/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */  

#include <stdio.h>
#include "util/Queue.h"
#include "util/String.h"
#include "util/StringTokenizer.h"

USING_NAMESPACE(muscle);

#define TEST(x) if ((x) != B_NO_ERROR) printf("Test failed, line %i\n",__LINE__)

void PrintToStream(const Queue<int> & q);
void PrintToStream(const Queue<int> & q)
{
   printf("Queue state is:\n");
   for (int i=0; i<q.GetNumItems(); i++)
   {
/*
      int val;
      TEST(q.GetItemAt(i, val));    
      printf("%i -> %i\n",i,val);
*/
      printf("%i -> %i\n",i,q[i]);
   }
}

int SortInts(const int & i1, const int & i2, void *);
int SortInts(const int & i1, const int & i2, void *)
{
   return i1-i2;
}

int SortStrings(const String & i1, const String & i2, void *);
int SortStrings(const String & i1, const String & i2, void *)
{
   return strcmp(i1(), i2());
}

// This program exercises the Queue class.
int main(void) 
{
#ifdef TEST_SWAP_METHOD
   while(1)
   {
      char qs1[512]; printf("Enter q1: "); fflush(stdout); gets(qs1);
      char qs2[512]; printf("Enter q2: "); fflush(stdout); gets(qs2);

      Queue<int> q1, q2;      
      StringTokenizer t1(qs1), t2(qs2);
      const char * s;
      while((s = t1()) != NULL) q1.AddTail(atoi(s));
      while((s = t2()) != NULL) q2.AddTail(atoi(s));
      printf("T1Before="); PrintToStream(q1);
      printf("T2Before="); PrintToStream(q2);
      q1.SwapContents(q2);
      printf("T1After="); PrintToStream(q1);
      printf("T2After="); PrintToStream(q2);
   }
#endif

   const int testSize = 15;
   Queue<int> q;

   int vars[] = {5,6,7,8,9,10,11,12,13,14,15};

   printf("ADDTAIL TEST\n");
   {
      for (int i=0; i<testSize; i++) 
      {
         TEST(q.AddTail(i));
         printf("len=%lu/%lu\n", q.GetNumItems(), q.GetNumAllocatedItemSlots());
      }
   } 

   printf("AddTail array\n");
   q.AddTail(vars, ARRAYITEMS(vars));
   PrintToStream(q);

   printf("AddHead array\n");
   q.AddHead(vars, ARRAYITEMS(vars));
   PrintToStream(q);

   printf("REPLACEITEMAT TEST\n");
   {
      for (int i=0; i<testSize; i++) 
      {
         TEST(q.ReplaceItemAt(i, i+10));
         PrintToStream(q);
      }
   } 

   printf("INSERTITEMAT TEST\n");
   {
      for (int i=0; i<testSize; i++) 
      {
         TEST(q.InsertItemAt(i,i));
         PrintToStream(q);
      }
   }

   printf("REMOVEITEMAT TEST\n");
   {
      for (int i=0; i<testSize; i++) 
      {
         TEST(q.RemoveItemAt(i));
         PrintToStream(q);
      }
   }

   printf("SORT TEST 1\n");
   {
      q.Clear();
      for (int i=0; i<testSize; i++)
      {
         int next = rand()%255;
         TEST(q.AddTail(next));
         printf("Added item %i = %i\n", i, q[i]);
      }
      printf("sorting ints...\n");
      q.Sort(SortInts);
      for (int j=0; j<testSize; j++) printf("Now item %i = %i\n", j, q[j]);
   }

   printf("SORT TEST 2\n");
   {
      Queue<String> q2;
      for (int i=0; i<testSize; i++)
      {
         int next = rand()%255;
         char buf[64];
         sprintf(buf, "%i", next);
         TEST(q2.AddTail(buf));
         printf("Added item %i = %s\n", i, q2[i].Cstr());
      }
      printf("sorting strings...\n");
      q2.Sort(SortStrings);
      for (int j=0; j<testSize; j++) printf("Now item %i = %s\n", j, q2[j].Cstr());
   }

   printf("REVERSE TEST\n");
   {
      q.Clear();
      for (int i=0; i<testSize; i++) TEST(q.AddTail(i));
      q.ReverseItemOrdering();
      for (int j=0; j<testSize; j++) printf("After reverse, %i->%i\n", j, q[j]);
   }

   printf("CONCAT TEST 1\n");
   {
      q.Clear();
      Queue<int> q2;
      for (int i=0; i<testSize; i++) 
      {
         TEST(q.AddTail(i));
         TEST(q2.AddTail(i+100));
      }
      q.AddTail(q2);
      for (int j=0; j<q.GetNumItems(); j++) printf("After concat, %i->%i\n", j, q[j]);
   }

   printf("CONCAT TEST 2\n");
   {
      q.Clear();
      Queue<int> q2;
      for (int i=0; i<testSize; i++) 
      {
         TEST(q.AddTail(i));
         TEST(q2.AddTail(i+100));
      }
      q.AddHead(q2);
      for (int j=0; j<q.GetNumItems(); j++) printf("After concat, %i->%i\n", j, q[j]);
   }
   {
      printf("GetArrayPointer() test\n");
      uint32 len;
      int * a;
      for (uint32 i=0; (a = q.GetArrayPointer(i, len)) != NULL; i++)
      {
         printf("SubArray %i: %lu items: ", i, len);
         for (uint32 j=0; j<len; j++) printf("%i, ", a[j]);
         printf("\n");
      }
   }
   return 0;
}
