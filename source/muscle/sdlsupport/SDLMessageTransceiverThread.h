/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */

#ifndef MuscleSDLMessageTransceiverThread_h
#define MuscleSDLMessageTransceiverThread_h

#include <SDL/SDL_events.h>
#include "system/MessageTransceiverThread.h"

BEGIN_NAMESPACE(muscle);

static const uint32 SDL_MTT_EVENT = SDL_NUMEVENTS-1;

/** This class is useful for interfacing a MessageTransceiverThread to the SDL event loop.
 *  @author Shard
 */
class SDLMessageTransceiverThread : public MessageTransceiverThread
{
public:
   /** Constructor. */
   SDLMessageTransceiverThread();

protected:
   /** Overridden to send a SDLEvent */
   virtual void SignalOwner();
};

END_NAMESPACE(muscle);

#endif
