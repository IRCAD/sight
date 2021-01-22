/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#pragma once

#include "core/config.hpp"
#include "core/memory/exception/Memory.hpp"
#include <core/base.hpp>

namespace sight::core::memory
{

class CORE_CLASS_API BufferAllocationPolicy
{
public:
    typedef SPTR (BufferAllocationPolicy) sptr;
    typedef size_t SizeType;
    typedef void* BufferType;

    CORE_API virtual void allocate(BufferType& buffer, SizeType size) = 0;
    CORE_API virtual void reallocate(BufferType& buffer, SizeType size) = 0;
    CORE_API virtual void destroy(BufferType& buffer) = 0;

    CORE_API virtual ~BufferAllocationPolicy()
    {
    }
};

class CORE_CLASS_API BufferMallocPolicy : public BufferAllocationPolicy
{
public:
    CORE_API void allocate(BufferType& buffer,
                           BufferAllocationPolicy::SizeType size);
    CORE_API void reallocate(BufferType& buffer,
                             BufferAllocationPolicy::SizeType size);
    CORE_API void destroy(BufferType& buffer);

    CORE_API static BufferAllocationPolicy::sptr New();
};

class CORE_CLASS_API BufferNewPolicy : public BufferAllocationPolicy
{
public:
    CORE_API void allocate(BufferType& buffer,
                           BufferAllocationPolicy::SizeType size);
    CORE_API void reallocate(BufferType& buffer,
                             BufferAllocationPolicy::SizeType size);
    CORE_API void destroy(BufferType& buffer);

    CORE_API static BufferAllocationPolicy::sptr New();
};

class CORE_CLASS_API BufferNoAllocPolicy : public BufferAllocationPolicy
{
public:
    CORE_API void allocate(BufferType& buffer,
                           BufferAllocationPolicy::SizeType size);
    CORE_API void reallocate(BufferType& buffer,
                             BufferAllocationPolicy::SizeType size);
    CORE_API void destroy(BufferType& buffer);

    CORE_API static BufferAllocationPolicy::sptr New();
};

}
