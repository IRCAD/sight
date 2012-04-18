/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/IBufferManager.hpp>
#include <fwTools/BufferObject.hpp>
#include <fwTools/Exception.hpp>

#include <fwMemory/BufferManager.hpp>

#include "BufferManagerTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMemory::ut::BufferManagerTest );

namespace fwMemory
{
namespace ut
{

void BufferManagerTest::setUp()
{
    // Set up context before running a test.
}

void BufferManagerTest::tearDown()
{
    // Clean up after the test run.
}

void BufferManagerTest::allocateTest()
{
    ::fwMemory::BufferManager::NewSptr manager;
    ::fwTools::IBufferManager::setCurrent( manager );

    const int SIZE = 100000;
    ::fwTools::BufferObject::sptr bo = ::fwTools::BufferObject::New();

    CPPUNIT_ASSERT( bo->isEmpty() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() == NULL );

    bo->allocate(SIZE);

    CPPUNIT_ASSERT( !bo->isEmpty() );
    CPPUNIT_ASSERT_EQUAL( static_cast< ::fwTools::BufferObject::SizeType>(SIZE), bo->getSize() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() != NULL );

    CPPUNIT_ASSERT_EQUAL( static_cast<long>(0), bo->lockCount() );

    {
        ::fwTools::BufferObject::Lock lock(bo->lock());
        CPPUNIT_ASSERT_EQUAL( static_cast<long>(1), bo->lockCount() );
        char *buf = static_cast<char*>(lock.getBuffer());

        for (int i = 0; i<SIZE; ++i)
        {
            buf[i] = (i%256);
        }
    }

    {
        ::fwTools::BufferObject::Lock lock(bo->lock());
        char *buf = static_cast<char*>(lock.getBuffer());

        for (int i = 0; i<SIZE; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<char>(i%256), buf[i]);
        }
    }

    CPPUNIT_ASSERT_EQUAL( static_cast<long>(0), bo->lockCount() );

    {
        ::fwTools::BufferObject::Lock lock(bo->lock());
        CPPUNIT_ASSERT_EQUAL( static_cast<long>(1), bo->lockCount() );
        ::fwTools::BufferObject::Lock lock2(bo->lock());
        CPPUNIT_ASSERT_EQUAL( static_cast<long>(2), bo->lockCount() );
        ::fwTools::BufferObject::csptr cbo = bo;
        ::fwTools::BufferObject::ConstLock clock(cbo->lock());
        CPPUNIT_ASSERT_EQUAL( static_cast<long>(3), bo->lockCount() );
    }

    CPPUNIT_ASSERT_EQUAL( static_cast<long>(0), bo->lockCount() );

    bo->destroy();

    CPPUNIT_ASSERT( bo->isEmpty() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() == NULL );



    CPPUNIT_ASSERT( bo->isEmpty() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() == NULL );

    bo->allocate(SIZE, ::fwTools::BufferNewPolicy::New());

    CPPUNIT_ASSERT( !bo->isEmpty() );
    CPPUNIT_ASSERT_EQUAL( static_cast< ::fwTools::BufferObject::SizeType>(SIZE), bo->getSize() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() != NULL );


    {
        ::fwTools::BufferObject::Lock lock(bo->lock());
        char *buf = static_cast<char*>(lock.getBuffer());

        for (int i = 0; i<SIZE; ++i)
        {
            buf[i] = (i%256);
        }
    }

    {
        ::fwTools::BufferObject::Lock lock(bo->lock());
        char *buf = static_cast<char*>(lock.getBuffer());

        for (int i = 0; i<SIZE; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<char>(i%256), buf[i]);
        }
    }



    bo->destroy();

    CPPUNIT_ASSERT( bo->isEmpty() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() == NULL );
}

void BufferManagerTest::memoryInfoTest()
{
    ::fwMemory::BufferManager::NewSptr manager;
    ::fwTools::IBufferManager::setCurrent( manager );

    {
        SLM_INFO(manager->toString());
        ::fwTools::BufferObject::sptr bo = ::fwTools::BufferObject::New();
        const int SIZE = 100000;
        bo->allocate(SIZE);
        SLM_INFO(manager->toString());
        ::fwTools::BufferObject::sptr bo1 = ::fwTools::BufferObject::New();
        SLM_INFO(manager->toString());
        {
            ::fwTools::BufferObject::Lock lock1(bo1->lock());
            SLM_INFO(manager->toString());
        }
        ::fwTools::BufferObject::sptr bo2 = ::fwTools::BufferObject::New();
        SLM_INFO(manager->toString());
        bo->reallocate(SIZE*2);
        {
            ::fwTools::BufferObject::Lock lock(bo->lock());
            SLM_INFO(manager->toString());
        }
        bo->destroy();
        SLM_INFO(manager->toString());
        bo1->allocate(SIZE);
        bo2->allocate(SIZE);
        char * buff = new char[SIZE];
        bo->setBuffer( buff, SIZE, ::fwTools::BufferNewPolicy::New() );
        SLM_INFO(manager->toString());

        { ::fwTools::BufferObject::Lock lock(bo->lock()); }
        { ::fwTools::BufferObject::Lock lock(bo1->lock()); }
        { ::fwTools::BufferObject::Lock lock(bo2->lock()); }
    }
    SLM_INFO(manager->toString());
}

} // namespace ut
} // namespace fwMemory

