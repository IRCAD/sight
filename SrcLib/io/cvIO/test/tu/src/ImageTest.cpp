/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ImageTest.hpp"

#include "cvCommon.hpp"

#include <cvIO/Image.hpp>

#include <fwData/Array.hpp>

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/Image.hpp>
#include <fwDataTools/helper/ImageGetter.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::cvIO::ut::ImageTest );

namespace cvIO
{
namespace ut
{

//------------------------------------------------------------------------------

template <typename T>
static ::fwData::Image::sptr genImage(const std::vector<T>& _imageBuffer, size_t _w, size_t _h, size_t _d,
                                      std::uint8_t _numChannels)
{
    ::fwData::Image::sptr image = ::fwData::Image::New();
    ::fwDataTools::helper::Image imgHelper(image);

    SLM_ASSERT("Width should be at least 1", _w >= 1);
    size_t imageDim = 1;
    if(_h > 0)
    {
        imageDim++;
    }
    if(_d > 0)
    {
        imageDim++;
    }

    const ::fwTools::Type imageType = ::fwTools::Type::create<T>();
    ::fwData::Image::SizeType imageSize(imageDim);
    imageSize[0] = _w;
    if(_h > 0)
    {
        imageSize[1] = _h;
    }
    if(_d > 0)
    {
        imageSize[2] = _d;
    }

    image->allocate(imageSize, imageType, _numChannels);

    ::fwData::Array::sptr array = image->getDataArray();
    ::fwDataTools::helper::Array arrayHelper(array);

    std::uint8_t* dstBuffer       = arrayHelper.begin< std::uint8_t >();
    const std::uint8_t* srcBuffer = reinterpret_cast<const std::uint8_t*>(_imageBuffer.data());
    std::copy( srcBuffer, srcBuffer+array->getSizeInBytes(), dstBuffer);

    return image;
}

//------------------------------------------------------------------------------

template <typename T>
static void compareImages( const ::cv::Mat& _cvImage,
                           const ::fwData::Image::csptr& _image,
                           size_t _w, size_t _h, size_t _d,
                           std::uint8_t _numChannels)
{
    ::fwDataTools::helper::ImageGetter helper(_image);
    const T* imageBuffer = reinterpret_cast<const T*>(helper.getBuffer());

    std::vector< ::cv::Mat> channels(_numChannels);
    ::cv::split(_cvImage, channels);

    if(_d > 0)
    {
        CPPUNIT_ASSERT_EQUAL(_image->getSize().size(), static_cast<size_t>(_cvImage.dims));
        CPPUNIT_ASSERT_EQUAL(_w, static_cast<size_t>(_cvImage.size[2]));
        CPPUNIT_ASSERT_EQUAL(_h, static_cast<size_t>(_cvImage.size[1]));
        CPPUNIT_ASSERT_EQUAL(_d, static_cast<size_t>(_cvImage.size[0]));
        for(int k = 0; k < _cvImage.size[0]; ++k)
        {
            for(int j = 0; j < _cvImage.size[1]; ++j)
            {
                for(int i = 0; i < _cvImage.size[2]; ++i)
                {
                    for(std::uint8_t c = 0; c < _numChannels; ++c)
                    {
                        const size_t index = static_cast<size_t>(c +
                                                                 static_cast<size_t>(i) * _numChannels +
                                                                 static_cast<size_t>(j) * _numChannels * _w +
                                                                 static_cast<size_t>(k) *  _numChannels * _w * _h);
                        CPPUNIT_ASSERT_EQUAL(imageBuffer[index], channels[c].at<T>(k, j, i));
                    }
                }
            }
        }
    }
    else if(_h > 0)
    {
        CPPUNIT_ASSERT_EQUAL(_image->getSize().size(), static_cast<size_t>(_cvImage.dims));
        CPPUNIT_ASSERT_EQUAL(_w, static_cast<size_t>(_cvImage.size[1]));
        CPPUNIT_ASSERT_EQUAL(_h, static_cast<size_t>(_cvImage.size[0]));

        for(int j = 0; j < _cvImage.size[0]; ++j)
        {
            for(int i = 0; i < _cvImage.size[1]; ++i)
            {
                for(std::uint8_t c = 0; c < _numChannels; ++c)
                {
                    const size_t index = c +
                                         static_cast<size_t>(i) * _numChannels +
                                         static_cast<size_t>(j) * _numChannels * _w;
                    CPPUNIT_ASSERT_EQUAL(imageBuffer[index], channels[c].at<T>(j, i));
                }
            }
        }
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(2, _cvImage.dims);
        CPPUNIT_ASSERT_EQUAL(_w, static_cast<size_t>(_cvImage.size[1]));
        CPPUNIT_ASSERT_EQUAL(1, _cvImage.size[0]);

        for(int i = 0; i < _cvImage.size[1]; ++i)
        {
            for(std::uint8_t c = 0; c < _numChannels; ++c)
            {
                const size_t index = static_cast<size_t>(c + i * _numChannels);
                CPPUNIT_ASSERT_EQUAL(imageBuffer[index], channels[c].at<T>(i));
            }
        }
    }
}

//------------------------------------------------------------------------------

template <typename T>
static void testMoveToCV(size_t _w, size_t _h, size_t _d, std::uint8_t _numChannels)
{
    const std::vector<T> imageBuffer = genImageBuffer<T>(_w, _h, _d, _numChannels);
    ::fwData::Image::sptr image = genImage<T>(imageBuffer, _w, _h, _d, _numChannels);

    ::cv::Mat cvImage = ::cvIO::Image::moveToCv(image);

    // Since we share the same buffer, compare the pointers
    ::fwDataTools::helper::ImageGetter helper(image);
    CPPUNIT_ASSERT_EQUAL(helper.getBuffer(), static_cast<void*>(cvImage.data));

    compareImages<T>(cvImage, image, _w, _h, _d, _numChannels);
}

//------------------------------------------------------------------------------

template <typename T>
static void testCopyFromCV(size_t _w, size_t _h, size_t _d, std::uint8_t _numChannels)
{
    const std::vector<T> imageBuffer = genImageBuffer<T>(_w, _h, _d, _numChannels);
    const ::cv::Mat cvImage          = genCvImage<T>(imageBuffer, _w, _h, _d, _numChannels);

    ::fwData::Image::sptr image = ::fwData::Image::New();
    ::cvIO::Image::copyFromCv(image, cvImage);

    // Since we copy the buffer, ensure the pointers are different
    ::fwDataTools::helper::ImageGetter helper(image);
    CPPUNIT_ASSERT(helper.getBuffer() != static_cast<void*>(cvImage.data));

    compareImages<T>(cvImage, image, _w, _h, _d, _numChannels);
}

//------------------------------------------------------------------------------

template <typename T>
static void testCopyToCV(size_t _w, size_t _h, size_t _d, std::uint8_t _numChannels)
{
    const std::vector<T> imageBuffer = genImageBuffer<T>(_w, _h, _d, _numChannels);
    ::fwData::Image::csptr image = genImage<T>(imageBuffer, _w, _h, _d, _numChannels);

    ::cv::Mat cvImage = ::cvIO::Image::copyToCv(image);

    // Since we copy the buffer, ensure the pointers are different
    ::fwDataTools::helper::ImageGetter helper(image);
    CPPUNIT_ASSERT(helper.getBuffer() != static_cast<void*>(cvImage.data));

    compareImages<T>(cvImage, image, _w, _h, _d, _numChannels);
}

//------------------------------------------------------------------------------

void ImageTest::setUp()
{
    // Set up context before running a test.
    std::srand(101101);
}

//------------------------------------------------------------------------------

void ImageTest::tearDown()
{
}

//------------------------------------------------------------------------------

void ImageTest::moveToCv()
{
    testMoveToCV<std::uint8_t>(10, 2, 30, 1);
    testMoveToCV<std::uint8_t>(1, 20, 5, 2);
    testMoveToCV<std::uint8_t>(6, 12, 0, 3);
    testMoveToCV<std::uint8_t>(10, 7, 30, 4);

    testMoveToCV<std::int8_t>(10, 0, 0, 1);
    testMoveToCV<std::int8_t>(10, 20, 0, 2);
    testMoveToCV<std::int8_t>(23, 20, 30, 3);
    testMoveToCV<std::int8_t>(76, 2, 0, 4);

    testMoveToCV<std::uint16_t>(10, 2, 30, 1);
    testMoveToCV<std::uint16_t>(10, 20, 5, 2);
    testMoveToCV<std::uint16_t>(6, 20, 0, 3);
    testMoveToCV<std::uint16_t>(10, 7, 30, 4);

    testMoveToCV<std::int16_t>(10, 0, 0, 1);
    testMoveToCV<std::int16_t>(10, 20, 0, 2);
    testMoveToCV<std::int16_t>(23, 20, 30, 3);
    testMoveToCV<std::int16_t>(76, 2, 1, 4);

    testMoveToCV<std::int32_t>(10, 32, 0, 1);
    testMoveToCV<std::int32_t>(10, 20, 3, 2);
    testMoveToCV<std::int32_t>(23, 20, 0, 3);
    testMoveToCV<std::int32_t>(76, 2, 1, 4);

    testMoveToCV<float>(10, 32, 0, 1);
    testMoveToCV<float>(10, 20, 3, 2);
    testMoveToCV<float>(90, 20, 1, 3);
    testMoveToCV<float>(76, 2, 4, 4);

    testMoveToCV<double>(12, 32, 0, 1);
    testMoveToCV<double>(10, 1, 3, 2);
    testMoveToCV<double>(23, 8, 0, 3);
    testMoveToCV<double>(76, 2, 4, 4);
}

//------------------------------------------------------------------------------

void ImageTest::copyFromCv()
{
    testCopyFromCV<std::int16_t>(10, 0, 0, 1);
    testCopyFromCV<std::uint8_t>(10, 2, 30, 1);
    testCopyFromCV<std::uint8_t>(1, 20, 5, 2);
    testCopyFromCV<std::uint8_t>(6, 12, 0, 3);
    testCopyFromCV<std::uint8_t>(10, 7, 30, 4);

    testCopyFromCV<std::int8_t>(10, 0, 0, 1);
    testCopyFromCV<std::int8_t>(10, 20, 0, 2);
    testCopyFromCV<std::int8_t>(23, 20, 30, 3);
    testCopyFromCV<std::int8_t>(76, 2, 0, 4);

    testCopyFromCV<std::uint16_t>(10, 1, 1, 1);
    testCopyFromCV<std::uint16_t>(10, 2, 30, 1);
    testCopyFromCV<std::uint16_t>(10, 20, 5, 2);
    testCopyFromCV<std::uint16_t>(6, 20, 0, 3);
    testCopyFromCV<std::uint16_t>(10, 7, 30, 4);

    testCopyFromCV<std::int16_t>(10, 0, 0, 1);
    testCopyFromCV<std::int16_t>(10, 20, 0, 2);
    testCopyFromCV<std::int16_t>(23, 20, 30, 3);
    testCopyFromCV<std::int16_t>(76, 2, 0, 4);

    testCopyFromCV<std::int32_t>(10, 32, 0, 1);
    testCopyFromCV<std::int32_t>(10, 20, 3, 2);
    testCopyFromCV<std::int32_t>(23, 20, 0, 3);
    testCopyFromCV<std::int32_t>(76, 2, 4, 4);

    testCopyFromCV<float>(10, 32, 0, 1);
    testCopyFromCV<float>(10, 20, 3, 2);
    testCopyFromCV<float>(90, 20, 1, 3);
    testCopyFromCV<float>(76, 2, 4, 4);

    testCopyFromCV<double>(12, 32, 0, 1);
    testCopyFromCV<double>(10, 1, 3, 2);
    testCopyFromCV<double>(23, 8, 0, 3);
    testCopyFromCV<double>(76, 2, 4, 4);
}

//------------------------------------------------------------------------------

void ImageTest::copyToCv()
{
    testCopyToCV<std::uint8_t>(10, 2, 30, 1);
    testCopyToCV<std::uint8_t>(1, 20, 5, 2);
    testCopyToCV<std::uint8_t>(6, 12, 0, 3);
    testCopyToCV<std::uint8_t>(10, 7, 30, 4);

    testCopyToCV<std::int8_t>(10, 0, 0, 1);
    testCopyToCV<std::int8_t>(10, 20, 0, 2);
    testCopyToCV<std::int8_t>(23, 20, 30, 3);
    testCopyToCV<std::int8_t>(76, 2, 0, 4);

    testCopyToCV<std::uint16_t>(10, 2, 30, 1);
    testCopyToCV<std::uint16_t>(10, 20, 5, 2);
    testCopyToCV<std::uint16_t>(6, 20, 0, 3);
    testCopyToCV<std::uint16_t>(10, 7, 30, 4);

    testCopyToCV<std::int16_t>(10, 0, 0, 1);
    testCopyToCV<std::int16_t>(10, 20, 0, 2);
    testCopyToCV<std::int16_t>(23, 20, 30, 3);
    testCopyToCV<std::int16_t>(76, 2, 0, 4);

    testCopyToCV<std::int32_t>(10, 32, 0, 1);
    testCopyToCV<std::int32_t>(10, 20, 3, 2);
    testCopyToCV<std::int32_t>(23, 20, 0, 3);
    testCopyToCV<std::int32_t>(76, 2, 4, 4);

    testCopyToCV<float>(10, 32, 0, 1);
    testCopyToCV<float>(10, 20, 3, 2);
    testCopyToCV<float>(90, 20, 1, 3);
    testCopyToCV<float>(76, 2, 4, 4);

    testCopyToCV<double>(12, 32, 0, 1);
    testCopyToCV<double>(10, 1, 3, 2);
    testCopyToCV<double>(23, 8, 0, 3);
    testCopyToCV<double>(76, 2, 4, 4);
}

//------------------------------------------------------------------------------

}// namespace ut

}// namespace cvIO
