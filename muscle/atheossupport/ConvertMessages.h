/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */

#ifndef MuscleConvertMessages_h
#define MuscleConvertMessages_h

#include <util/message.h>
#include "message/Message.h"

BEGIN_NAMESPACE(muscle);
   
/*
 *   Functions to convert AtheOS Messages to MUSCLE Messages and back.
 *   This code will only compile under AtheOS!
 */

/** Converts a MUSCLE Message into an AtheOS Message.  Only compiles under AtheOS! 
 *  @param from the Message you wish to convert
 *  @param to the Message to write the result into
 *  @return B_NO_ERROR if the conversion succeeded, or B_ERROR if it failed.
 */
status_t ConvertToAMessage(const muscle::Message & from, os::Message & to);

/** Converts an AtheOS Message into a MUSCLE Message.  Only compiles under AtheOS! 
 *  @param from the Message you wish to convert
 *  @param to the Message to write the result into
 *  @return B_NO_ERROR if the conversion succeeded, or B_ERROR if it failed.
 */
status_t ConvertFromAMessage(const os::Message & from, muscle::Message & to);

END_NAMESPACE(muscle);

#endif
