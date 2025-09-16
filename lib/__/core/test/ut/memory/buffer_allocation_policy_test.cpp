/************************************************************************
 *
 * Copyright (C) 2022-2025 IRCAD France
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

#include <core/memory/buffer_allocation_policy.hpp>
#include <core/memory/exception/memory.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::core::memory::buffer_malloc_policy")
{
//------------------------------------------------------------------------------

    TEST_CASE("exception")
    {
        const size_t impossible_buffer_size                            = std::numeric_limits<size_t>::max();
        sight::core::memory::buffer_allocation_policy::buffer_t buffer = nullptr;

        // Buffer Malloc Policy
        auto malloc_p = std::make_shared<sight::core::memory::buffer_malloc_policy>();
        CHECK_THROWS_AS(malloc_p->allocate(buffer, impossible_buffer_size), sight::core::memory::exception::memory);
        CHECK_THROWS_AS(malloc_p->reallocate(buffer, impossible_buffer_size), sight::core::memory::exception::memory);

        // Buffer New Policy
        auto new_p = std::make_shared<sight::core::memory::buffer_new_policy>();
        CHECK_THROWS_AS(new_p->allocate(buffer, impossible_buffer_size), sight::core::memory::exception::memory);
        CHECK_THROWS_AS(new_p->reallocate(buffer, 1), sight::core::memory::exception::memory);

        // Buffer No Alloc Policy
        auto no_alloc_p = std::make_shared<sight::core::memory::buffer_no_alloc_policy>();
        CHECK_THROWS_AS(no_alloc_p->allocate(buffer, 1), sight::core::memory::exception::memory);
        CHECK_THROWS_AS(no_alloc_p->reallocate(buffer, 1), sight::core::memory::exception::memory);
    }
} // TEST_SUITE
