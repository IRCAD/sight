/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "BufferObjectTest.hpp"

#include <fwMemory/BufferAllocationPolicy.hpp>
#include <fwMemory/BufferObject.hpp>
#include <fwMemory/exception/Memory.hpp>

#include <boost/thread/thread.hpp>

#include <functional>
#include <limits>
#include <type_traits>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMemory::ut::BufferObjectTest );

namespace fwMemory
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

template <typename T> bool isPointedValueConst( T )
{
    return std::is_const< typename std::remove_pointer< T >::type >::value;
}

//------------------------------------------------------------------------------

void BufferObjectTest::allocateTest()
{
    const size_t SIZE = 100000;
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

    CPPUNIT_ASSERT_THROW( bo->allocate(std::numeric_limits<size_t>::max()/2), ::fwMemory::exception::Memory);

    bo->allocate(SIZE);
    CPPUNIT_ASSERT( !bo->isEmpty() );
    CPPUNIT_ASSERT_EQUAL( static_cast< ::fwMemory::BufferObject::SizeType>(SIZE), bo->getSize() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() != NULL );

    CPPUNIT_ASSERT_THROW( bo->reallocate(std::numeric_limits<size_t>::max()/2), ::fwMemory::exception::Memory);

    CPPUNIT_ASSERT( !bo->isEmpty() );
    CPPUNIT_ASSERT_EQUAL( static_cast< ::fwMemory::BufferObject::SizeType>(SIZE), bo->getSize() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() != NULL );

    const size_t SMALLER_REALLOC_SIZE = 1024;
    const size_t BIGGER_REALLOC_SIZE  = SIZE+1024;

    bo->reallocate(SMALLER_REALLOC_SIZE);
    CPPUNIT_ASSERT( !bo->isEmpty() );
    CPPUNIT_ASSERT_EQUAL( static_cast< ::fwMemory::BufferObject::SizeType>(SMALLER_REALLOC_SIZE), bo->getSize() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() != NULL );

    bo->reallocate(BIGGER_REALLOC_SIZE);
    CPPUNIT_ASSERT( !bo->isEmpty() );
    CPPUNIT_ASSERT_EQUAL( static_cast< ::fwMemory::BufferObject::SizeType>(BIGGER_REALLOC_SIZE), bo->getSize() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() != NULL );

    bo->destroy();

    CPPUNIT_ASSERT( bo->isEmpty() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() == NULL );

    CPPUNIT_ASSERT_THROW( bo->allocate(150, ::fwMemory::BufferNoAllocPolicy::New()), ::fwMemory::exception::Memory);
    // CPPUNIT_ASSERT_THROW( bo->reallocate(150), ::fwMemory::exception::Memory);
}

//------------------------------------------------------------------------------

void BufferObjectTest::allocateZeroTest()
{
    const size_t SIZE = 100000;
    ::fwMemory::BufferObject::sptr bo = ::fwMemory::BufferObject::New();

    CPPUNIT_ASSERT( bo->isEmpty() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() == NULL );

    bo->allocate(0);

    CPPUNIT_ASSERT( bo->isEmpty() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() == NULL );

    bo->allocate(SIZE);

    CPPUNIT_ASSERT( !bo->isEmpty() );
    CPPUNIT_ASSERT_EQUAL( static_cast< ::fwMemory::BufferObject::SizeType>(SIZE), bo->getSize() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() != NULL );

}

//------------------------------------------------------------------------------

void stressLock(::fwMemory::BufferObject::sptr bo, int nbLocks, int nbTest)
{
    std::vector< ::fwMemory::BufferObject::Lock > m_locks;

    for( int t = 0; t < nbTest; ++t)
    {
        for( int i = 0; i < nbLocks; ++i)
        {
            m_locks.push_back(bo->lock());
        }

        CPPUNIT_ASSERT( bo->lockCount() >= static_cast<long>(3) );

        m_locks.clear();
    }

}

//------------------------------------------------------------------------------

void BufferObjectTest::lockThreadedStressTest()
{
    ::fwMemory::BufferObject::sptr bo = ::fwMemory::BufferObject::New();

    ::boost::thread_group group;

    group.create_thread( std::bind( &stressLock, bo, 800, 600 ) );
    group.create_thread( std::bind( &stressLock, bo, 600, 800 ) );
    group.create_thread( std::bind( &stressLock, bo, 452, 692 ) );
    group.create_thread( std::bind( &stressLock, bo, 253, 345 ) );

    group.join_all();

    CPPUNIT_ASSERT_EQUAL( static_cast<long>(0), bo->lockCount() );

}

} // namespace ut
} // namespace fwMemory

