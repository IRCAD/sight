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

#include "buffer_object_test.hpp"

#include <core/memory/buffer_allocation_policy.hpp>
#include <core/memory/buffer_object.hpp>
#include <core/memory/exception/memory.hpp>

#include <utest/wait.hpp>

#include <boost/thread/thread.hpp>

#include <chrono>
#include <functional>
#include <limits>
#include <thread>
#include <type_traits>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::memory::ut::buffer_object_test);

namespace sight::core::memory::ut
{

//------------------------------------------------------------------------------

void buffer_object_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void buffer_object_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

template<typename T>
bool is_pointed_value_const(T /*unused*/)
{
    return std::is_const_v<std::remove_pointer_t<T> >;
}

//------------------------------------------------------------------------------

void buffer_object_test::allocate_test()
{
    const std::size_t size               = 100000;
    core::memory::buffer_object::sptr bo = std::make_shared<core::memory::buffer_object>();

    CPPUNIT_ASSERT(bo->is_empty());
    CPPUNIT_ASSERT(bo->lock().buffer() == nullptr);

    bo->allocate(size);

    CPPUNIT_ASSERT(!bo->is_empty());
    CPPUNIT_ASSERT_EQUAL(static_cast<core::memory::buffer_object::size_t>(size), bo->size());
    CPPUNIT_ASSERT(bo->lock().buffer() != nullptr);

    // We need to wait before checking that the buffer was unlocked because all buffer operations are done on a worker.
    // The actual buffer ref count might still be owned (as a std::promise) by the worker task when we reach this point.
    SIGHT_TEST_WAIT(bo->lock_count() == 0);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::int64_t>(0), bo->lock_count());

    {
        core::memory::buffer_object::lock_t lock(bo->lock());
        SIGHT_TEST_WAIT(bo->lock_count() == 1);
        CPPUNIT_ASSERT_EQUAL(static_cast<std::int64_t>(1), bo->lock_count());
        char* buf = static_cast<char*>(lock.buffer());

        for(std::size_t i = 0 ; i < size ; ++i)
        {
            buf[i] = static_cast<char>(i % 256);
        }
    }

    {
        core::memory::buffer_object::lock_t lock(bo->lock());
        char* buf = static_cast<char*>(lock.buffer());

        for(std::size_t i = 0 ; i < size ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<char>(i % 256), buf[i]);
        }
    }

    SIGHT_TEST_WAIT(bo->lock_count() == 0);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::int64_t>(0), bo->lock_count());

    {
        core::memory::buffer_object::lock_t lock(bo->lock());

        SIGHT_TEST_WAIT(bo->lock_count() == 1);
        CPPUNIT_ASSERT_EQUAL(static_cast<std::int64_t>(1), bo->lock_count());
        core::memory::buffer_object::lock_t lock2(bo->lock());
        SIGHT_TEST_WAIT(bo->lock_count() == 2);
        CPPUNIT_ASSERT_EQUAL(static_cast<std::int64_t>(2), bo->lock_count());
        core::memory::buffer_object::csptr cbo = bo;
        core::memory::buffer_object::const_lock_t clock(cbo->lock());
        SIGHT_TEST_WAIT(bo->lock_count() == 3);
        CPPUNIT_ASSERT_EQUAL(static_cast<std::int64_t>(3), bo->lock_count());
        CPPUNIT_ASSERT(is_pointed_value_const(clock.buffer()));
        CPPUNIT_ASSERT(is_pointed_value_const(cbo->lock().buffer()));
    }

    SIGHT_TEST_WAIT(bo->lock_count() == 0);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::int64_t>(0), bo->lock_count());

    bo->destroy();

    CPPUNIT_ASSERT(bo->is_empty());
    CPPUNIT_ASSERT(bo->lock().buffer() == nullptr);

    CPPUNIT_ASSERT(bo->is_empty());
    CPPUNIT_ASSERT(bo->lock().buffer() == nullptr);

    bo->allocate(size, std::make_shared<core::memory::buffer_new_policy>());

    CPPUNIT_ASSERT(!bo->is_empty());
    CPPUNIT_ASSERT_EQUAL(static_cast<core::memory::buffer_object::size_t>(size), bo->size());
    CPPUNIT_ASSERT(bo->lock().buffer() != nullptr);

    {
        core::memory::buffer_object::lock_t lock(bo->lock());
        char* buf = static_cast<char*>(lock.buffer());

        for(std::size_t i = 0 ; i < size ; ++i)
        {
            buf[i] = static_cast<char>(i % 256);
        }
    }

    {
        core::memory::buffer_object::lock_t lock(bo->lock());
        char* buf = static_cast<char*>(lock.buffer());

        for(std::size_t i = 0 ; i < size ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<char>(i % 256), buf[i]);
        }
    }

    bo->destroy();
    CPPUNIT_ASSERT(bo->is_empty());
    CPPUNIT_ASSERT(bo->lock().buffer() == nullptr);

    CPPUNIT_ASSERT_THROW(bo->allocate(std::numeric_limits<std::size_t>::max() / 2), core::memory::exception::memory);

    bo->allocate(size);
    CPPUNIT_ASSERT(!bo->is_empty());
    CPPUNIT_ASSERT_EQUAL(static_cast<core::memory::buffer_object::size_t>(size), bo->size());
    CPPUNIT_ASSERT(bo->lock().buffer() != nullptr);

    CPPUNIT_ASSERT_THROW(bo->reallocate(std::numeric_limits<std::size_t>::max() / 2), core::memory::exception::memory);

    CPPUNIT_ASSERT(!bo->is_empty());
    CPPUNIT_ASSERT_EQUAL(static_cast<core::memory::buffer_object::size_t>(size), bo->size());
    CPPUNIT_ASSERT(bo->lock().buffer() != nullptr);

    const std::size_t smaller_realloc_size = 1024;
    const std::size_t bigger_realloc_size  = size + 1024;

    bo->reallocate(smaller_realloc_size);
    CPPUNIT_ASSERT(!bo->is_empty());
    CPPUNIT_ASSERT_EQUAL(static_cast<core::memory::buffer_object::size_t>(smaller_realloc_size), bo->size());
    CPPUNIT_ASSERT(bo->lock().buffer() != nullptr);

    bo->reallocate(bigger_realloc_size);
    CPPUNIT_ASSERT(!bo->is_empty());
    CPPUNIT_ASSERT_EQUAL(static_cast<core::memory::buffer_object::size_t>(bigger_realloc_size), bo->size());
    CPPUNIT_ASSERT(bo->lock().buffer() != nullptr);

    bo->destroy();

    CPPUNIT_ASSERT(bo->is_empty());
    CPPUNIT_ASSERT(bo->lock().buffer() == nullptr);

    CPPUNIT_ASSERT_THROW(
        bo->allocate(150, std::make_shared<core::memory::buffer_no_alloc_policy>()),
        core::memory::exception::memory
    );
    // CPPUNIT_ASSERT_THROW( bo->reallocate(150), core::memory::exception::Memory);
}

