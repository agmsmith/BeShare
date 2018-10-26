/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */  

#include <stdio.h>

#include "message/Message.h"
#include "minimessage/MiniMessage.h"

using namespace muscle;

static MMessage * CreateTestMessage(uint32 recurseCount, Message & m)
{
   MMessage * msg = MMAllocMessage(0x1234);
   if (msg)
   {
      m.what = 0x1234;

      const int ITEM_COUNT = 10;
      int i;

      // Test every type....
      {
         MByteBuffer ** data = MMPutStringField(msg, false, "testStrings", ITEM_COUNT);
         if (data)
         {
            for (i=0; i<ITEM_COUNT; i++)
            {
               char buf[128+ITEM_COUNT];
               sprintf(buf, "This is test string #%i ", i);
               for (int j=0; j<i; j++) strcat(buf, "A");

               data[i] = MBStrdupByteBuffer(buf);
               m.AddString("testStrings", buf);
            }
         } 
         else printf("Error allocating string field!\n");
      }
      {
         MBool * data = MMPutBoolField(msg, false, "testBools", ITEM_COUNT);
         if (data)
         {
            for (i=0; i<ITEM_COUNT; i++) 
            {
               data[i] = (i%2) ? true : false;
               m.AddBool("testBools", data[i]);
            }
         } 
         else printf("Error allocating bool field!\n");
      }
      {
         int8 * data = MMPutInt8Field(msg, false, "testInt8s", ITEM_COUNT);
         if (data)
         {
            for (i=0; i<ITEM_COUNT; i++)
            {
               data[i] = i;
               m.AddInt8("testInt8s", data[i]);
            }
         } 
         else printf("Error allocating int8 field!\n");
      }
      {
         int16 * data = MMPutInt16Field(msg, false, "testInt16s", ITEM_COUNT);
         if (data)
         {
            for (i=0; i<ITEM_COUNT; i++) 
            {
               data[i] = i;
               m.AddInt16("testInt16s", data[i]);
            }
         } 
         else printf("Error allocating int16 field!\n");
      }
      {
         int32 * data = MMPutInt32Field(msg, false, "testInt32s", ITEM_COUNT);
         if (data)
         {
            for (i=0; i<ITEM_COUNT; i++) 
            {
               data[i] = i;
               m.AddInt32("testInt32s", data[i]);
            }
         } 
         else printf("Error allocating int32 field!\n");
      }
      {
         int64 * data = MMPutInt64Field(msg, false, "testInt64s", ITEM_COUNT);
         if (data)
         {
            for (i=0; i<ITEM_COUNT; i++) 
            {
               data[i] = i;
               m.AddInt64("testInt64s", data[i]);
            }
         } 
         else printf("Error allocating int64 field!\n");
      }
      {
         float * data = MMPutFloatField(msg, false, "testFloats", ITEM_COUNT);
         if (data)
         {
            for (i=0; i<ITEM_COUNT; i++) 
            {
               data[i] = i;
               m.AddFloat("testFloats", data[i]);
            }
         } 
         else printf("Error allocating float field!\n");
      }
      {
         double * data = MMPutDoubleField(msg, false, "testDoubles", ITEM_COUNT);
         if (data)
         {
            for (i=0; i<ITEM_COUNT; i++) 
            {
               data[i] = i;
               m.AddDouble("testDoubles", data[i]);
            }
         }
         else printf("Error allocating double field!\n");
      }
      {
         MMessage ** data = MMPutMessageField(msg, false, "testMessages", ITEM_COUNT);
         if (data)
         {
            for (i=0; i<ITEM_COUNT; i++) 
            {
               if (recurseCount > 0)
               {
                  Message subMsg;
                  data[i] = CreateTestMessage(recurseCount-1, subMsg);
                  m.AddMessage("testMessages", subMsg);
               }
               else 
               {
                  data[i] = MMAllocMessage(i);
                  m.AddMessage("testMessages", Message(i));
               }
            }
         }
         else printf("Error allocating message field!\n");
      }
      {
         void ** data = MMPutPointerField(msg, false, "testPointers", ITEM_COUNT);
         if (data)
         {
            for (i=0; i<ITEM_COUNT; i++) 
            {
               data[i] = (void *)i;
               m.AddPointer("testPointers", data[i]);
            }
         } 
         else printf("Error allocating pointer field!\n");
      }
      {
         MPoint * data = MMPutPointField(msg, false, "testX", ITEM_COUNT);
         if (data)
         {
            for (i=0; i<ITEM_COUNT; i++)
            {
               data[i].x = i;
               data[i].y = i+ITEM_COUNT;
               m.AddPoint("testPoints", Point(data[i].x, data[i].y));
            }
         } 
         else printf("Error allocating point field!\n");

         if (MMRenameField(msg, "testX", "testPoints") != B_NO_ERROR) printf("ERROR:  MMRenameField() failed!\n");
         if (MMRenameField(msg, "testX", "testPoints") == B_NO_ERROR) printf("ERROR:  Invalid MMRenameField() succeeded!\n");
      }
      {
         MRect * data = MMPutRectField(msg, false, "testRects", ITEM_COUNT);
         if (data)
         {
            for (i=0; i<ITEM_COUNT; i++)
            {
               data[i].left   = i;
               data[i].top    = i+ITEM_COUNT;
               data[i].right  = i+(ITEM_COUNT*2);
               data[i].bottom = i+(ITEM_COUNT*3);
               m.AddRect("testRects", Rect(data[i].left, data[i].top, data[i].right, data[i].bottom));
            }
         } 
         else printf("Error allocating rect field!\n");
      }
      {
         MByteBuffer ** data = MMPutDataField(msg, false, 0x666, "testDatas", ITEM_COUNT); 
         if (data)
         {
            for (i=0; i<ITEM_COUNT; i++)
            {
               char buf[128+ITEM_COUNT];
               sprintf(buf, "This is test data #%i ", i);
               for (int j=0; j<i; j++) strcat(buf, "B");

               data[i] = MBStrdupByteBuffer(buf);
               m.AddData("testDatas", 0x666, buf, strlen(buf)+1);
            }
         }
         else printf("Error allocating data field!\n");
      }
   }
   else printf("Error allocating Message!\n");

   return msg;
}

