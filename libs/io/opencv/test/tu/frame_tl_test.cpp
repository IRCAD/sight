/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include "frame_tl_test.hpp"

#include "cv_common.hpp"

#include <io/opencv/frame_tl.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::opencv::ut::frame_tl_test);

namespace sight::io::opencv::ut
{

//------------------------------------------------------------------------------

template<typename T>
static std::pair<data::frame_tl::sptr, SPTR(data::frame_tl::BufferType)> genFrameTL(
    std::size_t _w,
    std::size_t _h,
    std::uint8_t _numChannels
)
{
    CPPUNIT_ASSERT(_numChannels == 1 || _numChannels == 3 || _numChannels == 4);

    const core::type type = core::type::get<T>();

    data::frame_tl::sptr frameTL = std::make_shared<data::frame_tl>();
    frameTL->initPoolSize(
        _w,
        _h,
        type,
        _numChannels == 1
        ? data::frame_tl::PixelFormat::GRAY_SCALE
        : _numChannels == 3
        ? data::frame_tl::PixelFormat::RGB
        : _numChannels == 4
        ? data::frame_tl::PixelFormat::RGBA
        : data::frame_tl::PixelFormat::UNDEFINED
    );
    auto buffer = frameTL->createBuffer(core::hires_clock::get_time_in_milli_sec());

    return std::make_pair(frameTL, buffer);
}

//------------------------------------------------------------------------------

template<typename T>
static void compareImages(
    const cv::Mat& _cvImage,
    const data::frame_tl::BufferType::ElementType* _buffer,
    std::size_t _w,
    std::size_t _h,
    std::uint8_t _numChannels
)
{
    std::vector<cv::Mat> channels(_numChannels);
    cv::split(_cvImage, channels);

    CPPUNIT_ASSERT_EQUAL(2, _cvImage.dims);
    CPPUNIT_ASSERT_EQUAL(_w, static_cast<std::size_t>(_cvImage.size[1]));
    CPPUNIT_ASSERT_EQUAL(_h, static_cast<std::size_t>(_cvImage.size[0]));

    const T* imageBuffer = reinterpret_cast<const T*>(_buffer);

    for(int j = 0 ; j < _cvImage.size[0] ; ++j)
    {
        for(int i = 0 ; i < _cvImage.size[1] ; ++i)
        {
            for(std::uint8_t c = 0 ; c < _numChannels ; ++c)
            {
                const std::size_t index = c
                                          + static_cast<std::size_t>(i) * _numChannels
                                          + static_cast<std::size_t>(j) * _numChannels * _w;
                CPPUNIT_ASSERT_EQUAL(imageBuffer[index], channels[c].at<T>(j, i));
            }
        }
    }
}

//------------------------------------------------------------------------------

template<typename T>
static void testMoveToCV(std::size_t _w, std::size_t _h, std::uint8_t _numChannels)
{
    const std::vector<T> imageBuffer = genImageBuffer<T>(_w, _h, 0, _numChannels);

    data::frame_tl::sptr frameTL;
    SPTR(data::frame_tl::BufferType) buffer;

    std::tie(frameTL, buffer) = genFrameTL<T>(_w, _h, _numChannels);
    auto* eltBuffer = buffer->addElement(0);
    std::copy(imageBuffer.begin(), imageBuffer.end(), eltBuffer);

    {
        cv::Mat cvImage;
        io::opencv::frame_tl::moveToCv(frameTL, eltBuffer, cvImage);

        // Since we share the same buffer, compare the pointers
        CPPUNIT_ASSERT_EQUAL(static_cast<void*>(eltBuffer), static_cast<void*>(cvImage.data));

        compareImages<T>(cvImage, eltBuffer, _w, _h, _numChannels);
    }
    {
        cv::Mat cvImage2;
        cvImage2 = io::opencv::frame_tl::moveToCv(frameTL, eltBuffer);

        // Since we share the same buffer, compare the pointers
        CPPUNIT_ASSERT_EQUAL(static_cast<void*>(eltBuffer), static_cast<void*>(cvImage2.data));

        compareImages<T>(cvImage2, eltBuffer, _w, _h, _numChannels);
    }
}

//------------------------------------------------------------------------------

template<typename T>
static void testCopyFromCV(std::size_t _w, std::size_t _h, std::uint8_t _numChannels)
{
    const std::vector<T> imageBuffer = genImageBuffer<T>(_w, _h, 0, _numChannels);
    const cv::Mat cvImage            = genCvImage<T>(imageBuffer, _w, _h, 0, _numChannels);

    data::frame_tl::sptr frameTL;
    SPTR(data::frame_tl::BufferType) buffer;

    std::tie(frameTL, buffer) = genFrameTL<T>(_w, _h, _numChannels);
    auto* eltBuffer = buffer->addElement(0);

    io::opencv::frame_tl::copyFromCv(frameTL, eltBuffer, cvImage);

    // Since we copy the buffer, ensure the pointers are different
    CPPUNIT_ASSERT(static_cast<void*>(eltBuffer) != static_cast<void*>(cvImage.data));

    compareImages<T>(cvImage, eltBuffer, _w, _h, _numChannels);
}

//------------------------------------------------------------------------------

template<typename T>
static void testCopyToCV(std::size_t _w, std::size_t _h, std::uint8_t _numChannels)
{
    const std::vector<T> imageBuffer = genImageBuffer<T>(_w, _h, 0, _numChannels);

    data::frame_tl::sptr frameTL;
    SPTR(data::frame_tl::BufferType) buffer;

    std::tie(frameTL, buffer) = genFrameTL<T>(_w, _h, _numChannels);
    auto* eltBuffer = buffer->addElement(0);
    std::copy(imageBuffer.begin(), imageBuffer.end(), eltBuffer);

    cv::Mat cvImage;
    io::opencv::frame_tl::copyToCv(frameTL, eltBuffer, cvImage);

    // Since we copy the buffer, ensure the pointers are different
    CPPUNIT_ASSERT(static_cast<void*>(eltBuffer) != static_cast<void*>(cvImage.data));

    compareImages<T>(cvImage, eltBuffer, _w, _h, _numChannels);
}

//------------------------------------------------------------------------------

void frame_tl_test::setUp()
{
}

//------------------------------------------------------------------------------

void frame_tl_test::tearDown()
{
}

//------------------------------------------------------------------------------

void frame_tl_test::moveToCv()
{
    testMoveToCV<std::uint8_t>(10, 2, 1);
    testMoveToCV<std::uint8_t>(6, 12, 3);
    testMoveToCV<std::uint8_t>(10, 7, 4);
}

//------------------------------------------------------------------------------

void frame_tl_test::copyFromCv()
{
    testCopyFromCV<std::uint8_t>(10, 7, 1);
    testCopyFromCV<std::uint8_t>(10, 2, 1);
    testCopyFromCV<std::uint8_t>(6, 12, 3);
    testCopyFromCV<std::uint8_t>(10, 7, 4);
}

//------------------------------------------------------------------------------

void frame_tl_test::copyToCv()
{
    testCopyToCV<std::uint8_t>(10, 2, 1);
    testCopyToCV<std::uint8_t>(6, 12, 3);
    testCopyToCV<std::uint8_t>(10, 7, 4);
}

} // namespace sight::io::opencv::ut
