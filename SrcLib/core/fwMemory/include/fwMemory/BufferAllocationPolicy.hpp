/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWMEMORY_BUFFERALLOCATIONPOLICY_HPP_
#define _FWMEMORY_BUFFERALLOCATIONPOLICY_HPP_

#include <fwCore/base.hpp>

#include "fwMemory/exception/Memory.hpp"
#include "fwMemory/config.hpp"

namespace fwMemory
{

class FWMEMORY_CLASS_API BufferAllocationPolicy
{
public:
    typedef SPTR(BufferAllocationPolicy) sptr;
    typedef size_t SizeType;
    typedef void* BufferType;

    FWMEMORY_API virtual void allocate(BufferType &buffer, SizeType size) throw( ::fwMemory::exception::Memory ) = 0;
    FWMEMORY_API virtual void reallocate(BufferType &buffer, SizeType size) throw( ::fwMemory::exception::Memory ) = 0;
    FWMEMORY_API virtual void destroy(BufferType &buffer) = 0;

    FWMEMORY_API virtual ~BufferAllocationPolicy(){};
};


class FWMEMORY_CLASS_API BufferMallocPolicy : public BufferAllocationPolicy
{
public:
    FWMEMORY_API void allocate(BufferType &buffer, BufferAllocationPolicy::SizeType size) throw( ::fwMemory::exception::Memory );
    FWMEMORY_API void reallocate(BufferType &buffer, BufferAllocationPolicy::SizeType size) throw( ::fwMemory::exception::Memory );
    FWMEMORY_API void destroy(BufferType &buffer);

    FWMEMORY_API static BufferAllocationPolicy::sptr New();
};


class FWMEMORY_CLASS_API BufferNewPolicy : public BufferAllocationPolicy
{
public:
    FWMEMORY_API void allocate(BufferType &buffer, BufferAllocationPolicy::SizeType size) throw( ::fwMemory::exception::Memory );
    FWMEMORY_API void reallocate(BufferType &buffer, BufferAllocationPolicy::SizeType size) throw( ::fwMemory::exception::Memory );
    FWMEMORY_API void destroy(BufferType &buffer);

    FWMEMORY_API static BufferAllocationPolicy::sptr New();
};

class FWMEMORY_CLASS_API BufferNoAllocPolicy : public BufferAllocationPolicy
{
public:
    FWMEMORY_API void allocate(BufferType &buffer, BufferAllocationPolicy::SizeType size) throw( ::fwMemory::exception::Memory );
    FWMEMORY_API void reallocate(BufferType &buffer, BufferAllocationPolicy::SizeType size) throw( ::fwMemory::exception::Memory );
    FWMEMORY_API void destroy(BufferType &buffer);

    FWMEMORY_API static BufferAllocationPolicy::sptr New();
};


}

#endif /* _FWMEMORY_BUFFERALLOCATIONPOLICY_HPP_ */