static void RecursiveRemoveFields(MMessage * msg, const char * fieldName)
{
   MMessageIterator iter = MMGetFieldNameIterator(msg, B_MESSAGE_TYPE);
   const char * fn;
   uint32 tc;
   while((fn = MMGetNextFieldName(&iter, &tc)) != NULL)
   {
      uint32 numItems;
      MMessage ** subMsgs = MMGetMessageField(msg, fn, &numItems);
      if (subMsgs)
      {
         int i;
         for (i=0; i<numItems; i++) RecursiveRemoveFields(subMsgs[i], fieldName);
      }
      else printf("RecursiveRemoveFields:  ERROR getting submessage field [%s]!\n", fn);
   }

   (void) MMRemoveField(msg, fieldName);
}

// This program compares flattened MiniMessages against flattened Messages, to
// make sure the created bytes are the same in both cases.
int main(int, char **)
{
   Message m;
   MMessage * mmsg = CreateTestMessage(1, m);
   MMessage * mmsg2 = NULL;
   if (mmsg)
   {
      MMessage * clone = MMCloneMessage(mmsg);
      if (clone)
      {
         if (MMAreMessagesEqual(mmsg, clone)) printf("Clone test passed.\n");
                                         else printf("ERROR, Cloned Message isn't equal?????\n");
         MMFreeMessage(clone);
      }
      else printf("ERROR cloning MMessage!\n");
   
      uint8 * buf = NULL, * mmbuf = NULL, * mmbuf2 = NULL;
      uint32 bufSize = 0, mmBufSize = 0, mmBuf2Size;

      printf("---------------------------------MMsg:\n");
      MMPrintToStream(mmsg);

      printf("---------------------------------Msg:\n");
      m.PrintToStream();

      printf("---------------------------------MMsg:\n");
      {
         mmBufSize = MMGetFlattenedSize(mmsg);
         mmbuf = (uint8 *)malloc(mmBufSize);
         if (mmbuf)
         {
            MMFlattenMessage(mmsg, mmbuf);
            for (int i=0; i<mmBufSize; i++) printf("%02x ", mmbuf[i]); printf("\n");

            mmsg2 = MMAllocMessage(0);
            if (mmsg2)
            {
               if (MMUnflattenMessage(mmsg2, mmbuf, mmBufSize) == B_NO_ERROR)
               {
                  mmBuf2Size = MMGetFlattenedSize(mmsg2);
                  if (mmBuf2Size == mmBufSize)
                  {
                     printf("Unflattened Message:\n");
                     MMPrintToStream(mmsg2);

                     {
                        MMessage * mmsg3 = MMCloneMessage(mmsg);
                        if (mmsg3)
                        {
                           RecursiveRemoveFields(mmsg3, "testPointers");  /* since this won't have been flattened */

                           if (MMAreMessagesEqual(mmsg3, mmsg2)) printf("MMUnflattenMessage()'d Message matches!\n");
                                                            else printf("ERROR:  MMUnflattenMessage()'d Message didn't match!\n");

                           MMFreeMessage(mmsg3);
                        }
                        else printf("ERROR:  Couldn't clone mmsg!\n");
                     }

                     mmbuf2 = (uint8 *)malloc(mmBuf2Size);
                     if (mmbuf2) MMFlattenMessage(mmsg2, mmbuf2);
                            else printf("ERROR:  Couldn't allocate %lu byte buffer for mmbuf2!\n", mmBuf2Size);
                  }
                  else printf("ERROR:  mmBuf2Size != mmBufSize!  (%lu vs %lu)\n", mmBuf2Size, mmBufSize);
               }
               else printf("ERROR: MMUnflattenMessage() returned an error!\n");
            }
            else printf("ERROR allocating mmsg2!\n");
         }
      }

      printf("---------------------------------Msg:\n");
      {
         bufSize = m.FlattenedSize();
         buf = (uint8 *)malloc(bufSize);
         if (buf)
         {
            m.Flatten(buf);
            for (int i=0; i<bufSize; i++) printf("%02x ", buf[i]); printf("\n");
         }
      }
 
      if ((buf)&&(mmbuf))
      {
         if ((bufSize == mmBufSize)&&(mmBuf2Size == bufSize))
         {
            bool sawMismatch = false;
            for (int i=0; i<bufSize; i++) 
            {
               if ((buf[i] != mmbuf[i])||(buf[i] != mmbuf2[i])) 
               {
                  printf("BYTE MISMATCH AT POSITION %lu:  %02x vs %02x or %02x\n", i, buf[i], mmbuf[i], mmbuf2[i]);
                  sawMismatch = true;
                  break;
               }
            }
            if (sawMismatch == false) printf("Buffers matched (%lu bytes).\n", bufSize);
         }
         else printf("ERROR, BUFFER LENGTHS DON'T MATCH! (bufSize=%lu mmBufSize=%lu mmBuf2Size=%lu)\n", bufSize, mmBufSize, mmBuf2Size);
      }
      else printf("ERROR, BUFFERS NOT ALLOCED?\n");

      if (buf)    free(buf);
      if (mmbuf)  free(mmbuf);
      if (mmbuf2) free(mmbuf2);

      MMFreeMessage(mmsg);
      MMFreeMessage(mmsg2);
   }
   return 0;
}
