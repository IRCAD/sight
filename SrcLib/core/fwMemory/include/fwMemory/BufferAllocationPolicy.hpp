/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWMEMORY_BUFFERALLOCATIONPOLICY_HPP__
#define __FWMEMORY_BUFFERALLOCATIONPOLICY_HPP__

#include <fwCore/base.hpp>

#include "fwMemory/exception/Memory.hpp"
#include "fwMemory/config.hpp"

namespace fwMemory
{

class FWMEMORY_CLASS_API BufferAllocationPolicy
{
public:
    typedef SPTR (BufferAllocationPolicy) sptr;
    typedef size_t SizeType;
    typedef void* BufferType;

    FWMEMORY_API virtual void allocate(BufferType &buffer, SizeType size)   = 0;
    FWMEMORY_API virtual void reallocate(BufferType &buffer, SizeType size) = 0;
    FWMEMORY_API virtual void destroy(BufferType &buffer)                                                          = 0;

    FWMEMORY_API virtual ~BufferAllocationPolicy()
    {
    }
};


class FWMEMORY_CLASS_API BufferMallocPolicy : public BufferAllocationPolicy
{
public:
    FWMEMORY_API void allocate(BufferType &buffer,
                               BufferAllocationPolicy::SizeType size);
    FWMEMORY_API void reallocate(BufferType &buffer,
                                 BufferAllocationPolicy::SizeType size);
    FWMEMORY_API void destroy(BufferType &buffer);

    FWMEMORY_API static BufferAllocationPolicy::sptr New();
};


class FWMEMORY_CLASS_API BufferNewPolicy : public BufferAllocationPolicy
{
public:
    FWMEMORY_API void allocate(BufferType &buffer,
                               BufferAllocationPolicy::SizeType size);
    FWMEMORY_API void reallocate(BufferType &buffer,
                                 BufferAllocationPolicy::SizeType size);
    FWMEMORY_API void destroy(BufferType &buffer);

    FWMEMORY_API static BufferAllocationPolicy::sptr New();
};

class FWMEMORY_CLASS_API BufferNoAllocPolicy : public BufferAllocationPolicy
{
public:
    FWMEMORY_API void allocate(BufferType &buffer,
                               BufferAllocationPolicy::SizeType size);
    FWMEMORY_API void reallocate(BufferType &buffer,
                                 BufferAllocationPolicy::SizeType size);
    FWMEMORY_API void destroy(BufferType &buffer);

    FWMEMORY_API static BufferAllocationPolicy::sptr New();
};


}

#endif /* __FWMEMORY_BUFFERALLOCATIONPOLICY_HPP__ */
