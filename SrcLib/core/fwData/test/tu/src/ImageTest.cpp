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

    // old API begin --------------------------
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
    // old API end --------------------------
}

//------------------------------------------------------------------------------

void ImageTest::testAllocation()
{
    const ::fwTools::Type TYPE                 = ::fwTools::Type::create("int16");
    const ::fwData::Image::Size IMG_SIZE       = {14, 15, 26};
    const ::fwData::Image::SizeType VECTORSIZE = {14, 15, 26};
    const size_t NB_ELTS                       = 14*15*26;
    const size_t SIZE                          = NB_ELTS*TYPE.sizeOf();

    // process
    ::fwData::Image::sptr img1 = ::fwData::Image::New();

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), img1->getNumElements());

    img1->setType(TYPE);
    img1->setSize2(IMG_SIZE);

    img1->resize();
    CPPUNIT_ASSERT_EQUAL(SIZE, img1->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(NB_ELTS, img1->getNumElements());
    CPPUNIT_ASSERT(img1->getType() == TYPE);

    // old API begin --------------------------
    ::fwData::Array::sptr array = img1->getDataArray();
    CPPUNIT_ASSERT(array->getSize() == VECTORSIZE);
    CPPUNIT_ASSERT(array->getType() == TYPE);
    CPPUNIT_ASSERT_EQUAL(SIZE, array->getSizeInBytes());
    // old API end --------------------------

    ::fwData::Image::sptr img2 = ::fwData::Image::New();
    img2->resize(VECTORSIZE[0], VECTORSIZE[1], VECTORSIZE[2], TYPE, ::fwData::Image::PixelFormat::GRAY_SCALE);

    CPPUNIT_ASSERT_EQUAL(SIZE, img2->getSizeInBytes());
    CPPUNIT_ASSERT(img1->getType() == TYPE);

    // old API begin --------------------------
    array = img2->getDataArray();
    CPPUNIT_ASSERT(array->getSize() == VECTORSIZE);
    CPPUNIT_ASSERT(array->getType() == TYPE);
    CPPUNIT_ASSERT_EQUAL(SIZE, array->getSizeInBytes());
    // old API end --------------------------

    ::fwData::Image::sptr img3 = ::fwData::Image::New();
    img3->resize(IMG_SIZE, TYPE, ::fwData::Image::PixelFormat::GRAY_SCALE);
    CPPUNIT_ASSERT_EQUAL(SIZE, img3->getSizeInBytes());
    CPPUNIT_ASSERT(img1->getType() == TYPE);
    CPPUNIT_ASSERT_EQUAL(::fwData::Image::PixelFormat::GRAY_SCALE, img3->getPixelFormat());

    // old API begin --------------------------
    array = img3->getDataArray();
    CPPUNIT_ASSERT(array->getSize() == VECTORSIZE);
    CPPUNIT_ASSERT(array->getType() == TYPE);
    CPPUNIT_ASSERT_EQUAL(SIZE, array->getSizeInBytes());
    // old API end --------------------------
}

//------------------------------------------------------------------------------

