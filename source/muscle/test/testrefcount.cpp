/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */  

#include <stdio.h>
#include "util/String.h"
#include "util/RefCount.h"
#include "util/Queue.h"
#include "util/Hashtable.h"

USING_NAMESPACE(muscle);

class TestClass : public RefCountable
{
public:
   TestClass() : _name("Default") {printf("CountableConstructing [%s] this=%p\n",_name(), this);}
   TestClass(const char * name) : _name(name) {printf("CountableConstructing [%s] this=%p\n",_name(), this);}
   ~TestClass() {printf("CountableDestroying [%s] this=%p\n",_name(), this);}

   const char * GetName() const {return _name();}

private:
   String _name;
};

// This program exercises the Ref class.
int main(void) 
{
   Ref<TestClass>::ItemPool tpool;
   Ref<TestClass>::ItemPool * pool = &tpool;
 
   {
      Ref<TestClass> * refPtr = NULL;
      Ref<TestClass> extraRef;
      {
         printf("1\n");
         Ref<TestClass> testRef(new TestClass("Test"));
         printf("2\n");
         extraRef = testRef;
         printf("3\n");
         refPtr = new Ref<TestClass>(extraRef);
         printf("4\n");

         Ref<TestClass> copy(*refPtr);
         printf("orig. copy=%p\n", copy());
         if (copy.EnsureRefIsPrivate() != B_NO_ERROR) printf("copy.EnsureRefIsPrivate() 1 failed!\n");
         printf("after EnsureRefIsPrivate(1), copy=%p\n", copy());
         if (copy.EnsureRefIsPrivate() != B_NO_ERROR) printf("copy.EnsureRefIsPrivate() 2 failed!\n");
         printf("after EnsureRefIsPrivate(2), copy=%p\n", copy());
      }
      printf("6\n");
      delete refPtr;
      //printf("7 [%s]\n", extraRef()->GetName());
   }

   {
      printf("Checking queue...\n");
      Queue<Ref<TestClass> > q;
      printf("Adding refs...\n");
      for (int i=0; i<10; i++) 
      {
         char temp[50]; sprintf(temp, "%i", i);
         q.AddTail(Ref<TestClass>(new TestClass(temp)));
      }
      printf("Removing refs...\n");
      while(q.GetNumItems() > 0) q.RemoveHead();
      printf("Done with queue test!\n");
   }

   {
      printf("Checking hashtable\n");
      Hashtable<String, Ref<TestClass> > ht;
      printf("Adding refs...\n");
      for (int i=0; i<10; i++) 
      {
         char temp[50]; sprintf(temp, "%i", i);
         ht.Put(String(temp), Ref<TestClass>(new TestClass(temp)));
      }
      printf("Removing refs...\n");
      for (int i=0; i<10; i++) 
      {
         char temp[50]; sprintf(temp, "%i", i);
         ht.Remove(String(temp));
      }
      printf("Done with hash table test!\n");
   }
    
   printf("Checking for memory leaks... watch free mem in about box, press CTRL-C when done\n");
   while(1)
   {
      Ref<TestClass> r2(pool ? pool->ObtainObject() : new TestClass("haha!"));
   }
   return 0;
}
