/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/BufferObject.hpp>
#include <fwTools/BufferAllocationPolicy.hpp>
#include <fwTools/Exception.hpp>

#include "BufferObjectTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwTools::ut::BufferObjectTest );

namespace fwTools
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

void BufferObjectTest::allocateTest()
{
    const int SIZE = 100000;
    ::fwTools::BufferObject::sptr bo = ::fwTools::BufferObject::New();

    CPPUNIT_ASSERT( bo->isNull() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() == NULL );

    bo->allocate(SIZE);

    CPPUNIT_ASSERT( !bo->isNull() );
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

        CPPUNIT_ASSERT_THROW( bo->allocate(150), ::fwTools::Exception);
        CPPUNIT_ASSERT_THROW( bo->reallocate(150), ::fwTools::Exception);
        CPPUNIT_ASSERT_THROW( bo->destroy(), ::fwTools::Exception);
        CPPUNIT_ASSERT_THROW( bo->setBuffer(0,0), ::fwTools::Exception);

    }

    CPPUNIT_ASSERT_EQUAL( static_cast<long>(0), bo->lockCount() );

    bo->destroy();

    CPPUNIT_ASSERT( bo->isNull() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() == NULL );




    CPPUNIT_ASSERT( bo->isNull() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() == NULL );

    bo->allocate(SIZE, ::fwTools::BufferNewPolicy::New());

    CPPUNIT_ASSERT( !bo->isNull() );
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

    CPPUNIT_ASSERT( bo->isNull() );
    CPPUNIT_ASSERT( bo->lock().getBuffer() == NULL );


}

} // namespace ut
} // namespace fwTools