void ImageTest::testReallocation()
{
    const std::uint8_t DIMENSION          = 3;
    const ::fwTools::Type TYPE1           = ::fwTools::Type::create("int16");
    const ::fwTools::Type TYPE2           = ::fwTools::Type::create("int64");
    const ::fwTools::Type TYPE3           = ::fwTools::Type::create("uint8");
    const ::fwData::Image::Size IMG_SIZE1 = {10, 10, 10};
    const ::fwData::Image::Size IMG_SIZE2 = {20, 20, 20};
    const ::fwData::Image::Size IMG_SIZE3 = {5, 5, 5};
    const ::fwData::Image::Size IMG_SIZE5 = {0, 0, 0};
    const ::fwData::Image::Size IMG_SIZE6 = {42, 20};
    const ::fwData::Image::SizeType VECTORSIZE1(DIMENSION, 10);
    const ::fwData::Image::SizeType VECTORSIZE2(DIMENSION, 20);
    const ::fwData::Image::SizeType VECTORSIZE3(DIMENSION, 5);
    const size_t SIZE1 = 10*10*10*TYPE1.sizeOf();
    const size_t SIZE2 = 20*20*20*TYPE2.sizeOf();
    const size_t SIZE3 = 5*5*5*TYPE3.sizeOf();
    const size_t SIZE4 = 5*5*5*4*TYPE3.sizeOf();
    const size_t SIZE5 = 0;
    const size_t SIZE6 = 42*20;
    const size_t SIZE7 = 42*20*3;

    // process
    ::fwData::Image::sptr img1  = ::fwData::Image::New();
    ::fwData::Array::sptr array = img1->getDataArray();

    const size_t resized1 = img1->resize(IMG_SIZE1, TYPE1, ::fwData::Image::PixelFormat::GRAY_SCALE);
    CPPUNIT_ASSERT_EQUAL(resized1, img1->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(SIZE1, img1->getSizeInBytes());

    // old API begin --------------------------
    CPPUNIT_ASSERT(array->getSize() == VECTORSIZE1);
    CPPUNIT_ASSERT_EQUAL(TYPE1, array->getType());
    CPPUNIT_ASSERT_EQUAL(SIZE1, array->getSizeInBytes());
    // old API end --------------------------

    const size_t resized2 = img1->resize(IMG_SIZE2, TYPE2, ::fwData::Image::PixelFormat::GRAY_SCALE);
    CPPUNIT_ASSERT_EQUAL(resized2, img1->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(SIZE2, img1->getSizeInBytes());

    // old API begin --------------------------
    CPPUNIT_ASSERT(array->getSize() == VECTORSIZE2);
    CPPUNIT_ASSERT_EQUAL(TYPE2, array->getType());
    CPPUNIT_ASSERT_EQUAL(SIZE2, array->getSizeInBytes());
    // old API end --------------------------

    const size_t resized3 = img1->resize(IMG_SIZE3, TYPE3, ::fwData::Image::PixelFormat::GRAY_SCALE);
    CPPUNIT_ASSERT_EQUAL(resized3, img1->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(SIZE3, img1->getSizeInBytes());

    // old API begin --------------------------
    CPPUNIT_ASSERT(array->getSize() == VECTORSIZE3);
    CPPUNIT_ASSERT_EQUAL(TYPE3, array->getType());
    CPPUNIT_ASSERT_EQUAL(SIZE3, array->getSizeInBytes());
    // old API end --------------------------

    const size_t resized4 = img1->resize(IMG_SIZE3, TYPE3, ::fwData::Image::PixelFormat::RGBA);
    CPPUNIT_ASSERT_EQUAL(resized4, img1->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(SIZE4, img1->getSizeInBytes());

    const size_t resized5 = img1->resize(IMG_SIZE5, TYPE3, ::fwData::Image::PixelFormat::GRAY_SCALE);
    CPPUNIT_ASSERT_EQUAL(resized5, img1->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(SIZE5, img1->getSizeInBytes());

    const size_t resized6 = img1->resize(IMG_SIZE6, TYPE3, ::fwData::Image::PixelFormat::GRAY_SCALE);
    CPPUNIT_ASSERT_EQUAL(resized6, img1->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(SIZE6, img1->getSizeInBytes());

    const size_t resized7 = img1->resize(IMG_SIZE6, TYPE3, ::fwData::Image::PixelFormat::RGB);
    CPPUNIT_ASSERT_EQUAL(resized7, img1->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(SIZE7, img1->getSizeInBytes());
}

//------------------------------------------------------------------------------

void ImageTest::testPixelType()
{
    // old API begin --------------------------
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
    // old API end --------------------------
}

//------------------------------------------------------------------------------

void ImageTest::testSetGetPixel()
{
    ::fwData::Image::sptr img = ::fwData::Image::New();

    const ::fwTools::Type TYPE       = ::fwTools::Type::s_INT16;
    const ::fwData::Image::Size SIZE = {10, 20, 30};

    const auto allocatedSize = img->resize(SIZE, TYPE, ::fwData::Image::PixelFormat::GRAY_SCALE);

    CPPUNIT_ASSERT_EQUAL(SIZE[0]*SIZE[1]*SIZE[2]*2, allocatedSize);

    const auto lock    = img->lock();
    auto iter          = img->begin<iterator::IterationBase<std::int16_t>::Raw>();
    const auto iterEnd = img->end<iterator::IterationBase<std::int16_t>::Raw>();

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
                const auto index       = x+y*SIZE[0]+z*SIZE[0]*SIZE[1];
                const std::int16_t val = static_cast<std::int16_t>(index * 2);
                img->at<std::int16_t>(index) = val;
            }
        }
    }

    count = 0;
    auto iter2 = img->begin<iterator::IterationBase<std::int16_t>::Raw>();
    for (; iter2 != iterEnd; ++iter2)
    {
        CPPUNIT_ASSERT_EQUAL(static_cast<std::int16_t>(count++ *2), iter2->value);
    }

    std::fill(img->begin(), img->end(), 0);

    for (const auto& element: *img)
    {
        CPPUNIT_ASSERT_EQUAL(static_cast<char>(0), element.value);
    }
}

//------------------------------------------------------------------------------

void ImageTest::testRGBAIterator()
{
    ::fwData::Image::sptr img = ::fwData::Image::New();

    const ::fwTools::Type TYPE       = ::fwTools::Type::s_UINT16;
    const ::fwData::Image::Size SIZE = {10, 20, 15};

    const auto allocatedSize = img->resize(SIZE, TYPE, ::fwData::Image::PixelFormat::RGBA);

    CPPUNIT_ASSERT_EQUAL(SIZE[0]*SIZE[1]*SIZE[2]*4*2, allocatedSize);

    CPPUNIT_ASSERT_EQUAL(SIZE[0]*SIZE[1]*SIZE[2]*4, img->getNumElements());

    const auto lock = img->lock();

    typedef iterator::IterationBase<std::uint16_t>::RGBA RGBAIterator;
    auto iter          = img->begin<RGBAIterator>();
    const auto iterEnd = img->end<RGBAIterator>();

    std::uint16_t count = 0;
    for (; iter != iterEnd; ++iter)
    {
        iter->r = count++;
        iter->g = count++;
        iter->b = count++;
        iter->a = count++;
    }

    count = 0;

    auto iter2          = img->begin< ::fwData::Image::Iteration<std::uint16_t> >();
    const auto iterEnd2 = img->end< ::fwData::Image::Iteration<std::uint16_t> >();
    for (; iter2 != iterEnd2; ++iter2)
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].v",
                                     static_cast<std::uint16_t>(count), iter2->value);
        ++count;
    }

    count = 0;

    ::fwData::Image::ConstIterator<RGBAIterator> iter3          = img->begin<RGBAIterator>();
    const ::fwData::Image::ConstIterator<RGBAIterator> iter3End = img->end<RGBAIterator>();

    for (; iter3 != iter3End; ++iter3)
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].r",
                                     static_cast<std::uint16_t>(4*count), iter3->r);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].g",
                                     static_cast<std::uint16_t>(4*count+1), iter3->g);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].b",
                                     static_cast<std::uint16_t>(4*count+2), iter3->b);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].a",
                                     static_cast<std::uint16_t>(4*count+3), iter3->a);
        ++count;
    }
}

//------------------------------------------------------------------------------

void ImageTest::testRGBIterator()
{
    ::fwData::Image::sptr img = ::fwData::Image::New();

    const ::fwTools::Type TYPE       = ::fwTools::Type::s_UINT8;
    const ::fwData::Image::Size SIZE = {10, 20, 15};

    const auto allocatedSize = img->resize(SIZE, TYPE, ::fwData::Image::PixelFormat::RGB);

    CPPUNIT_ASSERT_EQUAL(SIZE[0]*SIZE[1]*SIZE[2]*3, allocatedSize);

    CPPUNIT_ASSERT_EQUAL(SIZE[0]*SIZE[1]*SIZE[2]*3, img->getNumElements());

    const auto lock = img->lock();

    typedef iterator::IterationBase<std::uint8_t>::RGB RGBIterator;
    const std::array< std::uint8_t, 3> value = {18, 12, 68};
    std::fill(img->begin<RGBIterator>(), img->end<RGBIterator>(), value);

    std::uint8_t count = 0;

    auto iterRGB          = img->begin<RGBIterator>();
    const auto iterEndRGB = img->end<RGBIterator>();

    for (; iterRGB != iterEndRGB; ++iterRGB)
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].r", value[0], iterRGB->r);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].g", value[1], iterRGB->g);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].b", value[2], iterRGB->b);
        ++count;
    }

    count = 0;
    auto iterRGB2 = img->begin<RGBIterator>();
    for (; iterRGB2 != iterEndRGB; ++iterRGB2)
    {
        iterRGB2->r = count++;
        iterRGB2->g = count++;
        iterRGB2->b = count++;
    }

    count = 0;

    ::fwData::Image::ConstIterator<RGBIterator> iterRGB3          = img->begin<RGBIterator>();
    const ::fwData::Image::ConstIterator<RGBIterator> iterRGB3End = img->end<RGBIterator>();
    for (; iterRGB3 != iterRGB3End; ++iterRGB3)
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].r",
                                     static_cast<std::uint8_t>(3*count), iterRGB3->r);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].g",
                                     static_cast<std::uint8_t>(3*count+1), iterRGB3->g);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].b",
                                     static_cast<std::uint8_t>(3*count+2), iterRGB3->b);
        ++count;
    }
}

