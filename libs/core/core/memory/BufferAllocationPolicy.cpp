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

#include "core/memory/BufferAllocationPolicy.hpp"

#include "core/memory/ByteSize.hpp"
#include "core/memory/exception/Memory.hpp"

namespace sight::core::memory
{

//------------------------------------------------------------------------------

void BufferMallocPolicy::allocate(BufferType& buffer,
                                  BufferAllocationPolicy::SizeType size)
{
    if (size > 0)
    {
        try
        {
            buffer = malloc( size );
        }
        catch(...)
        {
            buffer = nullptr;
        }

        if (buffer == nullptr)
        {
            SIGHT_THROW_EXCEPTION_MSG( core::memory::exception::Memory,
                                       "Cannot allocate memory ("
                                       << core::memory::ByteSize(core::memory::ByteSize::SizeType(size)) <<").");
        }
    }
}

//------------------------------------------------------------------------------

void BufferMallocPolicy::reallocate(BufferType& buffer,
                                    BufferAllocationPolicy::SizeType size)
{
    BufferType newBuffer;
    if (size > 0)
    {
        newBuffer = realloc( buffer, size );
    }
    else
    {
        free( buffer );
        newBuffer = NULL;
    }
    if (newBuffer == NULL && size > 0)
    {
        SIGHT_THROW_EXCEPTION_MSG( core::memory::exception::Memory,
                                   "Cannot allocate memory ("
                                   << core::memory::ByteSize(core::memory::ByteSize::SizeType(size)) <<").");
    }
    buffer = newBuffer;
}

//------------------------------------------------------------------------------

void BufferMallocPolicy::destroy(BufferType& buffer)
{
    free( buffer );
    buffer = 0;
}

//------------------------------------------------------------------------------

BufferAllocationPolicy::sptr BufferMallocPolicy::New()
{
    return BufferAllocationPolicy::sptr(new BufferMallocPolicy);
}

//------------------------------------------------------------------------------

void BufferNewPolicy::allocate(BufferType& buffer,
                               BufferAllocationPolicy::SizeType size)
{
    try
    {
        if (size > 0)
        {
            buffer = new char[size];
        }
    }
    catch (std::bad_alloc& ba)
    {
        SIGHT_THROW_EXCEPTION_MSG( core::memory::exception::Memory,
                                   "bad_alloc caught: " << ba.what());
    }
}

//------------------------------------------------------------------------------

void BufferNewPolicy::reallocate(BufferType& buffer,
                                 BufferAllocationPolicy::SizeType size)
{
    SIGHT_NOT_USED(buffer);
    SIGHT_NOT_USED(size);
    SIGHT_THROW_EXCEPTION_MSG( core::memory::exception::Memory,
                               "Reallocation not managed for buffer allocated with 'new' operator.");
}

//------------------------------------------------------------------------------

void BufferNewPolicy::destroy(BufferType& buffer)
{
    delete[] static_cast<char*>(buffer);
    buffer = 0;
}

//------------------------------------------------------------------------------

BufferAllocationPolicy::sptr BufferNewPolicy::New()
{
    return BufferAllocationPolicy::sptr(new BufferNewPolicy);
}

//------------------------------------------------------------------------------

void BufferNoAllocPolicy::allocate(BufferType& buffer,
                                   BufferAllocationPolicy::SizeType size)
{
    SIGHT_NOT_USED(buffer);
    SIGHT_NOT_USED(size);
    SIGHT_THROW_EXCEPTION_MSG( core::memory::exception::Memory,
                               "No Allocation Policy should not be called.");
}

//------------------------------------------------------------------------------

void BufferNoAllocPolicy::reallocate(BufferType& buffer,
                                     BufferAllocationPolicy::SizeType size)
{
    SIGHT_NOT_USED(buffer);
    SIGHT_NOT_USED(size);
    SIGHT_THROW_EXCEPTION_MSG( core::memory::exception::Memory,
                               "No Allocation Policy should not be called.");
}

//------------------------------------------------------------------------------

void BufferNoAllocPolicy::destroy(BufferType& buffer)
{
    SIGHT_NOT_USED(buffer);
    SIGHT_ASSERT("No Alloc Policy should not be called", 0);
}

//------------------------------------------------------------------------------

BufferAllocationPolicy::sptr BufferNoAllocPolicy::New()
{
    return BufferAllocationPolicy::sptr(new BufferNoAllocPolicy);
}

//------------------------------------------------------------------------------

} //namespace sight::core::memory
