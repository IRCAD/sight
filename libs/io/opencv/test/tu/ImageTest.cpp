/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "cvCommon.hpp"

#include <data/Array.hpp>

#include <io/opencv/Image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(::sight::io::opencv::ut::ImageTest);

namespace sight::io::opencv
{

namespace ut
{

//------------------------------------------------------------------------------

template<typename T>
static data::Image::sptr genImage(
    const std::vector<T>& _imageBuffer,
    size_t _w,
    size_t _h,
    size_t _d,
    std::uint8_t _numChannels
)
{
    data::Image::sptr image = data::Image::New();
    const auto dumpLock     = image->lock();

    SIGHT_ASSERT("Width should be at least 1", _w >= 1);
    size_t imageDim = 1;
    if(_h > 0)
    {
        imageDim++;
    }

    if(_d > 0)
    {
        imageDim++;
    }

    const core::tools::Type imageType = core::tools::Type::create<T>();
    data::Image::Size imageSize       = {0, 0, 0};
    imageSize[0] = _w;
    if(_h > 0)
    {
        imageSize[1] = _h;
    }

    if(_d > 0)
    {
        imageSize[2] = _d;
    }

    image->setSize2(imageSize);
    image->setType(imageType);
    image->setNumberOfComponents(_numChannels);
    image->resize();

    auto dstBuffer                = image->begin<std::uint8_t>();
    const std::uint8_t* srcBuffer = reinterpret_cast<const std::uint8_t*>(_imageBuffer.data());
    std::copy(srcBuffer, srcBuffer + image->getSizeInBytes(), dstBuffer);

    return image;
}

//------------------------------------------------------------------------------

template<typename T>
static void compareImages(
    const ::cv::Mat& _cvImage,
    const data::Image::csptr& _image,
    size_t _w,
    size_t _h,
    size_t _d,
    std::uint8_t _numChannels
)
{
    const auto dumpLock  = _image->lock();
    const T* imageBuffer = reinterpret_cast<const T*>(_image->getBuffer());

    std::vector< ::cv::Mat> channels(_numChannels);
    ::cv::split(_cvImage, channels);

    if(_d > 0)
    {
        CPPUNIT_ASSERT_EQUAL(_image->getNumberOfDimensions(), static_cast<size_t>(_cvImage.dims));
        CPPUNIT_ASSERT_EQUAL(_w, static_cast<size_t>(_cvImage.size[2]));
        CPPUNIT_ASSERT_EQUAL(_h, static_cast<size_t>(_cvImage.size[1]));
        CPPUNIT_ASSERT_EQUAL(_d, static_cast<size_t>(_cvImage.size[0]));
        for(int k = 0 ; k < _cvImage.size[0] ; ++k)
        {
            for(int j = 0 ; j < _cvImage.size[1] ; ++j)
            {
                for(int i = 0 ; i < _cvImage.size[2] ; ++i)
                {
                    for(std::uint8_t c = 0 ; c < _numChannels ; ++c)
                    {
                        const size_t index = static_cast<size_t>(c
                                                                 + static_cast<size_t>(i) * _numChannels
                                                                 + static_cast<size_t>(j) * _numChannels * _w
                                                                 + static_cast<size_t>(k) * _numChannels * _w * _h);
                        CPPUNIT_ASSERT_EQUAL(imageBuffer[index], channels[c].at<T>(k, j, i));
                    }
                }
            }
        }
    }
    else if(_h > 0)
    {
        CPPUNIT_ASSERT_EQUAL(_image->getNumberOfDimensions(), static_cast<size_t>(_cvImage.dims));
        CPPUNIT_ASSERT_EQUAL(_w, static_cast<size_t>(_cvImage.size[1]));
        CPPUNIT_ASSERT_EQUAL(_h, static_cast<size_t>(_cvImage.size[0]));

        for(int j = 0 ; j < _cvImage.size[0] ; ++j)
        {
            for(int i = 0 ; i < _cvImage.size[1] ; ++i)
            {
                for(std::uint8_t c = 0 ; c < _numChannels ; ++c)
                {
                    const size_t index = c
                                         + static_cast<size_t>(i) * _numChannels
                                         + static_cast<size_t>(j) * _numChannels * _w;
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

        for(int i = 0 ; i < _cvImage.size[1] ; ++i)
        {
            for(std::uint8_t c = 0 ; c < _numChannels ; ++c)
            {
                const size_t index = static_cast<size_t>(c + i * _numChannels);
                CPPUNIT_ASSERT_EQUAL(imageBuffer[index], channels[c].at<T>(i));
            }
        }
    }
}

//------------------------------------------------------------------------------

template<typename T>
static void testMoveToCV(size_t _w, size_t _h, size_t _d, std::uint8_t _numChannels)
{
    const std::vector<T> imageBuffer = genImageBuffer<T>(_w, _h, _d, _numChannels);
    data::Image::sptr image          = genImage<T>(imageBuffer, _w, _h, _d, _numChannels);

    ::cv::Mat cvImage = io::opencv::Image::moveToCv(image);

    // Since we share the same buffer, compare the pointers
    const auto dumpLock = image->lock();
    CPPUNIT_ASSERT_EQUAL(image->getBuffer(), static_cast<void*>(cvImage.data));

    compareImages<T>(cvImage, image, _w, _h, _d, _numChannels);
}

//------------------------------------------------------------------------------

template<typename T>
static void testCopyFromCV(size_t _w, size_t _h, size_t _d, std::uint8_t _numChannels)
{
    const std::vector<T> imageBuffer = genImageBuffer<T>(_w, _h, _d, _numChannels);
    const ::cv::Mat cvImage          = genCvImage<T>(imageBuffer, _w, _h, _d, _numChannels);

    data::Image::sptr image = data::Image::New();
    io::opencv::Image::copyFromCv(*image.get(), cvImage);

    // Since we copy the buffer, ensure the pointers are different
    const auto dumpLock = image->lock();
    CPPUNIT_ASSERT(image->getBuffer() != static_cast<void*>(cvImage.data));

    compareImages<T>(cvImage, image, _w, _h, _d, _numChannels);
}

//------------------------------------------------------------------------------

template<typename T>
static void testCopyToCV(size_t _w, size_t _h, size_t _d, std::uint8_t _numChannels)
{
    const std::vector<T> imageBuffer = genImageBuffer<T>(_w, _h, _d, _numChannels);
    data::Image::csptr image         = genImage<T>(imageBuffer, _w, _h, _d, _numChannels);

    ::cv::Mat cvImage = io::opencv::Image::copyToCv(image);

    // Since we copy the buffer, ensure the pointers are different
    const auto dumpLock = image->lock();
    CPPUNIT_ASSERT(image->getBuffer() != static_cast<void*>(cvImage.data));

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

} // namespace ut

} // namespace sight::io::opencv