//------------------------------------------------------------------------------

void buffer_object_test::allocate_zero_test()
{
    const std::size_t size               = 100000;
    core::memory::buffer_object::sptr bo = std::make_shared<core::memory::buffer_object>();

    CPPUNIT_ASSERT(bo->is_empty());
    CPPUNIT_ASSERT(bo->lock().buffer() == nullptr);

    bo->allocate(0);

    CPPUNIT_ASSERT(bo->is_empty());
    CPPUNIT_ASSERT(bo->lock().buffer() == nullptr);

    bo->allocate(size);

    CPPUNIT_ASSERT(!bo->is_empty());
    CPPUNIT_ASSERT_EQUAL(static_cast<core::memory::buffer_object::size_t>(size), bo->size());
    CPPUNIT_ASSERT(bo->lock().buffer() != nullptr);

    bo->destroy();
}

//------------------------------------------------------------------------------

void stress_lock(core::memory::buffer_object::sptr _bo, int _nb_locks, int _nb_test)
{
    std::vector<core::memory::buffer_object::lock_t> m_locks;

    for(int t = 0 ; t < _nb_test ; ++t)
    {
        for(int i = 0 ; i < _nb_locks ; ++i)
        {
            m_locks.push_back(_bo->lock());
        }

        SIGHT_TEST_WAIT(_bo->lock_count() >= 3);
        CPPUNIT_ASSERT(_bo->lock_count() >= static_cast<std::int64_t>(3));

        m_locks.clear();
    }
}

//------------------------------------------------------------------------------

void buffer_object_test::lock_threaded_stress_test()
{
    core::memory::buffer_object::sptr bo = std::make_shared<core::memory::buffer_object>();

    boost::thread_group group;

    group.create_thread([bo](auto&& ...){return stress_lock(bo, 800, 600);});
    group.create_thread([bo](auto&& ...){return stress_lock(bo, 600, 800);});
    group.create_thread([bo](auto&& ...){return stress_lock(bo, 452, 692);});
    group.create_thread([bo](auto&& ...){return stress_lock(bo, 253, 345);});

    group.join_all();

    SIGHT_TEST_WAIT(bo->lock_count() == 0);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::int64_t>(0), bo->lock_count());
}

} // namespace sight::core::memory::ut
