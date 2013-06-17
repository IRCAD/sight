/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <boost/type_traits/is_const.hpp>
#include <boost/type_traits/remove_pointer.hpp>

#include <limits>

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <fwMemory/BufferObject.hpp>
#include <fwMemory/BufferAllocationPolicy.hpp>
#include <fwMemory/exception/Memory.hpp>

#include "BufferObjectTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMemory::ut::BufferObjectTest );

namespace fwMemory
{
namespace ut
{

void BufferObjectTest::setUp()
{
    // Set up context before running a test.
}

void BufferObjectTest::tearDown()
{
    // Clean up after the test run.
}


template <typename T> bool isPointedValueConst( T )
{
    return ::boost::is_const< typename boost::remove_pointer< T >::type >::value;
}

void BufferObjectTest::allocateTest()
{
    const int SIZE = 100000;
    ::fwMemory::BufferObject::sptr bo = ::fwMemory::BufferObject::New();

    CPPUNIT_ASSERT( bo->isEmpty() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() == NULL );

    bo->allocate(SIZE);

    CPPUNIT_ASSERT( !bo->isEmpty() );
    CPPUNIT_ASSERT_EQUAL( static_cast< ::fwMemory::BufferObject::SizeType>(SIZE), bo->getSize() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() != NULL );

    CPPUNIT_ASSERT_EQUAL( static_cast<long>(0), bo->lockCount() );

    {
        ::fwMemory::BufferObject::Lock lock(bo->lock());
        CPPUNIT_ASSERT_EQUAL( static_cast<long>(1), bo->lockCount() );
        char *buf = static_cast<char*>(lock.getBuffer());

        for (int i = 0; i<SIZE; ++i)
        {
            buf[i] = (i%256);
        }
    }

    {
        ::fwMemory::BufferObject::Lock lock(bo->lock());
        char *buf = static_cast<char*>(lock.getBuffer());

        for (int i = 0; i<SIZE; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<char>(i%256), buf[i]);
        }
    }

    CPPUNIT_ASSERT_EQUAL( static_cast<long>(0), bo->lockCount() );

    {
        ::fwMemory::BufferObject::Lock lock(bo->lock());
        CPPUNIT_ASSERT_EQUAL( static_cast<long>(1), bo->lockCount() );
        ::fwMemory::BufferObject::Lock lock2(bo->lock());
        CPPUNIT_ASSERT_EQUAL( static_cast<long>(2), bo->lockCount() );
        ::fwMemory::BufferObject::csptr cbo = bo;
        ::fwMemory::BufferObject::ConstLock clock(cbo->lock());
        CPPUNIT_ASSERT_EQUAL( static_cast<long>(3), bo->lockCount() );
        CPPUNIT_ASSERT( isPointedValueConst( clock.getBuffer() ) );
        CPPUNIT_ASSERT( isPointedValueConst( cbo->lock().getBuffer() ) );
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
        char *buf = static_cast<char*>(lock.getBuffer());

        for (int i = 0; i<SIZE; ++i)
        {
            buf[i] = (i%256);
        }
    }

    {
        ::fwMemory::BufferObject::Lock lock(bo->lock());
        char *buf = static_cast<char*>(lock.getBuffer());

        for (int i = 0; i<SIZE; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<char>(i%256), buf[i]);
        }
    }

    bo->destroy();
    CPPUNIT_ASSERT( bo->isEmpty() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() == NULL );

    CPPUNIT_ASSERT_THROW( bo->allocate(std::numeric_limits<size_t>::max()/2), ::fwMemory::exception::Memory);
    CPPUNIT_ASSERT_THROW( bo->reallocate(std::numeric_limits<size_t>::max()/2), ::fwMemory::exception::Memory);

    CPPUNIT_ASSERT_THROW( bo->allocate(150, ::fwMemory::BufferNoAllocPolicy::New()), ::fwMemory::exception::Memory);
    CPPUNIT_ASSERT_THROW( bo->reallocate(150), ::fwMemory::exception::Memory);
}



void stressLock(::fwMemory::BufferObject::sptr bo, int nbLocks, int nbTest)
{
    std::vector< ::fwMemory::BufferObject::Lock > m_locks;

    for( int t = 0; t < nbTest ; ++t)
    {
        for( int i = 0; i < nbLocks ; ++i)
        {
            m_locks.push_back(bo->lock());
        }

        CPPUNIT_ASSERT( bo->lockCount() >= static_cast<long>(3) );

        m_locks.clear();
    }

}

void BufferObjectTest::lockThreadedStressTest()
{
    ::fwMemory::BufferObject::sptr bo = ::fwMemory::BufferObject::New();

    boost::thread_group group;

    group.create_thread( boost::bind( &stressLock, bo, 800, 600 ) );
    group.create_thread( boost::bind( &stressLock, bo, 600, 800 ) );
    group.create_thread( boost::bind( &stressLock, bo, 452, 692 ) );
    group.create_thread( boost::bind( &stressLock, bo, 253, 345 ) );

    group.join_all();

    CPPUNIT_ASSERT_EQUAL( static_cast<long>(0), bo->lockCount() );

}



} // namespace ut
} // namespace fwMemory

