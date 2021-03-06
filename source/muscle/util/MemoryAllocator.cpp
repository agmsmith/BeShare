/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */ 

#include "util/MemoryAllocator.h"

BEGIN_NAMESPACE(muscle);

status_t ProxyMemoryAllocator :: AboutToAllocate(size_t currentlyAllocatedBytes, size_t allocRequestBytes)
{
   return _slaveRef() ? _slaveRef()->AboutToAllocate(currentlyAllocatedBytes, allocRequestBytes) : B_NO_ERROR;
}

void ProxyMemoryAllocator :: AboutToFree(size_t currentlyAllocatedBytes, size_t allocRequestBytes)
{
   if (_slaveRef()) _slaveRef()->AboutToFree(currentlyAllocatedBytes, allocRequestBytes);
}

void ProxyMemoryAllocator :: AllocationFailed(size_t currentlyAllocatedBytes, size_t allocRequestBytes)
{
   if (_slaveRef()) 
   {
      _slaveRef()->SetAllocationHasFailed(true);
      _slaveRef()->AllocationFailed(currentlyAllocatedBytes, allocRequestBytes);
   }
}

void ProxyMemoryAllocator :: SetAllocationHasFailed(bool hasFailed)
{
   MemoryAllocator::SetAllocationHasFailed(hasFailed);
   if (_slaveRef()) _slaveRef()->SetAllocationHasFailed(hasFailed);
}

size_t ProxyMemoryAllocator :: GetMaxNumBytes() const
{
   return (_slaveRef()) ? _slaveRef()->GetMaxNumBytes() : MUSCLE_NO_LIMIT;
}

size_t ProxyMemoryAllocator :: GetNumAvailableBytes(size_t currentlyAllocated) const
{
   return (_slaveRef()) ? _slaveRef()->GetNumAvailableBytes(currentlyAllocated) : MUSCLE_NO_LIMIT;
}


UsageLimitProxyMemoryAllocator :: UsageLimitProxyMemoryAllocator(const MemoryAllocatorRef & slaveRef, size_t maxBytes) : ProxyMemoryAllocator(slaveRef), _maxBytes(maxBytes)
{
   // empty
}
 
UsageLimitProxyMemoryAllocator :: ~UsageLimitProxyMemoryAllocator()
{
   // empty
}
 
status_t UsageLimitProxyMemoryAllocator :: AboutToAllocate(size_t currentlyAllocatedBytes, size_t allocRequestBytes)
{
   return ((allocRequestBytes < _maxBytes)&&(currentlyAllocatedBytes + allocRequestBytes <= _maxBytes)) ? ProxyMemoryAllocator::AboutToAllocate(currentlyAllocatedBytes, allocRequestBytes) : B_ERROR;
}

void AutoCleanupProxyMemoryAllocator :: AllocationFailed(size_t, size_t)
{
   uint32 nc = _callbacks.GetNumItems();
   for (uint32 i=0; i<nc; i++) if (_callbacks[i]()) (_callbacks[i]())->OutOfMemory();
}

END_NAMESPACE(muscle);