//------------------------------------------------------------------------------

void ImageTest::testBGRIterator()
{
    ::fwData::Image::sptr img = ::fwData::Image::New();

    const ::fwTools::Type TYPE       = ::fwTools::Type::s_UINT8;
    const ::fwData::Image::Size SIZE = {10, 20};

    const auto allocatedSize = img->resize(SIZE, TYPE, ::fwData::Image::PixelFormat::BGR);

    CPPUNIT_ASSERT_EQUAL(SIZE[0]*SIZE[1]*3, allocatedSize);

    CPPUNIT_ASSERT_EQUAL(SIZE[0]*SIZE[1]*3, img->getNumElements());

    const auto lock = img->lock();

    auto iter          = img->begin< ::fwData::Image::Iteration<std::uint8_t> >();
    const auto iterEnd = img->end< ::fwData::Image::Iteration<std::uint8_t> >();

    std::uint8_t count = 0;
    for (; iter != iterEnd; ++iter)
    {
        iter->value = count++;
    }

    typedef ::fwData::Image::BGRIteration BGRIteration;
    count = 0;

    auto iter2          = img->begin< BGRIteration >();
    const auto iterEnd2 = img->end< BGRIteration >();
    for (; iter2 != iterEnd2; ++iter2)
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].b",
                                     static_cast<BGRIteration::type>(3*count), iter2->b);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].g",
                                     static_cast<BGRIteration::type>(3*count+1), iter2->g);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].r",
                                     static_cast<BGRIteration::type>(3*count+2), iter2->r);
        ++count;
    }
}

//------------------------------------------------------------------------------

void ImageTest::testBGRAIterator()
{
    ::fwData::Image::sptr img = ::fwData::Image::New();

    const ::fwTools::Type TYPE       = ::fwTools::Type::s_UINT8;
    const ::fwData::Image::Size SIZE = {10, 20};

    const auto allocatedSize = img->resize(SIZE, TYPE, ::fwData::Image::PixelFormat::BGRA);

    CPPUNIT_ASSERT_EQUAL(SIZE[0]*SIZE[1]*4, allocatedSize);

    CPPUNIT_ASSERT_EQUAL(SIZE[0]*SIZE[1]*4, img->getNumElements());

    const auto lock = img->lock();

    auto iter          = img->begin< ::fwData::Image::Iteration<std::uint8_t> >();
    const auto iterEnd = img->end< ::fwData::Image::Iteration<std::uint8_t> >();

    std::uint8_t count = 0;
    for (; iter != iterEnd; ++iter)
    {
        *iter = count++;
    }

    typedef ::fwData::Image::BGRAIteration BGRAIteration;
    count = 0;

    auto iter2          = img->begin< BGRAIteration >();
    const auto iterEnd2 = img->end< BGRAIteration >();
    for (; iter2 != iterEnd2; ++iter2)
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].b",
                                     static_cast<BGRAIteration::type>(4*count), iter2->b);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].g",
                                     static_cast<BGRAIteration::type>(4*count+1), iter2->g);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].r",
                                     static_cast<BGRAIteration::type>(4*count+2), iter2->r);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].a",
                                     static_cast<BGRAIteration::type>(4*count+3), iter2->a);
        ++count;
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwData
