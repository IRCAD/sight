/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "buffer_allocation_policy_test.hpp"

#include <core/memory/buffer_allocation_policy.hpp>
#include <core/memory/exception/memory.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::memory::ut::buffer_allocation_policy_test);

namespace sight::core::memory::ut
{

//------------------------------------------------------------------------------

void buffer_allocation_policy_test::exception_test()
{
    const size_t impossible_buffer_size                     = std::numeric_limits<size_t>::max();
    core::memory::buffer_allocation_policy::buffer_t buffer = nullptr;

    // Buffer Malloc Policy
    core::memory::buffer_malloc_policy::sptr malloc_p = std::make_shared<core::memory::buffer_malloc_policy>();
    CPPUNIT_ASSERT_THROW(malloc_p->allocate(buffer, impossible_buffer_size), core::memory::exception::memory);
    CPPUNIT_ASSERT_THROW(malloc_p->reallocate(buffer, impossible_buffer_size), core::memory::exception::memory);

    // Buffer New Policy
    core::memory::buffer_new_policy::sptr new_p = std::make_shared<core::memory::buffer_new_policy>();
    CPPUNIT_ASSERT_THROW(new_p->allocate(buffer, impossible_buffer_size), core::memory::exception::memory);
    CPPUNIT_ASSERT_THROW(new_p->reallocate(buffer, 1), core::memory::exception::memory);

    // Buffer No Alloc Policy
    core::memory::buffer_no_alloc_policy::sptr no_alloc_p = std::make_shared<core::memory::buffer_no_alloc_policy>();
    CPPUNIT_ASSERT_THROW(no_alloc_p->allocate(buffer, 1), core::memory::exception::memory);
    CPPUNIT_ASSERT_THROW(no_alloc_p->reallocate(buffer, 1), core::memory::exception::memory);
}

} // namespace sight::core::memory::ut
