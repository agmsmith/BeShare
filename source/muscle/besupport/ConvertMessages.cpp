/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */  

#include "besupport/ConvertMessages.h"

BEGIN_NAMESPACE(muscle);

status_t ConvertToBMessage(const Message & from, BMessage & to) 
{
   to.MakeEmpty();
   to.what = from.what;

   uint32 type;
   uint32 count;
   bool fixedSize;

   MessageFieldNameIterator it = from.GetFieldNameIterator(B_ANY_TYPE, HTIT_FLAG_NOREGISTER);
   const char * n;
   while((n = it.GetNextFieldName()) != NULL)
   {
      if (from.GetInfo(n, &type, &count, &fixedSize) != B_NO_ERROR) return B_ERROR;
      for (uint32 j=0; j<count; j++)
      {
         const void * nextItem;
         uint32 itemSize;
         if (from.FindData(n, type, j, &nextItem, &itemSize) != B_NO_ERROR) return B_ERROR;

         // do any necessary translation from the Muscle data types to Be data types
         switch(type)
         {
            case B_POINT_TYPE:
            {
               const Point * p = (const Point *)nextItem;
               BPoint bpoint(p->x(), p->y());
               if (to.AddPoint(n, bpoint) != B_NO_ERROR) return B_ERROR;
            }
            break;

            case B_RECT_TYPE:
            {
               const Rect * r = (const Rect *)nextItem;
               BRect brect(r->left(), r->top(), r->right(), r->bottom());
               if (to.AddRect(n, brect) != B_NO_ERROR) return B_ERROR;
            }
            break;
 
            case B_MESSAGE_TYPE:
            {
               MessageRef * msgRef = (MessageRef *) nextItem;
               BMessage bmsg;
               if (msgRef->GetItemPointer() == NULL) return B_ERROR;
               if (ConvertToBMessage(*msgRef->GetItemPointer(), bmsg) != B_NO_ERROR) return B_ERROR;
               if (to.AddMessage(n, &bmsg) != B_NO_ERROR) return B_ERROR;
            }
            break;

            default:
               if (to.AddData(n, type, nextItem, itemSize, fixedSize, count) != B_NO_ERROR) return B_ERROR;
            break;
         }
      }
   }
   return B_NO_ERROR;
}

status_t ConvertFromBMessage(const BMessage & from, Message & to) 
{
   to.Clear();
   to.what = from.what;

#if B_BEOS_VERSION_DANO
   const char * name;
#else
   char * name;
#endif

   type_code type;
   int32 count;

   for (int32 i=0; (from.GetInfo(B_ANY_TYPE, i, &name, &type, &count) == B_NO_ERROR); i++)
   {
      for (int32 j=0; j<count; j++)
      {
         const void * nextItem;
         ssize_t itemSize;
         if (from.FindData(name, type, j, &nextItem, &itemSize) != B_NO_ERROR) return B_ERROR;

         // do any necessary translation from the Be data types to Muscle data types
         switch(type)
         {
            case B_POINT_TYPE:
            {
               const BPoint * p = (const BPoint *)nextItem;
               Point pPoint(p->x, p->y);
               if (to.AddPoint(name, pPoint) != B_NO_ERROR) return B_ERROR;
            }
            break;

            case B_RECT_TYPE:
            {
               const BRect * r = (const BRect *)nextItem;
               Rect pRect(r->left, r->top, r->right, r->bottom);
               if (to.AddRect(name, pRect) != B_NO_ERROR) return B_ERROR;
            }
            break;

            case B_MESSAGE_TYPE:
            {
               BMessage bmsg;
               if (bmsg.Unflatten((const char *)nextItem) != B_NO_ERROR) return B_ERROR;
               Message * newMsg = newnothrow Message;
               if (newMsg)
               {
                  MessageRef msgRef(newMsg);
                  if (ConvertFromBMessage(bmsg, *newMsg) != B_NO_ERROR) return B_ERROR;
                  if (to.AddMessage(name, msgRef) != B_NO_ERROR) return B_ERROR;
               }
               else {WARN_OUT_OF_MEMORY; return B_ERROR;}
            }
            break;

            default:
               if (to.AddData(name, type, nextItem, itemSize) != B_NO_ERROR) return B_ERROR;
            break;
         }

      }
   }
   return B_NO_ERROR;
}

END_NAMESPACE(muscle);
