/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "ImageTest.hpp"

#include <fwData/Image.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/Image.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <exception>
#include <iostream>
#include <map>
#include <ostream>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::ImageTest );

namespace fwData
{
namespace ut
{

//------------------------------------------------------------------------------

void ImageTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ImageTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ImageTest::testGetterSetter()
{
    const size_t DIMENSION                    = 2;
    const ::fwTools::Type TYPE                = ::fwTools::Type::create("int16");
    const ::fwData::Image::Spacing SPACING    = {2.5, 2.6};
    const ::fwData::Image::Origin ORIGIN      = {2.7, 2.8};
    const ::fwData::Image::Size SIZE          = {42, 43};
    const double WINDOWCENTER                 = 10.10;
    const double WINDOWWIDTH                  = 11.34;
    const ::fwData::Image::PixelFormat FORMAT = ::fwData::Image::PixelFormat::GRAY_SCALE;

    // Old API
    const ::fwData::Image::SpacingType VECTORSPACING = {2.5, 2.6};
    const ::fwData::Image::OriginType VECTORORIGIN   = {2.7, 2.8};
    const ::fwData::Image::SizeType VECTORSIZE       = {42, 43};

    // process
    ::fwData::Image::sptr img1 = ::fwData::Image::New();

    img1->setType( TYPE);
    img1->setSpacing2(SPACING);
    img1->setOrigin2(ORIGIN);
    img1->setSize2(SIZE);
    img1->setWindowCenter(WINDOWCENTER);
    img1->setWindowWidth( WINDOWWIDTH );
    img1->setPixelFormat(FORMAT );

    // check
    CPPUNIT_ASSERT_EQUAL(DIMENSION, img1->getNumberOfDimensions());
    CPPUNIT_ASSERT(img1->getType() == TYPE);
    CPPUNIT_ASSERT(img1->getSpacing2() == SPACING);
    CPPUNIT_ASSERT(img1->getOrigin2() == ORIGIN);
    CPPUNIT_ASSERT(img1->getSize2() == SIZE);
    CPPUNIT_ASSERT_EQUAL(WINDOWCENTER, img1->getWindowCenter());
    CPPUNIT_ASSERT_EQUAL(WINDOWWIDTH, img1->getWindowWidth());
    CPPUNIT_ASSERT_EQUAL(FORMAT, img1->getPixelFormat());

    // Old API

    ::fwData::Image::sptr img2 = ::fwData::Image::New();
    img2->setType( TYPE);
    img2->setSpacing(VECTORSPACING);
    img2->setOrigin(VECTORORIGIN);
    img2->setSize(VECTORSIZE);
    img2->setWindowCenter(WINDOWCENTER);
    img2->setWindowWidth( WINDOWWIDTH );

    // check
    CPPUNIT_ASSERT_EQUAL(DIMENSION, img2->getNumberOfDimensions());
    CPPUNIT_ASSERT(img2->getType() == TYPE);
    CPPUNIT_ASSERT(img2->getSpacing() == VECTORSPACING);
    CPPUNIT_ASSERT(img2->getOrigin() == VECTORORIGIN);
    CPPUNIT_ASSERT(img2->getSize() == VECTORSIZE);
}

//------------------------------------------------------------------------------

void ImageTest::testAllocation()
{
    ::fwTools::Type TYPE                 = ::fwTools::Type::create("int16");
    ::fwData::Image::Size IMG_SIZE       = {14, 15, 26};
    ::fwData::Image::SizeType VECTORSIZE = {14, 15, 26};
    size_t SIZE = 14*15*26*TYPE.sizeOf();

    // process
    ::fwData::Image::sptr img1 = ::fwData::Image::New();

    img1->setType(TYPE);
    img1->setSize2(IMG_SIZE);

    img1->allocate();

    ::fwData::Array::sptr array = img1->getDataArray();
    CPPUNIT_ASSERT(array->getSize() == VECTORSIZE);
    CPPUNIT_ASSERT(array->getType() == TYPE);
    CPPUNIT_ASSERT_EQUAL(SIZE, array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(SIZE, img1->getSizeInBytes());

    ::fwData::Image::sptr img2 = ::fwData::Image::New();
    img2->allocate(VECTORSIZE[0], VECTORSIZE[1], VECTORSIZE[2], TYPE, ::fwData::Image::PixelFormat::GRAY_SCALE);

    array = img2->getDataArray();
    CPPUNIT_ASSERT(array->getSize() == VECTORSIZE);
    CPPUNIT_ASSERT(array->getType() == TYPE);
    CPPUNIT_ASSERT_EQUAL(SIZE, array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(SIZE, img2->getSizeInBytes());

    ::fwData::Image::sptr img3 = ::fwData::Image::New();
    img3->allocate(IMG_SIZE, TYPE, ::fwData::Image::PixelFormat::GRAY_SCALE);

    array = img3->getDataArray();
    CPPUNIT_ASSERT(array->getSize() == VECTORSIZE);
    CPPUNIT_ASSERT(array->getType() == TYPE);
    CPPUNIT_ASSERT_EQUAL(SIZE, array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(SIZE, img3->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(::fwData::Image::PixelFormat::GRAY_SCALE, img3->getPixelFormat());
}

//------------------------------------------------------------------------------

void ImageTest::testReallocation()
{
    const std::uint8_t DIMENSION = 3;
    ::fwTools::Type TYPE1 = ::fwTools::Type::create("int16");
    ::fwTools::Type TYPE2 = ::fwTools::Type::create("int64");
    ::fwTools::Type TYPE3 = ::fwTools::Type::create("uint8");
    ::fwData::Image::SizeType VECTORSIZE1(DIMENSION, 10);
    ::fwData::Image::SizeType VECTORSIZE2(DIMENSION, 20);
    ::fwData::Image::SizeType VECTORSIZE3(DIMENSION, 5);
    size_t SIZE1 = 10*10*10*TYPE1.sizeOf();
    size_t SIZE2 = 20*20*20*TYPE2.sizeOf();
    size_t SIZE3 = 5*5*5*TYPE3.sizeOf();

    // process
    ::fwData::Image::sptr img1 = ::fwData::Image::New();

    img1->allocate(VECTORSIZE1, TYPE1);
    ::fwData::Array::sptr array = img1->getDataArray();
    CPPUNIT_ASSERT(array->getSize() == VECTORSIZE1);
    CPPUNIT_ASSERT(array->getType() == TYPE1);
    CPPUNIT_ASSERT(array->getSizeInBytes() == SIZE1);
    CPPUNIT_ASSERT(img1->getSizeInBytes() == SIZE1);

    img1->allocate(VECTORSIZE2, TYPE2);
    CPPUNIT_ASSERT(array->getSize() == VECTORSIZE2);
    CPPUNIT_ASSERT(array->getType() == TYPE2);
    CPPUNIT_ASSERT(array->getSizeInBytes() == SIZE2);
    CPPUNIT_ASSERT(img1->getSizeInBytes() == SIZE2);

    img1->allocate(VECTORSIZE3, TYPE3);
    CPPUNIT_ASSERT(array->getSize() == VECTORSIZE3);
    CPPUNIT_ASSERT(array->getType() == TYPE3);
    CPPUNIT_ASSERT(array->getSizeInBytes() == SIZE3);
    CPPUNIT_ASSERT(img1->getSizeInBytes() == SIZE3);
}

//------------------------------------------------------------------------------

void ImageTest::testPixelType()
{
    ::fwData::Image::sptr img1 = ::fwData::Image::New();
    ::fwTools::DynamicType DT1;

    img1->setType(::fwTools::Type::create("int8"));
    DT1.setType< signed char >();
    CPPUNIT_ASSERT(DT1 == img1->getPixelType());

    img1->setType(::fwTools::Type::create("int16"));
    DT1.setType< signed short >();
    CPPUNIT_ASSERT(DT1 == img1->getPixelType());

    img1->setType(::fwTools::Type::create("int32"));
    DT1.setType< signed int >();
    CPPUNIT_ASSERT(DT1 == img1->getPixelType());

    img1->setType(::fwTools::Type::create("uint8"));
    DT1.setType< unsigned char >();
    CPPUNIT_ASSERT(DT1 == img1->getPixelType());

    img1->setType(::fwTools::Type::create("uint16"));
    DT1.setType< unsigned short >();
    CPPUNIT_ASSERT(DT1 == img1->getPixelType());

    img1->setType(::fwTools::Type::create("uint32"));
    DT1.setType< unsigned int >();
    CPPUNIT_ASSERT(DT1 == img1->getPixelType());

    img1->setType(::fwTools::Type::create("float"));
    DT1.setType< float >();
    CPPUNIT_ASSERT(DT1 == img1->getPixelType());

    img1->setType(::fwTools::Type::create("double"));
    DT1.setType< double >();
    CPPUNIT_ASSERT(DT1 == img1->getPixelType());
}

//------------------------------------------------------------------------------

void ImageTest::testSetGetPixel()
{
    ::fwData::Image::sptr img = ::fwData::Image::New();

    ::fwTools::Type TYPE       = ::fwTools::Type::create("int16");
    ::fwData::Image::Size SIZE = {10, 20, 30};

    const auto allocatedSize = img->allocate(SIZE, TYPE, ::fwData::Image::PixelFormat::GRAY_SCALE);

    CPPUNIT_ASSERT_EQUAL(SIZE[0]*SIZE[1]*SIZE[2]*2, allocatedSize);

    auto lock    = img->lock();
    auto iter    = img->begin<std::int16_t>();
    auto iterEnd = img->end<std::int16_t>();

    // test 1 : use getPixelBuffer
    std::int16_t count = 0;
    for (; iter != iterEnd; ++iter)
    {
        *iter = count++;
    }

    for (size_t x = 0; x < SIZE[0]; ++x)
    {
        for (size_t y = 0; y < SIZE[1]; ++y)
        {
            for (size_t z = 0; z < SIZE[2]; ++z)
            {
                const ::fwData::Image::IndexType index = x+y*SIZE[0]+z*SIZE[0]*SIZE[1];
                const std::int16_t val                 = static_cast<std::int16_t>(index);
                CPPUNIT_ASSERT_EQUAL(val, img->at<std::int16_t>(x, y, z));
                CPPUNIT_ASSERT_EQUAL(val, img->at<std::int16_t>(index));
                CPPUNIT_ASSERT_EQUAL(val, img->at<std::int16_t>(index));

                std::stringstream ss;
                ss << val;
                CPPUNIT_ASSERT_EQUAL( ss.str(), img->getPixelAsString(x, y, z));
            }
        }
    }

    // test 2 : use setPixelBuffer
    for (size_t x = 0; x < SIZE[0]; ++x)
    {
        for (size_t y = 0; y < SIZE[1]; ++y)
        {
            for (size_t z = 0; z < SIZE[2]; ++z)
            {
                const auto index = x+y*SIZE[0]+z*SIZE[0]*SIZE[1];
                std::int16_t val = static_cast<std::int16_t>(index * 2);
                img->at<std::int16_t>(index) = val;
            }
        }
    }

    count = 0;
    auto iter2 = img->begin<std::int16_t>();
    for (; iter2 != iterEnd; ++iter2)
    {
        CPPUNIT_ASSERT_EQUAL(static_cast<std::int16_t>(count++ *2), *iter2);
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwData
