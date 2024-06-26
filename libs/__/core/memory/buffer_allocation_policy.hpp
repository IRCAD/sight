/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/core/config.hpp>

#include "core/memory/exception/memory.hpp"

#include <core/base.hpp>

namespace sight::core::memory
{

class SIGHT_CORE_CLASS_API buffer_allocation_policy
{
public:

    using sptr      = std::shared_ptr<buffer_allocation_policy>;
    using size_type = std::size_t;
    using buffer_t  = void*;

    SIGHT_CORE_API virtual void allocate(buffer_t& _buffer, size_type _size)   = 0;
    SIGHT_CORE_API virtual void reallocate(buffer_t& _buffer, size_type _size) = 0;
    SIGHT_CORE_API virtual void destroy(buffer_t& _buffer)                     = 0;

    SIGHT_CORE_API virtual ~buffer_allocation_policy()
    = default;
};

class SIGHT_CORE_CLASS_API buffer_malloc_policy : public buffer_allocation_policy
{
public:

    SIGHT_CORE_API void allocate(
        buffer_t& _buffer,
        buffer_allocation_policy::size_type _size
    ) override;
    SIGHT_CORE_API void reallocate(
        buffer_t& _buffer,
        buffer_allocation_policy::size_type _size
    ) override;
    SIGHT_CORE_API void destroy(buffer_t& _buffer) override;
};

class SIGHT_CORE_CLASS_API buffer_new_policy : public buffer_allocation_policy
{
public:

    SIGHT_CORE_API void allocate(
        buffer_t& _buffer,
        buffer_allocation_policy::size_type _size
    ) override;
    SIGHT_CORE_API void reallocate(
        buffer_t& _buffer,
        buffer_allocation_policy::size_type _size
    ) override;
    SIGHT_CORE_API void destroy(buffer_t& _buffer) override;
};

class SIGHT_CORE_CLASS_API buffer_no_alloc_policy : public buffer_allocation_policy
{
public:

    SIGHT_CORE_API void allocate(
        buffer_t& _buffer,
        buffer_allocation_policy::size_type _size
    ) override;
    SIGHT_CORE_API void reallocate(
        buffer_t& _buffer,
        buffer_allocation_policy::size_type _size
    ) override;
    SIGHT_CORE_API void destroy(buffer_t& _buffer) override;
};

} // namespace sight::core::memory
