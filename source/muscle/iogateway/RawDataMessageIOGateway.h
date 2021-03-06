/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */

#ifndef MuscleRawDataMessageIOGateway_h
#define MuscleRawDataMessageIOGateway_h

#include "iogateway/AbstractMessageIOGateway.h"

BEGIN_NAMESPACE(muscle);

/** This is the name of the field used to hold data chunks */
#define PR_NAME_DATA_CHUNKS "rd"

/** The 'what' code that will be found in incoming Messages. */
#define PR_COMMAND_RAW_DATA 1919181923 // 'rddc'

/** 
 * This gateway is very crude; it can be used to write raw data to a TCP socket, and
 * to retrieve data from the socket in chunks of a specified size range.
 */
class RawDataMessageIOGateway : public AbstractMessageIOGateway
{
public:
   /** Constructor.
     * @param minChunkSize Don't return any data in chunks smaller than this.  Defaults to zero.
     * @param maxChunkSize Don't return any data in chunks larger than this.  Defaults to the largest possible uint32 value.
     */
   RawDataMessageIOGateway(uint32 minChunkSize=0, uint32 maxChunkSize=((uint32)-1));

   /** Destructor */
   virtual ~RawDataMessageIOGateway();

   virtual bool HasBytesToOutput() const;
   virtual void Reset();

protected:
   virtual int32 DoOutputImplementation(uint32 maxBytes = MUSCLE_NO_LIMIT);
   virtual int32 DoInputImplementation(AbstractGatewayMessageReceiver & receiver, uint32 maxBytes = MUSCLE_NO_LIMIT);

private:
   void FlushPendingInput();

   MessageRef _sendMsgRef;
   const void * _sendBuf;
   int32 _sendBufLength;      // # of bytes in current buffer
   int32 _sendBufIndex;       // index of the buffer currently being sent
   int32 _sendBufByteOffset;  // Index of Next byte to send in the current buffer

   MessageRef _recvMsgRef;
   void * _recvBuf;
   int32 _recvBufLength;
   int32 _recvBufByteOffset;

   uint8 * _recvScratchSpace;        // demand-allocated
   uint32 _recvScratchSpaceSize;

   uint32 _minChunkSize;
   uint32 _maxChunkSize;
};

END_NAMESPACE(muscle);

#endif
