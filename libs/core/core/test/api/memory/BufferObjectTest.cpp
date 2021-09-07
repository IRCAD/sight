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

#include "BufferObjectTest.hpp"

#include <core/memory/BufferAllocationPolicy.hpp>
#include <core/memory/BufferObject.hpp>
#include <core/memory/exception/Memory.hpp>

#include <utest/wait.hpp>

#include <boost/thread/thread.hpp>

#include <chrono>
#include <functional>
#include <limits>
#include <thread>
#include <type_traits>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::memory::ut::BufferObjectTest);

namespace sight::core::memory
{

namespace ut
{

//------------------------------------------------------------------------------

void BufferObjectTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void BufferObjectTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

template<typename T>
bool isPointedValueConst(T)
{
    return std::is_const<typename std::remove_pointer<T>::type>::value;
}

//------------------------------------------------------------------------------

void BufferObjectTest::allocateTest()
{
    const size_t SIZE                   = 100000;
    core::memory::BufferObject::sptr bo = core::memory::BufferObject::New();

    CPPUNIT_ASSERT(bo->isEmpty());
    CPPUNIT_ASSERT(bo->lock().getBuffer() == NULL);

    bo->allocate(SIZE);

    CPPUNIT_ASSERT(!bo->isEmpty());
    CPPUNIT_ASSERT_EQUAL(static_cast<core::memory::BufferObject::SizeType>(SIZE), bo->getSize());
    CPPUNIT_ASSERT(bo->lock().getBuffer() != NULL);

    // We need to wait before checking that the buffer was unlocked because all buffer operations are done on a worker.
    // The actual buffer ref count might still be owned (as a std::promise) by the worker task when we reach this point.
    fwTestWaitMacro(bo->lockCount() == 0);
    CPPUNIT_ASSERT_EQUAL(static_cast<long>(0), bo->lockCount());

    {
        core::memory::BufferObject::Lock lock(bo->lock());
        fwTestWaitMacro(bo->lockCount() == 1);
        CPPUNIT_ASSERT_EQUAL(static_cast<long>(1), bo->lockCount());
        char* buf = static_cast<char*>(lock.getBuffer());

        for(size_t i = 0 ; i < SIZE ; ++i)
        {
            buf[i] = static_cast<char>(i % 256);
        }
    }

    {
        core::memory::BufferObject::Lock lock(bo->lock());
        char* buf = static_cast<char*>(lock.getBuffer());

        for(size_t i = 0 ; i < SIZE ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<char>(i % 256), buf[i]);
        }
    }

    fwTestWaitMacro(bo->lockCount() == 0);
    CPPUNIT_ASSERT_EQUAL(static_cast<long>(0), bo->lockCount());

    {
        core::memory::BufferObject::Lock lock(bo->lock());

        fwTestWaitMacro(bo->lockCount() == 1);
        CPPUNIT_ASSERT_EQUAL(static_cast<long>(1), bo->lockCount());
        core::memory::BufferObject::Lock lock2(bo->lock());
        fwTestWaitMacro(bo->lockCount() == 2);
        CPPUNIT_ASSERT_EQUAL(static_cast<long>(2), bo->lockCount());
        core::memory::BufferObject::csptr cbo = bo;
        core::memory::BufferObject::ConstLock clock(cbo->lock());
        fwTestWaitMacro(bo->lockCount() == 3);
        CPPUNIT_ASSERT_EQUAL(static_cast<long>(3), bo->lockCount());
        CPPUNIT_ASSERT(isPointedValueConst(clock.getBuffer()));
        CPPUNIT_ASSERT(isPointedValueConst(cbo->lock().getBuffer()));
    }

    fwTestWaitMacro(bo->lockCount() == 0);
    CPPUNIT_ASSERT_EQUAL(static_cast<long>(0), bo->lockCount());

    bo->destroy();

    CPPUNIT_ASSERT(bo->isEmpty());
    CPPUNIT_ASSERT(bo->lock().getBuffer() == NULL);

    CPPUNIT_ASSERT(bo->isEmpty());
    CPPUNIT_ASSERT(bo->lock().getBuffer() == NULL);

    bo->allocate(SIZE, core::memory::BufferNewPolicy::New());

    CPPUNIT_ASSERT(!bo->isEmpty());
    CPPUNIT_ASSERT_EQUAL(static_cast<core::memory::BufferObject::SizeType>(SIZE), bo->getSize());
    CPPUNIT_ASSERT(bo->lock().getBuffer() != NULL);

    {
        core::memory::BufferObject::Lock lock(bo->lock());
        char* buf = static_cast<char*>(lock.getBuffer());

        for(size_t i = 0 ; i < SIZE ; ++i)
        {
            buf[i] = static_cast<char>(i % 256);
        }
    }

    {
        core::memory::BufferObject::Lock lock(bo->lock());
        char* buf = static_cast<char*>(lock.getBuffer());

        for(size_t i = 0 ; i < SIZE ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<char>(i % 256), buf[i]);
        }
    }

    bo->destroy();
    CPPUNIT_ASSERT(bo->isEmpty());
    CPPUNIT_ASSERT(bo->lock().getBuffer() == NULL);

    CPPUNIT_ASSERT_THROW(bo->allocate(std::numeric_limits<size_t>::max() / 2), core::memory::exception::Memory);

    bo->allocate(SIZE);
    CPPUNIT_ASSERT(!bo->isEmpty());
    CPPUNIT_ASSERT_EQUAL(static_cast<core::memory::BufferObject::SizeType>(SIZE), bo->getSize());
    CPPUNIT_ASSERT(bo->lock().getBuffer() != NULL);

    CPPUNIT_ASSERT_THROW(bo->reallocate(std::numeric_limits<size_t>::max() / 2), core::memory::exception::Memory);

    CPPUNIT_ASSERT(!bo->isEmpty());
    CPPUNIT_ASSERT_EQUAL(static_cast<core::memory::BufferObject::SizeType>(SIZE), bo->getSize());
    CPPUNIT_ASSERT(bo->lock().getBuffer() != NULL);

    const size_t SMALLER_REALLOC_SIZE = 1024;
    const size_t BIGGER_REALLOC_SIZE  = SIZE + 1024;

    bo->reallocate(SMALLER_REALLOC_SIZE);
    CPPUNIT_ASSERT(!bo->isEmpty());
    CPPUNIT_ASSERT_EQUAL(static_cast<core::memory::BufferObject::SizeType>(SMALLER_REALLOC_SIZE), bo->getSize());
    CPPUNIT_ASSERT(bo->lock().getBuffer() != NULL);

    bo->reallocate(BIGGER_REALLOC_SIZE);
    CPPUNIT_ASSERT(!bo->isEmpty());
    CPPUNIT_ASSERT_EQUAL(static_cast<core::memory::BufferObject::SizeType>(BIGGER_REALLOC_SIZE), bo->getSize());
    CPPUNIT_ASSERT(bo->lock().getBuffer() != NULL);

    bo->destroy();

    CPPUNIT_ASSERT(bo->isEmpty());
    CPPUNIT_ASSERT(bo->lock().getBuffer() == NULL);

    CPPUNIT_ASSERT_THROW(bo->allocate(150, core::memory::BufferNoAllocPolicy::New()), core::memory::exception::Memory);
    // CPPUNIT_ASSERT_THROW( bo->reallocate(150), core::memory::exception::Memory);
}

//------------------------------------------------------------------------------

void BufferObjectTest::allocateZeroTest()
{
    const size_t SIZE                   = 100000;
    core::memory::BufferObject::sptr bo = core::memory::BufferObject::New();

    CPPUNIT_ASSERT(bo->isEmpty());
    CPPUNIT_ASSERT(bo->lock().getBuffer() == NULL);

    bo->allocate(0);

    CPPUNIT_ASSERT(bo->isEmpty());
    CPPUNIT_ASSERT(bo->lock().getBuffer() == NULL);

    bo->allocate(SIZE);

    CPPUNIT_ASSERT(!bo->isEmpty());
    CPPUNIT_ASSERT_EQUAL(static_cast<core::memory::BufferObject::SizeType>(SIZE), bo->getSize());
    CPPUNIT_ASSERT(bo->lock().getBuffer() != NULL);

    bo->destroy();
}

//------------------------------------------------------------------------------

void stressLock(core::memory::BufferObject::sptr bo, int nbLocks, int nbTest)
{
    std::vector<core::memory::BufferObject::Lock> m_locks;

    for(int t = 0 ; t < nbTest ; ++t)
    {
        for(int i = 0 ; i < nbLocks ; ++i)
        {
            m_locks.push_back(bo->lock());
        }

        fwTestWaitMacro(bo->lockCount() >= 3);
        CPPUNIT_ASSERT(bo->lockCount() >= static_cast<long>(3));

        m_locks.clear();
    }
}

//------------------------------------------------------------------------------

void BufferObjectTest::lockThreadedStressTest()
{
    core::memory::BufferObject::sptr bo = core::memory::BufferObject::New();

    ::boost::thread_group group;

    group.create_thread(std::bind(&stressLock, bo, 800, 600));
    group.create_thread(std::bind(&stressLock, bo, 600, 800));
    group.create_thread(std::bind(&stressLock, bo, 452, 692));
    group.create_thread(std::bind(&stressLock, bo, 253, 345));

    group.join_all();

    fwTestWaitMacro(bo->lockCount() == 0);
    CPPUNIT_ASSERT_EQUAL(static_cast<long>(0), bo->lockCount());
}

} // namespace ut

} // namespace sight::core::memory
