/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Image.hpp>
#include <fwData/Composite.hpp>

#include <fwDataTools/Image.hpp>

#include <fwDataCamp/visitor/RecursiveLock.hpp>

#include "RecursiveLockTest.hpp"


CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataCamp::ut::RecursiveLockTest );

namespace fwDataCamp
{
namespace ut
{

//-----------------------------------------------------------------------------

// Set up context before running a test.
void RecursiveLockTest::setUp()
{};

//-----------------------------------------------------------------------------

void RecursiveLockTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void RecursiveLockTest::lockTest()
{
    {
        ::fwData::Image::sptr image = ::fwData::Image::New();
        ::fwDataTools::Image::generateRandomImage(image, ::fwTools::Type::create("int16"));
        ::fwData::Array::sptr array = image->getDataArray();
        ::fwTools::BufferObject::sptr bo = array->getBufferObject();

        {
            ::fwDataCamp::visitor::RecursiveLock visitor(image);

            CPPUNIT_ASSERT(!image->getMutex().try_lock());
            CPPUNIT_ASSERT(!array->getMutex().try_lock());
            CPPUNIT_ASSERT(!bo->getMutex().try_lock());
        }

        CPPUNIT_ASSERT(image->getMutex().try_lock());
        CPPUNIT_ASSERT(array->getMutex().try_lock());
        CPPUNIT_ASSERT(bo->getMutex().try_lock());
    }

    {
        ::fwData::Image::sptr img1 = ::fwData::Image::New();
        ::fwDataTools::Image::generateRandomImage(img1, ::fwTools::Type::create("int16"));
        ::fwData::Image::sptr img2 = ::fwData::Image::New();
        ::fwDataTools::Image::generateRandomImage(img2, ::fwTools::Type::create("uint8"));
        ::fwData::Composite::sptr composite = ::fwData::Composite::New();
        composite->getContainer()["img1"] = img1;
        composite->getContainer()["img2"] = img2;

        ::fwData::Array::sptr array1 = img1->getDataArray();
        ::fwData::Array::sptr array2 = img2->getDataArray();
        ::fwTools::BufferObject::sptr bo1 = array1->getBufferObject();
        ::fwTools::BufferObject::sptr bo2 = array2->getBufferObject();

        {
            ::fwDataCamp::visitor::RecursiveLock visitor(composite);

            CPPUNIT_ASSERT(!composite->getMutex().try_lock());
            CPPUNIT_ASSERT(!img1->getMutex().try_lock());
            CPPUNIT_ASSERT(!img2->getMutex().try_lock());
            CPPUNIT_ASSERT(!array1->getMutex().try_lock());
            CPPUNIT_ASSERT(!array2->getMutex().try_lock());
            CPPUNIT_ASSERT(!bo1->getMutex().try_lock());
            CPPUNIT_ASSERT(!bo2->getMutex().try_lock());
        }

        CPPUNIT_ASSERT(composite->getMutex().try_lock());
        CPPUNIT_ASSERT(img1->getMutex().try_lock());
        CPPUNIT_ASSERT(img2->getMutex().try_lock());
        CPPUNIT_ASSERT(array1->getMutex().try_lock());
        CPPUNIT_ASSERT(array2->getMutex().try_lock());
        CPPUNIT_ASSERT(bo1->getMutex().try_lock());
        CPPUNIT_ASSERT(bo2->getMutex().try_lock());

    }
}

//-----------------------------------------------------------------------------

}  // namespace ut
}  // namespace fwDataCamp
