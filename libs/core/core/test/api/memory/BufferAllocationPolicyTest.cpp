/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "BufferAllocationPolicyTest.hpp"

#include <core/memory/BufferAllocationPolicy.hpp>
#include <core/memory/exception/Memory.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::memory::ut::BufferAllocationPolicyTest);

namespace sight::core::memory::ut
{

//------------------------------------------------------------------------------

void BufferAllocationPolicyTest::exceptionTest()
{
    const size_t impossibleBufferSize                       = std::numeric_limits<size_t>::max();
    core::memory::BufferAllocationPolicy::BufferType buffer = nullptr;

    // Buffer Malloc Policy
    core::memory::BufferMallocPolicy::sptr mallocP = core::memory::BufferMallocPolicy::New();
    CPPUNIT_ASSERT_THROW(mallocP->allocate(buffer, impossibleBufferSize), core::memory::exception::Memory);
    CPPUNIT_ASSERT_THROW(mallocP->reallocate(buffer, impossibleBufferSize), core::memory::exception::Memory);

    // Buffer New Policy
    core::memory::BufferNewPolicy::sptr newP = core::memory::BufferNewPolicy::New();
    CPPUNIT_ASSERT_THROW(newP->allocate(buffer, impossibleBufferSize), core::memory::exception::Memory);
    CPPUNIT_ASSERT_THROW(newP->reallocate(buffer, 1), core::memory::exception::Memory);

    // Buffer No Alloc Policy
    core::memory::BufferNoAllocPolicy::sptr noAllocP = core::memory::BufferNoAllocPolicy::New();
    CPPUNIT_ASSERT_THROW(noAllocP->allocate(buffer, 1), core::memory::exception::Memory);
    CPPUNIT_ASSERT_THROW(noAllocP->reallocate(buffer, 1), core::memory::exception::Memory);
}

} // namespace sight::core::memory::ut
