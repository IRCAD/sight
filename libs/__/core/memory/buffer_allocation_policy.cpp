/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

// cspell:ignore NOLINT hicpp

#include "core/memory/buffer_allocation_policy.hpp"

#include "core/memory/byte_size.hpp"
#include "core/memory/exception/memory.hpp"

namespace sight::core::memory
{

//------------------------------------------------------------------------------

void buffer_malloc_policy::allocate(
    buffer_t& _buffer,
    buffer_allocation_policy::size_type _size
)
{
    if(_size > 0)
    {
        try
        {
            _buffer = malloc(_size); // NOLINT(cppcoreguidelines-no-malloc,hicpp-no-malloc)
        }
        catch(...)
        {
            _buffer = nullptr;
        }

        if(_buffer == nullptr)
        {
            SIGHT_THROW_EXCEPTION_MSG(
                core::memory::exception::memory,
                "Cannot allocate memory ("
                << core::memory::byte_size(core::memory::byte_size::size_t(_size)) << ")."
            );
        }
    }
}

//------------------------------------------------------------------------------

void buffer_malloc_policy::reallocate(
    buffer_t& _buffer,
    buffer_allocation_policy::size_type _size
)
{
    buffer_t new_buffer = nullptr;
    if(_size > 0)
    {
        new_buffer = realloc(_buffer, _size); // NOLINT(cppcoreguidelines-no-malloc,hicpp-no-malloc)
    }
    else
    {
        free(_buffer); // NOLINT(cppcoreguidelines-no-malloc,hicpp-no-malloc)
        new_buffer = nullptr;
    }

    if(new_buffer == nullptr && _size > 0)
    {
        SIGHT_THROW_EXCEPTION_MSG(
            core::memory::exception::memory,
            "Cannot allocate memory ("
            << core::memory::byte_size(core::memory::byte_size::size_t(_size)) << ")."
        );
    }

    _buffer = new_buffer;
}

//------------------------------------------------------------------------------

void buffer_malloc_policy::destroy(buffer_t& _buffer)
{
    free(_buffer); // NOLINT(cppcoreguidelines-no-malloc,hicpp-no-malloc)
    _buffer = nullptr;
}

//------------------------------------------------------------------------------

void buffer_new_policy::allocate(
    buffer_t& _buffer,
    buffer_allocation_policy::size_type _size
)
{
    try
    {
        if(_size > 0)
        {
            _buffer = new char [_size];
        }
    }
    catch(std::bad_alloc& ba)
    {
        SIGHT_THROW_EXCEPTION_MSG(
            core::memory::exception::memory,
            "bad_alloc caught: " << ba.what()
        );
    }
}

//------------------------------------------------------------------------------

void buffer_new_policy::reallocate(
    buffer_t& _buffer,
    buffer_allocation_policy::size_type _size
)
{
    SIGHT_NOT_USED(_buffer);
    SIGHT_NOT_USED(_size);
    SIGHT_THROW_EXCEPTION_MSG(
        core::memory::exception::memory,
        "Reallocation not managed for buffer allocated with 'new' operator."
    );
}

//------------------------------------------------------------------------------

void buffer_new_policy::destroy(buffer_t& _buffer)
{
    delete[] static_cast<char*>(_buffer);
    _buffer = nullptr;
}

//------------------------------------------------------------------------------

void buffer_no_alloc_policy::allocate(
    buffer_t& _buffer,
    buffer_allocation_policy::size_type _size
)
{
    SIGHT_NOT_USED(_buffer);
    SIGHT_NOT_USED(_size);
    SIGHT_THROW_EXCEPTION_MSG(
        core::memory::exception::memory,
        "No Allocation Policy should not be called."
    );
}

//------------------------------------------------------------------------------

void buffer_no_alloc_policy::reallocate(
    buffer_t& _buffer,
    buffer_allocation_policy::size_type _size
)
{
    SIGHT_NOT_USED(_buffer);
    SIGHT_NOT_USED(_size);
    SIGHT_THROW_EXCEPTION_MSG(
        core::memory::exception::memory,
        "No Allocation Policy should not be called."
    );
}

//------------------------------------------------------------------------------

void buffer_no_alloc_policy::destroy(buffer_t& _buffer)
{
    SIGHT_NOT_USED(_buffer);
    SIGHT_ASSERT("No Alloc Policy should not be called", 0);
}

//------------------------------------------------------------------------------

} //namespace sight::core::memory
