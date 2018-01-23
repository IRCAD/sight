/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "BufferManagerTest.hpp"

#include <fwMemory/BufferManager.hpp>
#include <fwMemory/BufferObject.hpp>

#include <chrono>
#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMemory::ut::BufferManagerTest );

namespace fwMemory
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
    ::fwMemory::BufferManager::sptr manager = ::fwMemory::BufferManager::getDefault();

    const int SIZE = 100000;
    ::fwMemory::BufferObject::sptr bo = ::fwMemory::BufferObject::New();

    CPPUNIT_ASSERT( bo->isEmpty() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() == NULL );

    bo->allocate(SIZE);

    CPPUNIT_ASSERT( !bo->isEmpty() );
    CPPUNIT_ASSERT_EQUAL( static_cast< ::fwMemory::BufferObject::SizeType>(SIZE), bo->getSize() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() != NULL );

    // We need to wait before checking that the buffer was unlocked because the actual unlocking is performed on a
    // worker for thread safety and the buffer might take a little time to actually unlock. Not waiting causes random
    // errors in the test that tend to pop when the machine is under load.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CPPUNIT_ASSERT_EQUAL( static_cast<long>(0), bo->lockCount() );

    {
        ::fwMemory::BufferObject::Lock lock(bo->lock());

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        CPPUNIT_ASSERT_EQUAL( static_cast<long>(1), bo->lockCount() );
        char* buf = static_cast<char*>(lock.getBuffer());

        for (int i = 0; i < SIZE; ++i)
        {
            buf[i] = (i%256);
        }
    }

    {
        ::fwMemory::BufferObject::Lock lock(bo->lock());
        char* buf = static_cast<char*>(lock.getBuffer());

        for (int i = 0; i < SIZE; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<char>(i%256), buf[i]);
        }
    }

    CPPUNIT_ASSERT_EQUAL( static_cast<long>(0), bo->lockCount() );

    {
        ::fwMemory::BufferObject::Lock lock(bo->lock());

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        CPPUNIT_ASSERT_EQUAL( static_cast<long>(1), bo->lockCount() );
        ::fwMemory::BufferObject::Lock lock2(bo->lock());
        CPPUNIT_ASSERT_EQUAL( static_cast<long>(2), bo->lockCount() );
        ::fwMemory::BufferObject::csptr cbo = bo;
        ::fwMemory::BufferObject::ConstLock clock(cbo->lock());
        CPPUNIT_ASSERT_EQUAL( static_cast<long>(3), bo->lockCount() );
    }

    CPPUNIT_ASSERT_EQUAL( static_cast<long>(0), bo->lockCount() );

    bo->destroy();

    CPPUNIT_ASSERT( bo->isEmpty() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() == NULL );

    CPPUNIT_ASSERT( bo->isEmpty() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() == NULL );

    bo->allocate(SIZE, ::fwMemory::BufferNewPolicy::New());

    CPPUNIT_ASSERT( !bo->isEmpty() );
    CPPUNIT_ASSERT_EQUAL( static_cast< ::fwMemory::BufferObject::SizeType>(SIZE), bo->getSize() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() != NULL );

    {
        ::fwMemory::BufferObject::Lock lock(bo->lock());
        char* buf = static_cast<char*>(lock.getBuffer());

        for (int i = 0; i < SIZE; ++i)
        {
            buf[i] = (i%256);
        }
    }

    {
        ::fwMemory::BufferObject::Lock lock(bo->lock());
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
    ::fwMemory::BufferManager::sptr manager = ::fwMemory::BufferManager::getDefault();

    {
        SLM_INFO(manager->toString().get());
        ::fwMemory::BufferObject::sptr bo = ::fwMemory::BufferObject::New();
        const int SIZE = 100000;
        bo->allocate(SIZE);
        SLM_INFO(manager->toString().get());
        ::fwMemory::BufferObject::sptr bo1 = ::fwMemory::BufferObject::New();
        SLM_INFO(manager->toString().get());
        {
            ::fwMemory::BufferObject::Lock lock1(bo1->lock());
            SLM_INFO(manager->toString().get());
        }
        ::fwMemory::BufferObject::sptr bo2 = ::fwMemory::BufferObject::New();
        SLM_INFO(manager->toString().get());
        bo->reallocate(SIZE*2);
        {
            ::fwMemory::BufferObject::Lock lock(bo->lock());
            SLM_INFO(manager->toString().get());
        }
        bo->destroy();
        SLM_INFO(manager->toString().get());
        bo1->allocate(SIZE);
        bo2->allocate(SIZE);
        char* buff = new char[SIZE];
        bo->setBuffer( buff, SIZE, ::fwMemory::BufferNewPolicy::New() );
        SLM_INFO(manager->toString().get());

        { ::fwMemory::BufferObject::Lock lock(bo->lock()); }
        { ::fwMemory::BufferObject::Lock lock(bo1->lock()); }
        { ::fwMemory::BufferObject::Lock lock(bo2->lock()); }
    }
    SLM_INFO(manager->toString().get());
}

} // namespace ut
} // namespace fwMemory
