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
    buffer_t& buffer,
    buffer_allocation_policy::size_type size
)
{
    if(size > 0)
    {
        try
        {
            buffer = malloc(size); // NOLINT(cppcoreguidelines-no-malloc,hicpp-no-malloc)
        }
        catch(...)
        {
            buffer = nullptr;
        }

        if(buffer == nullptr)
        {
            SIGHT_THROW_EXCEPTION_MSG(
                core::memory::exception::memory,
                "Cannot allocate memory ("
                << core::memory::byte_size(core::memory::byte_size::size_t(size)) << ")."
            );
        }
    }
}

//------------------------------------------------------------------------------

void buffer_malloc_policy::reallocate(
    buffer_t& buffer,
    buffer_allocation_policy::size_type size
)
{
    buffer_t new_buffer = nullptr;
    if(size > 0)
    {
        new_buffer = realloc(buffer, size); // NOLINT(cppcoreguidelines-no-malloc,hicpp-no-malloc)
    }
    else
    {
        free(buffer); // NOLINT(cppcoreguidelines-no-malloc,hicpp-no-malloc)
        new_buffer = nullptr;
    }

    if(new_buffer == nullptr && size > 0)
    {
        SIGHT_THROW_EXCEPTION_MSG(
            core::memory::exception::memory,
            "Cannot allocate memory ("
            << core::memory::byte_size(core::memory::byte_size::size_t(size)) << ")."
        );
    }

    buffer = new_buffer;
}

//------------------------------------------------------------------------------

void buffer_malloc_policy::destroy(buffer_t& buffer)
{
    free(buffer); // NOLINT(cppcoreguidelines-no-malloc,hicpp-no-malloc)
    buffer = nullptr;
}

//------------------------------------------------------------------------------

void buffer_new_policy::allocate(
    buffer_t& buffer,
    buffer_allocation_policy::size_type size
)
{
    try
    {
        if(size > 0)
        {
            buffer = new char [size];
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
    buffer_t& buffer,
    buffer_allocation_policy::size_type size
)
{
    SIGHT_NOT_USED(buffer);
    SIGHT_NOT_USED(size);
    SIGHT_THROW_EXCEPTION_MSG(
        core::memory::exception::memory,
        "Reallocation not managed for buffer allocated with 'new' operator."
    );
}

//------------------------------------------------------------------------------

void buffer_new_policy::destroy(buffer_t& buffer)
{
    delete[] static_cast<char*>(buffer);
    buffer = nullptr;
}

//------------------------------------------------------------------------------

void buffer_no_alloc_policy::allocate(
    buffer_t& buffer,
    buffer_allocation_policy::size_type size
)
{
    SIGHT_NOT_USED(buffer);
    SIGHT_NOT_USED(size);
    SIGHT_THROW_EXCEPTION_MSG(
        core::memory::exception::memory,
        "No Allocation Policy should not be called."
    );
}

//------------------------------------------------------------------------------

void buffer_no_alloc_policy::reallocate(
    buffer_t& buffer,
    buffer_allocation_policy::size_type size
)
{
    SIGHT_NOT_USED(buffer);
    SIGHT_NOT_USED(size);
    SIGHT_THROW_EXCEPTION_MSG(
        core::memory::exception::memory,
        "No Allocation Policy should not be called."
    );
}

//------------------------------------------------------------------------------

void buffer_no_alloc_policy::destroy(buffer_t& buffer)
{
    SIGHT_NOT_USED(buffer);
    SIGHT_ASSERT("No Alloc Policy should not be called", 0);
}

//------------------------------------------------------------------------------

} //namespace sight::core::memory
