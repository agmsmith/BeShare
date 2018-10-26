/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */

#include "sdlsupport/SDLMessageTransceiverThread.h"

BEGIN_NAMESPACE(muscle);

SDLMessageTransceiverThread :: SDLMessageTransceiverThread() : MessageTransceiverThread()
{
   // empty
}

void SDLMessageTransceiverThread :: SignalOwner()
{
   SDL_Event event;
   event.type       = SDL_MTT_EVENT;
   event.user.code  = SDL_MTT_EVENT;
   event.user.data1 = NULL;
   event.user.data2 = NULL;
   SDL_PeepEvents(&event, 1, SDL_ADDEVENT, 0);
}

END_NAMESPACE(muscle);
