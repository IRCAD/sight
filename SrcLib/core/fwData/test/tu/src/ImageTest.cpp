/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <fwMemory/stream/in/Raw.hpp>

#include <fwTest/generator/Image.hpp>

#include <fwTools/System.hpp>

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

static bool imagesEqual(const ::fwData::Image::sptr& _leftImg, const ::fwData::Image::sptr& _rightImg)
{
    bool result            = false;
    const bool equalFormat = _leftImg->getPixelFormat() == _rightImg->getPixelFormat() &&
                             _leftImg->getNumberOfComponents() == _rightImg->getNumberOfComponents();
    const bool equalDims = _leftImg->getSize2() == _rightImg->getSize2() &&
                           _leftImg->getSpacing2() == _rightImg->getSpacing2() &&
                           _leftImg->getOrigin2() == _rightImg->getOrigin2();

    if(equalFormat && equalDims)
    {
        // Compare images bytewise.
        const auto leftLock  = _leftImg->lock();
        const auto rightLock = _rightImg->lock();
        auto leftIt          = _leftImg->begin();
        auto rightIt         = _rightImg->begin();
        const auto leftEnd   = _leftImg->end();
        const auto rightEnd  = _rightImg->end();
        bool bytesEqual      = true;

        for(; leftIt != leftEnd && rightIt != rightEnd && bytesEqual; ++leftIt, ++rightIt)
        {
            bytesEqual = (*leftIt == *rightIt);
        }

        result = bytesEqual;
    }

    return result;
}

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
    auto iter          = img->begin<std::int16_t>();
    const auto iterEnd = img->end<std::int16_t>();

    // test 1 : get pixel value
    std::int16_t count = 0;
    for (; iter != iterEnd; ++iter)
    {
        *iter = count++;
    }

    ::fwData::Image::csptr constImg = img;
    for (size_t x = 0; x < SIZE[0]; ++x)
    {
        for (size_t y = 0; y < SIZE[1]; ++y)
        {
            for (size_t z = 0; z < SIZE[2]; ++z)
            {
                const ::fwData::Image::IndexType index = x+y*SIZE[0]+z*SIZE[0]*SIZE[1];
                const std::int16_t val                 = static_cast<std::int16_t>(index);
                CPPUNIT_ASSERT_EQUAL(val, img->at<std::int16_t>(x, y, z));
                CPPUNIT_ASSERT_EQUAL(val, constImg->at<std::int16_t>(x, y, z));
                CPPUNIT_ASSERT_EQUAL(val, img->at<std::int16_t>(index));
                CPPUNIT_ASSERT_EQUAL(val, constImg->at<std::int16_t>(index));
                CPPUNIT_ASSERT_EQUAL(val, *reinterpret_cast<std::int16_t*>(img->getPixelBuffer(index)));
                CPPUNIT_ASSERT_EQUAL(val, *reinterpret_cast<const std::int16_t*>(constImg->getPixelBuffer(index)));

                std::stringstream ss;
                ss << val;
                CPPUNIT_ASSERT_EQUAL( ss.str(), img->getPixelAsString(x, y, z));
            }
        }
    }

    // test 2 : set pixel value
    for (size_t x = 0; x < SIZE[0]; ++x)
    {
        for (size_t y = 0; y < SIZE[1]; ++y)
        {
            for (size_t z = 0; z < SIZE[2]; ++z)
            {
                const auto index = x+y*SIZE[0]+z*SIZE[0]*SIZE[1];

                if (index % 2 == 0)
                {
                    const std::int16_t val = static_cast<std::int16_t>(index * 2);
                    img->at<std::int16_t>(index) = val;
                }
                else
                {
                    std::int16_t val = static_cast<std::int16_t>(index * 2);
                    img->setPixelBuffer(index, reinterpret_cast< ::fwData::Image::BufferType* >(&val));
                }
            }
        }
    }

    count = 0;
    auto iter2      = img->begin<std::int16_t>();
    auto constIter2 = constImg->begin<std::int16_t>();
    for (; iter2 != iterEnd; ++iter2, ++constIter2)
    {
        CPPUNIT_ASSERT_EQUAL(static_cast<std::int16_t>(count *2), *iter2);
        CPPUNIT_ASSERT_EQUAL(static_cast<std::int16_t>(count++ *2), *constIter2);
    }

    ::fwData::Image::csptr img2 = ::fwData::Image::copy(img);

    const auto lock2 = img2->lock();
    {
        auto iterImg2          = img2->begin<std::int16_t>();
        auto iterImg1          = img->begin<std::int16_t>();
        const auto iterImg2End = img2->end<std::int16_t>();

        for (; iterImg2 != iterImg2End; ++iterImg2, ++iterImg1)
        {
            CPPUNIT_ASSERT_EQUAL(*iterImg1, *iterImg2);
        }
    }
    std::fill(img->begin(), img->end(), 0);

    for (const auto& element: *img)
    {
        CPPUNIT_ASSERT_EQUAL(static_cast<char>(0), element);
    }

    auto iter3 = img->begin<std::int16_t>();
    for (; iter3 != iterEnd; ++iter3)
    {
        CPPUNIT_ASSERT_EQUAL(static_cast<std::int16_t>(0), *iter3);
    }

    std::copy(img2->begin(), img2->end(), img->begin());

    {
        auto iterImg1          = img->begin<std::int16_t>();
        auto iterImg2          = img2->begin<std::int16_t>();
        const auto iterImg2End = img2->end<std::int16_t>();

        size_t i = 0;
        for (; iterImg2 != iterImg2End; ++iterImg2, ++iterImg1)
        {
            ++i;
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: "+ std::to_string(i), *iterImg2, *iterImg1);
        }
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

    struct RGBA16
    {
        std::uint16_t r;
        std::uint16_t g;
        std::uint16_t b;
        std::uint16_t a;
    };
    auto iter          = img->begin<RGBA16>();
    const auto iterEnd = img->end<RGBA16>();

    std::uint16_t count = 0;
    for (; iter != iterEnd; ++iter)
    {
        iter->r = count++;
        iter->g = count++;
        iter->b = count++;
        iter->a = count++;
    }

    count = 0;

    auto iter2          = img->begin< std::uint16_t >();
    const auto iterEnd2 = img->end< std::uint16_t >();
    for (; iter2 != iterEnd2; ++iter2)
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].v",
                                     static_cast<std::uint16_t>(count), *iter2);
        ++count;
    }

    count = 0;

    ::fwData::Image::ConstIterator<RGBA16> iter3          = img->begin<RGBA16>();
    const ::fwData::Image::ConstIterator<RGBA16> iter3End = img->end<RGBA16>();

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

void ImageTest::testIterator()
{
    ::fwData::Image::sptr img = ::fwData::Image::New();

    const ::fwTools::Type TYPE       = ::fwTools::Type::s_INT16;
    const ::fwData::Image::Size SIZE = {10, 20, 15};

    const auto allocatedSize = img->resize(SIZE, TYPE, ::fwData::Image::PixelFormat::GRAY_SCALE);

    CPPUNIT_ASSERT_EQUAL(SIZE[0]*SIZE[1]*SIZE[2]*2, allocatedSize);
    CPPUNIT_ASSERT_EQUAL(SIZE[0]*SIZE[1]*SIZE[2], img->getNumElements());

    const auto lock = img->lock();

    {
        // check default raw int8 iterator
        auto itr    = img->begin();
        auto itrEnd = img->end();

        CPPUNIT_ASSERT_EQUAL(static_cast< std::ptrdiff_t>(allocatedSize), itrEnd - itr);
    }

    {
        // check raw int16 iterator
        const auto itr    = img->begin<std::int16_t>();
        const auto itrEnd = img->end<std::int16_t>();

        CPPUNIT_ASSERT_EQUAL(static_cast< std::ptrdiff_t>(img->getNumElements()), itrEnd - itr);
    }

    const auto allocatedSize2 = img->resize(SIZE, TYPE, ::fwData::Image::PixelFormat::RGB);

    CPPUNIT_ASSERT_EQUAL(SIZE[0]*SIZE[1]*SIZE[2]*3*2, allocatedSize2);
    CPPUNIT_ASSERT_EQUAL(SIZE[0]*SIZE[1]*SIZE[2]*3, img->getNumElements());

    {
        // check raw int16 iterator
        const auto itr    = img->begin<std::int16_t>();
        const auto itrEnd = img->end<std::int16_t>();

        CPPUNIT_ASSERT_EQUAL(static_cast< std::ptrdiff_t>(img->getNumElements()), itrEnd - itr);
    }

    {
        // check RGB int8 iterator
        const auto itr    = img->begin< ::fwData::iterator::RGB >();
        const auto itrEnd = img->end< ::fwData::iterator::RGB >();

        CPPUNIT_ASSERT_EQUAL(static_cast< std::ptrdiff_t>(img->getNumElements()/3*2), itrEnd - itr);
    }

    {
        // check Raw int64 iterator
        const auto itr    = img->begin<std::int64_t>();
        const auto itrEnd = img->end<std::int64_t>();

        CPPUNIT_ASSERT_EQUAL(static_cast< std::ptrdiff_t>(img->getSizeInBytes()/8), itrEnd - itr);
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

    const ::fwData::iterator::RGB value = {18, 12, 68};
    std::fill(img->begin< ::fwData::iterator::RGB >(), img->end< ::fwData::iterator::RGB >(), value);

    std::uint8_t count = 0;

    auto iterRGB          = img->begin< ::fwData::iterator::RGB >();
    const auto iterEndRGB = img->end< ::fwData::iterator::RGB >();

    for (; iterRGB != iterEndRGB; ++iterRGB)
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].r", value.r, iterRGB->r);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].g", value.g, iterRGB->g);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].b", value.b, iterRGB->b);
        ++count;
    }

    count = 0;
    auto iterRGB2 = img->begin< ::fwData::iterator::RGB >();
    for (; iterRGB2 != iterEndRGB; ++iterRGB2)
    {
        iterRGB2->r = count++;
        iterRGB2->g = count++;
        iterRGB2->b = count++;
    }

    count = 0;

    ::fwData::Image::ConstIterator< ::fwData::iterator::RGB > iterRGB3 =
        img->begin< ::fwData::iterator::RGB >();
    const ::fwData::Image::ConstIterator< ::fwData::iterator::RGB > iterRGB3End = img->end< ::fwData::iterator::RGB >();
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

    auto iter          = img->begin< std::uint8_t >();
    const auto iterEnd = img->end< std::uint8_t >();

    std::uint8_t count = 0;
    for (; iter != iterEnd; ++iter)
    {
        *iter = count++;
    }

    count = 0;

    auto iter2          = img->begin<  ::fwData::iterator::BGR >();
    const auto iterEnd2 = img->end< ::fwData::iterator::BGR >();
    for (; iter2 != iterEnd2; ++iter2)
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].b",
                                     static_cast<std::uint8_t>(3*count), iter2->b);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].g",
                                     static_cast<std::uint8_t>(3*count+1), iter2->g);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].r",
                                     static_cast<std::uint8_t>(3*count+2), iter2->r);
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

    auto iter          = img->begin< std::uint8_t >();
    const auto iterEnd = img->end< std::uint8_t >();

    std::uint8_t count = 0;
    for (; iter != iterEnd; ++iter)
    {
        *iter = count++;
    }

    count = 0;

    auto iter2          = img->begin< ::fwData::iterator::BGRA >();
    const auto iterEnd2 = img->end< ::fwData::iterator::BGRA >();
    for (; iter2 != iterEnd2; ++iter2)
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].b",
                                     static_cast<std::uint8_t>(4*count), iter2->b);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].g",
                                     static_cast<std::uint8_t>(4*count+1), iter2->g);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].r",
                                     static_cast<std::uint8_t>(4*count+2), iter2->r);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("buff["+std::to_string(count) + "].a",
                                     static_cast<std::uint8_t>(4*count+3), iter2->a);
        ++count;
    }
}

