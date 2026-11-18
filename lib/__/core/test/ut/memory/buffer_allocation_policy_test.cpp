/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include <algorithm>
#include <array>
#include <cstdint>
#include <span>

namespace
{

/// Tells whether its destructor ran: a buffer freed as bytes would not run it.
struct counted_element
{
    static inline int s_alive {0};

    counted_element()
    {
        ++s_alive;
    }

    ~counted_element()
    {
        --s_alive;
    }
};

} // namespace

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
        auto new_p = std::make_shared<sight::core::memory::buffer_new_policy<> >();
        CHECK_THROWS_AS(new_p->allocate(buffer, impossible_buffer_size), sight::core::memory::exception::memory);
        CHECK_THROWS_AS(new_p->reallocate(buffer, impossible_buffer_size), sight::core::memory::exception::memory);

        // The element count is rounded up without wrapping: the largest sizes are refused.
        auto new_double_p = std::make_shared<sight::core::memory::buffer_new_policy<double> >();
        CHECK_THROWS_AS(new_double_p->allocate(buffer, impossible_buffer_size), sight::core::memory::exception::memory);

        // Buffer No Alloc Policy
        auto no_alloc_p = std::make_shared<sight::core::memory::buffer_no_alloc_policy>();
        CHECK_THROWS_AS(no_alloc_p->allocate(buffer, 1), sight::core::memory::exception::memory);
        CHECK_THROWS_AS(no_alloc_p->reallocate(buffer, 1), sight::core::memory::exception::memory);
    }

//------------------------------------------------------------------------------

    TEST_CASE("new_policy_reallocates_with_content")
    {
        sight::core::memory::buffer_new_policy<std::int32_t> policy;
        sight::core::memory::buffer_allocation_policy::buffer_t buffer = nullptr;

        policy.allocate(buffer, 4 * sizeof(std::int32_t));
        REQUIRE(buffer != nullptr);
        std::ranges::copy(std::array {1, 2, 3, 4}, static_cast<std::int32_t*>(buffer));

        policy.reallocate(buffer, 8 * sizeof(std::int32_t));
        CHECK(std::ranges::equal(std::span(static_cast<std::int32_t*>(buffer), 4), std::array {1, 2, 3, 4}));

        policy.reallocate(buffer, 2 * sizeof(std::int32_t));
        CHECK(std::ranges::equal(std::span(static_cast<std::int32_t*>(buffer), 2), std::array {1, 2}));

        policy.reallocate(buffer, 0);
        CHECK(buffer == nullptr);

        policy.destroy(buffer);
        CHECK(buffer == nullptr);
    }

//------------------------------------------------------------------------------

    TEST_CASE("new_policy_refuses_to_reallocate_a_buffer_of_unknown_size")
    {
        sight::core::memory::buffer_new_policy<std::int32_t> policy;
        sight::core::memory::buffer_allocation_policy::buffer_t buffer = new std::int32_t[2] {1, 2};

        CHECK_THROWS_AS(policy.reallocate(buffer, 4 * sizeof(std::int32_t)), sight::core::memory::exception::memory);
        CHECK_EQ(static_cast<std::int32_t*>(buffer)[0], 1);
        CHECK_EQ(static_cast<std::int32_t*>(buffer)[1], 2);
        policy.destroy(buffer);
    }

//------------------------------------------------------------------------------

    TEST_CASE("new_policy_frees_a_buffer_with_the_type_of_its_elements")
    {
        sight::core::memory::buffer_new_policy<counted_element> policy;
        sight::core::memory::buffer_allocation_policy::buffer_t buffer = nullptr;

        policy.allocate(buffer, 4 * sizeof(counted_element));
        CHECK_EQ(counted_element::s_alive, 4);

        policy.reallocate(buffer, 2 * sizeof(counted_element));
        CHECK_EQ(counted_element::s_alive, 2);

        policy.destroy(buffer);
        CHECK_EQ(counted_element::s_alive, 0);
        CHECK_EQ(buffer, nullptr);
    }
} // TEST_SUITE
