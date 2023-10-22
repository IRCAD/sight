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

#pragma once

#include "core/config.hpp"
#include "core/memory/exception/memory.hpp"

#include <core/base.hpp>

namespace sight::core::memory
{

class CORE_CLASS_API buffer_allocation_policy
{
public:

    typedef SPTR(buffer_allocation_policy) sptr;
    typedef std::size_t size_type;
    typedef void* buffer_t;

    CORE_API virtual void allocate(buffer_t& _buffer, size_type _size)   = 0;
    CORE_API virtual void reallocate(buffer_t& _buffer, size_type _size) = 0;
    CORE_API virtual void destroy(buffer_t& _buffer)                     = 0;

    CORE_API virtual ~buffer_allocation_policy()
    = default;
};

class CORE_CLASS_API buffer_malloc_policy : public buffer_allocation_policy
{
public:

    CORE_API void allocate(
        buffer_t& _buffer,
        buffer_allocation_policy::size_type _size
    ) override;
    CORE_API void reallocate(
        buffer_t& _buffer,
        buffer_allocation_policy::size_type _size
    ) override;
    CORE_API void destroy(buffer_t& _buffer) override;
};

class CORE_CLASS_API buffer_new_policy : public buffer_allocation_policy
{
public:

    CORE_API void allocate(
        buffer_t& _buffer,
        buffer_allocation_policy::size_type _size
    ) override;
    CORE_API void reallocate(
        buffer_t& _buffer,
        buffer_allocation_policy::size_type _size
    ) override;
    CORE_API void destroy(buffer_t& _buffer) override;
};

class CORE_CLASS_API buffer_no_alloc_policy : public buffer_allocation_policy
{
public:

    CORE_API void allocate(
        buffer_t& _buffer,
        buffer_allocation_policy::size_type _size
    ) override;
    CORE_API void reallocate(
        buffer_t& _buffer,
        buffer_allocation_policy::size_type _size
    ) override;
    CORE_API void destroy(buffer_t& _buffer) override;
};

} // namespace sight::core::memory