//------------------------------------------------------------------------------

void ImageTest::imageDeepCopy()
{
    {
        const ::fwData::Image::sptr img = ::fwData::Image::New();
        const ::fwData::Image::Size size { 32, 32, 32 };
        const ::fwData::Image::Origin origin { 0.2, 123.4, 999.666 };
        const ::fwData::Image::Spacing spacing { 0.6, 0.6, 1.8};
        const auto type   = ::fwTools::Type::s_UINT8;
        const auto format = ::fwData::Image::PixelFormat::RGB;

        {
            const auto imgDumpLock = img->lock();
            ::fwTest::generator::Image::generateImage(img, size, spacing, origin, type, format);
            ::fwTest::generator::Image::randomizeImage(img);
        }

        const ::fwData::Image::sptr imgCopy = ::fwData::Image::New();

        // Lock the imgCopy buffer to make sure the underlying array isn't deleted.
        // Attempting to delete a locked array raises an assert in `fwMemory::BufferManager::unregisterBufferImpl()`.
        const auto imgCopyLock = imgCopy->lock();

        imgCopy->deepCopy(img);

        CPPUNIT_ASSERT_EQUAL(true, imagesEqual(img, imgCopy));
    }

    {
        const ::fwData::Image::sptr img = ::fwData::Image::New();
        const ::fwData::Image::Size size { 156, 126, 0 };
        const ::fwData::Image::Origin origin { 1., 1., 0. };
        const ::fwData::Image::Spacing spacing {10., 10., 0. };
        const auto type   = ::fwTools::Type::s_FLOAT;
        const auto format = ::fwData::Image::PixelFormat::GRAY_SCALE;

        {
            const auto imgDumpLock = img->lock();
            ::fwTest::generator::Image::generateImage(img, size, spacing, origin, type, format);
            ::fwTest::generator::Image::randomizeImage(img);
        }

        const ::fwData::Image::sptr imgCopy = ::fwData::Image::New();
        const auto imgCopyLock              = imgCopy->lock();

        imgCopy->deepCopy(img);

        CPPUNIT_ASSERT_EQUAL(true, imagesEqual(img, imgCopy));
    }
}

//------------------------------------------------------------------------------

void ImageTest::setISStreamTest()
{
    ::fwData::Image::sptr image = ::fwData::Image::New();
    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::s_INT16);

    const auto dumpLock              = image->lock();
    const std::filesystem::path PATH = ::fwTools::System::getTemporaryFolder() / "ImageTest.raw";

    std::ofstream ostr(PATH, std::ios::binary);
    ostr.write(static_cast<const char*>(image->getBuffer()), image->getSizeInBytes());
    ostr.close();

    ::fwData::Image::sptr newImage = ::fwData::Image::New();
    newImage->setSize2(image->getSize2());
    newImage->setType(image->getType());
    newImage->setPixelFormat(image->getPixelFormat());
    newImage->setIStreamFactory(std::make_shared< ::fwMemory::stream::in::Raw >(PATH),
                                image->getSizeInBytes(), PATH, ::fwMemory::RAW);

    const auto newDumpLock = newImage->lock();

    auto itr       = image->begin< std::int16_t>();
    const auto end = image->end< std::int16_t>();
    auto newItr    = newImage->begin< std::int16_t>();

    for (; itr != end; ++itr, ++newItr)
    {
        CPPUNIT_ASSERT_EQUAL(*itr, *newItr);
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwData
