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

#include "BufferManagerTest.hpp"

#include <core/memory/BufferManager.hpp>
#include <core/memory/BufferObject.hpp>

#include <utest/wait.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( sight::core::memory::ut::BufferManagerTest );

namespace sight::core::memory
{
namespace ut
{

//------------------------------------------------------------------------------

void BufferManagerTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void BufferManagerTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void BufferManagerTest::allocateTest()
{
    core::memory::BufferManager::sptr manager = core::memory::BufferManager::getDefault();

    const int SIZE                      = 100000;
    core::memory::BufferObject::sptr bo = core::memory::BufferObject::New();

    CPPUNIT_ASSERT( bo->isEmpty() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() == NULL );

    bo->allocate(SIZE);

    CPPUNIT_ASSERT( !bo->isEmpty() );
    CPPUNIT_ASSERT_EQUAL( static_cast< core::memory::BufferObject::SizeType>(SIZE), bo->getSize() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() != NULL );

    // We need to wait before checking that the buffer was unlocked because all buffer operations are done on a worker.
    // The actual buffer ref count might still be owned (as a std::promise) by the worker task when we reach this point.
    fwTestWaitMacro(bo->lockCount() == 0);
    CPPUNIT_ASSERT_EQUAL( static_cast<long>(0), bo->lockCount() );

    {
        core::memory::BufferObject::Lock lock(bo->lock());

        fwTestWaitMacro(bo->lockCount() == 1);
        CPPUNIT_ASSERT_EQUAL( static_cast<long>(1), bo->lockCount() );
        char* buf = static_cast<char*>(lock.getBuffer());

        for (int i = 0; i < SIZE; ++i)
        {
            buf[i] = static_cast<char>(i%256);
        }
    }

    {
        core::memory::BufferObject::Lock lock(bo->lock());
        char* buf = static_cast<char*>(lock.getBuffer());

        for (int i = 0; i < SIZE; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<char>(i%256), buf[i]);
        }
    }

    fwTestWaitMacro(bo->lockCount() == 0);
    CPPUNIT_ASSERT_EQUAL( static_cast<long>(0), bo->lockCount() );

    {
        core::memory::BufferObject::Lock lock(bo->lock());

        fwTestWaitMacro(bo->lockCount() == 1);
        CPPUNIT_ASSERT_EQUAL( static_cast<long>(1), bo->lockCount() );
        core::memory::BufferObject::Lock lock2(bo->lock());
        fwTestWaitMacro(bo->lockCount() == 2);
        CPPUNIT_ASSERT_EQUAL( static_cast<long>(2), bo->lockCount() );
        core::memory::BufferObject::csptr cbo = bo;
        core::memory::BufferObject::ConstLock clock(cbo->lock());
        fwTestWaitMacro(bo->lockCount() == 3);
        CPPUNIT_ASSERT_EQUAL( static_cast<long>(3), bo->lockCount() );
    }

    fwTestWaitMacro(bo->lockCount() == 0);
    CPPUNIT_ASSERT_EQUAL( static_cast<long>(0), bo->lockCount() );

    bo->destroy();

    CPPUNIT_ASSERT( bo->isEmpty() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() == NULL );

    CPPUNIT_ASSERT( bo->isEmpty() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() == NULL );

    bo->allocate(SIZE, core::memory::BufferNewPolicy::New());

    CPPUNIT_ASSERT( !bo->isEmpty() );
    CPPUNIT_ASSERT_EQUAL( static_cast< core::memory::BufferObject::SizeType>(SIZE), bo->getSize() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() != NULL );

    {
        core::memory::BufferObject::Lock lock(bo->lock());
        char* buf = static_cast<char*>(lock.getBuffer());

        for (int i = 0; i < SIZE; ++i)
        {
            buf[i] = static_cast<char>(i%256);
        }
    }

    {
        core::memory::BufferObject::Lock lock(bo->lock());
        char* buf = static_cast<char*>(lock.getBuffer());

        for (int i = 0; i < SIZE; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<char>(i%256), buf[i]);
        }
    }

    bo->destroy();

    CPPUNIT_ASSERT( bo->isEmpty() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() == NULL );
}

//------------------------------------------------------------------------------

void BufferManagerTest::memoryInfoTest()
{
    core::memory::BufferManager::sptr manager = core::memory::BufferManager::getDefault();

    {
        SLM_INFO(manager->toString().get());
        core::memory::BufferObject::sptr bo = core::memory::BufferObject::New();
        const int SIZE                      = 100000;
        bo->allocate(SIZE);
        SLM_INFO(manager->toString().get());
        core::memory::BufferObject::sptr bo1 = core::memory::BufferObject::New();
        SLM_INFO(manager->toString().get());
        {
            core::memory::BufferObject::Lock lock1(bo1->lock());
            SLM_INFO(manager->toString().get());
        }
        core::memory::BufferObject::sptr bo2 = core::memory::BufferObject::New();
        SLM_INFO(manager->toString().get());
        bo->reallocate(SIZE*2);
        {
            core::memory::BufferObject::Lock lock(bo->lock());
            SLM_INFO(manager->toString().get());
        }
        bo->destroy();
        SLM_INFO(manager->toString().get());
        bo1->allocate(SIZE);
        bo2->allocate(SIZE);
        char* buff = new char[SIZE];
        bo->setBuffer( buff, SIZE, core::memory::BufferNewPolicy::New() );
        SLM_INFO(manager->toString().get());

        { core::memory::BufferObject::Lock lock(bo->lock()); }
        { core::memory::BufferObject::Lock lock(bo1->lock()); }
        { core::memory::BufferObject::Lock lock(bo2->lock()); }

        bo->destroy();
        bo1->destroy();
        bo2->destroy();
    }
    SLM_INFO(manager->toString().get());
}

} // namespace ut
} // namespace sight::core::memory
