/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "FrameTLTest.hpp"

#include "cvCommon.hpp"

#include <cvIO/FrameTL.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::cvIO::ut::FrameTLTest );

namespace cvIO
{
namespace ut
{

//------------------------------------------------------------------------------

template <typename T>
static std::pair< ::arData::FrameTL::sptr, SPTR(::arData::FrameTL::BufferType) >
genFrameTL(size_t _w, size_t _h, std::uint8_t _numChannels)
{
    const ::fwTools::Type type = ::fwTools::Type::create<T>();

    ::arData::FrameTL::sptr frameTL = ::arData::FrameTL::New();
    frameTL->initPoolSize(_w, _h, type, _numChannels);
    auto buffer = frameTL->createBuffer(::fwCore::HiResClock::getTimeInMilliSec());

    return std::make_pair(frameTL, buffer);
}
//------------------------------------------------------------------------------

template <typename T>
static void compareImages( const ::cv::Mat& _cvImage,
                           const ::arData::FrameTL::BufferType::ElementType* _buffer,
                           size_t _w, size_t _h, std::uint8_t _numChannels)
{
    std::vector< ::cv::Mat> channels(_numChannels);
    ::cv::split(_cvImage, channels);

    CPPUNIT_ASSERT_EQUAL(2, _cvImage.dims);
    CPPUNIT_ASSERT_EQUAL(_w, static_cast<size_t>(_cvImage.size[1]));
    CPPUNIT_ASSERT_EQUAL(_h, static_cast<size_t>(_cvImage.size[0]));

    const T* imageBuffer = reinterpret_cast<const T*>(_buffer);

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

//------------------------------------------------------------------------------

template <typename T>
static void testMoveToCV(size_t _w, size_t _h, std::uint8_t _numChannels)
{
    const std::vector<T> imageBuffer = genImageBuffer<T>(_w, _h, 0, _numChannels);

    ::arData::FrameTL::sptr frameTL;
    SPTR(::arData::FrameTL::BufferType) buffer;

    std::tie(frameTL, buffer) = genFrameTL<T>(_w, _h, _numChannels);
    auto eltBuffer = buffer->addElement(0);
    std::copy(imageBuffer.begin(), imageBuffer.end(), eltBuffer);

    {
        ::cv::Mat cvImage;
        ::cvIO::FrameTL::moveToCv(frameTL, eltBuffer, cvImage);

        // Since we share the same buffer, compare the pointers
        CPPUNIT_ASSERT_EQUAL(static_cast<void*>(eltBuffer), static_cast<void*>(cvImage.data));

        compareImages<T>(cvImage, eltBuffer, _w, _h, _numChannels);
    }
    {
        ::cv::Mat cvImage2;
        cvImage2 = ::cvIO::FrameTL::moveToCv(frameTL, eltBuffer);

        // Since we share the same buffer, compare the pointers
        CPPUNIT_ASSERT_EQUAL(static_cast<void*>(eltBuffer), static_cast<void*>(cvImage2.data));

        compareImages<T>(cvImage2, eltBuffer, _w, _h, _numChannels);
    }
}

//------------------------------------------------------------------------------

template <typename T>
static void testCopyFromCV(size_t _w, size_t _h, std::uint8_t _numChannels)
{
    const std::vector<T> imageBuffer = genImageBuffer<T>(_w, _h, 0, _numChannels);
    const ::cv::Mat cvImage          = genCvImage<T>(imageBuffer, _w, _h, 0, _numChannels);

    ::arData::FrameTL::sptr frameTL;
    SPTR(::arData::FrameTL::BufferType) buffer;

    std::tie(frameTL, buffer) = genFrameTL<T>(_w, _h, _numChannels);
    auto eltBuffer = buffer->addElement(0);

    ::cvIO::FrameTL::copyFromCv(frameTL, eltBuffer, cvImage);

    // Since we copy the buffer, ensure the pointers are different
    CPPUNIT_ASSERT(static_cast<void*>(eltBuffer) != static_cast<void*>(cvImage.data));

    compareImages<T>(cvImage, eltBuffer, _w, _h, _numChannels);
}

//------------------------------------------------------------------------------

template <typename T>
static void testCopyToCV(size_t _w, size_t _h, std::uint8_t _numChannels)
{
    const std::vector<T> imageBuffer = genImageBuffer<T>(_w, _h, 0, _numChannels);

    ::arData::FrameTL::sptr frameTL;
    SPTR(::arData::FrameTL::BufferType) buffer;

    std::tie(frameTL, buffer) = genFrameTL<T>(_w, _h, _numChannels);
    auto eltBuffer = buffer->addElement(0);
    std::copy(imageBuffer.begin(), imageBuffer.end(), eltBuffer);

    ::cv::Mat cvImage;
    ::cvIO::FrameTL::copyToCv(frameTL, eltBuffer, cvImage);

    // Since we copy the buffer, ensure the pointers are different
    CPPUNIT_ASSERT(static_cast<void*>(eltBuffer) != static_cast<void*>(cvImage.data));

    compareImages<T>(cvImage, eltBuffer, _w, _h, _numChannels);
}

//------------------------------------------------------------------------------

void FrameTLTest::setUp()
{
    // Set up context before running a test.
    std::srand(149129791);
}

//------------------------------------------------------------------------------

void FrameTLTest::tearDown()
{
}

//------------------------------------------------------------------------------

void FrameTLTest::moveToCv()
{
    testMoveToCV<std::uint8_t>(10, 2, 1);
    testMoveToCV<std::uint8_t>(1, 20, 2);
    testMoveToCV<std::uint8_t>(6, 12, 3);
    testMoveToCV<std::uint8_t>(10, 7, 4);

    testMoveToCV<std::int8_t>(10, 8, 1);
    testMoveToCV<std::int8_t>(10, 20, 2);
    testMoveToCV<std::int8_t>(23, 20, 3);
    testMoveToCV<std::int8_t>(76, 2, 4);

    testMoveToCV<std::uint16_t>(10, 2, 1);
    testMoveToCV<std::uint16_t>(10, 20, 2);
    testMoveToCV<std::uint16_t>(6, 20, 3);
    testMoveToCV<std::uint16_t>(10, 7, 4);

    testMoveToCV<std::int16_t>(10, 401, 1);
    testMoveToCV<std::int16_t>(10, 20, 2);
    testMoveToCV<std::int16_t>(23, 20, 3);
    testMoveToCV<std::int16_t>(76, 2, 4);

    testMoveToCV<std::int32_t>(10, 32, 1);
    testMoveToCV<std::int32_t>(10, 20, 2);
    testMoveToCV<std::int32_t>(23, 20, 3);
    testMoveToCV<std::int32_t>(76, 2, 4);

    testMoveToCV<float>(10, 32, 1);
    testMoveToCV<float>(10, 20, 2);
    testMoveToCV<float>(90, 20, 3);
    testMoveToCV<float>(76, 2, 4);

    testMoveToCV<double>(12, 32, 1);
    testMoveToCV<double>(10, 1, 2);
    testMoveToCV<double>(23, 8, 3);
    testMoveToCV<double>(76, 2, 4);
}

//------------------------------------------------------------------------------

void FrameTLTest::copyFromCv()
{
    testCopyFromCV<std::int16_t>(10, 7, 1);
    testCopyFromCV<std::uint8_t>(10, 2, 1);
    testCopyFromCV<std::uint8_t>(1, 20, 2);
    testCopyFromCV<std::uint8_t>(6, 12, 3);
    testCopyFromCV<std::uint8_t>(10, 7, 4);

    testCopyFromCV<std::int8_t>(10, 144, 1);
    testCopyFromCV<std::int8_t>(10, 20, 2);
    testCopyFromCV<std::int8_t>(23, 20, 3);
    testCopyFromCV<std::int8_t>(76, 2, 4);

    testCopyFromCV<std::uint16_t>(10, 21, 1);
    testCopyFromCV<std::uint16_t>(10, 2, 1);
    testCopyFromCV<std::uint16_t>(10, 20, 2);
    testCopyFromCV<std::uint16_t>(6, 20, 3);
    testCopyFromCV<std::uint16_t>(10, 7, 4);

    testCopyFromCV<std::int16_t>(10, 35, 1);
    testCopyFromCV<std::int16_t>(10, 20, 2);
    testCopyFromCV<std::int16_t>(23, 20, 3);
    testCopyFromCV<std::int16_t>(76, 2, 4);

    testCopyFromCV<std::int32_t>(10, 32, 1);
    testCopyFromCV<std::int32_t>(10, 20, 2);
    testCopyFromCV<std::int32_t>(23, 20, 3);
    testCopyFromCV<std::int32_t>(76, 2, 4);

    testCopyFromCV<float>(10, 32, 1);
    testCopyFromCV<float>(10, 20, 2);
    testCopyFromCV<float>(90, 20, 3);
    testCopyFromCV<float>(76, 2, 4);

    testCopyFromCV<double>(12, 32, 1);
    testCopyFromCV<double>(10, 1, 2);
    testCopyFromCV<double>(23, 8, 3);
    testCopyFromCV<double>(76, 2, 4);
}

//------------------------------------------------------------------------------

void FrameTLTest::copyToCv()
{
    testCopyToCV<std::uint8_t>(10, 2, 1);
    testCopyToCV<std::uint8_t>(1, 20, 2);
    testCopyToCV<std::uint8_t>(6, 12, 3);
    testCopyToCV<std::uint8_t>(10, 7, 4);

    testCopyToCV<std::int8_t>(10, 21, 1);
    testCopyToCV<std::int8_t>(10, 20, 2);
    testCopyToCV<std::int8_t>(23, 20, 3);
    testCopyToCV<std::int8_t>(76, 2, 4);

    testCopyToCV<std::uint16_t>(54, 2, 1);
    testCopyToCV<std::uint16_t>(87, 20, 2);
    testCopyToCV<std::uint16_t>(6, 20, 3);
    testCopyToCV<std::uint16_t>(10, 7, 4);

    testCopyToCV<std::int16_t>(10, 78, 1);
    testCopyToCV<std::int16_t>(10, 20, 2);
    testCopyToCV<std::int16_t>(23, 20, 3);
    testCopyToCV<std::int16_t>(76, 2, 4);

    testCopyToCV<std::int32_t>(10, 32, 1);
    testCopyToCV<std::int32_t>(78, 20, 2);
    testCopyToCV<std::int32_t>(23, 20, 3);
    testCopyToCV<std::int32_t>(76, 2, 4);

    testCopyToCV<float>(10, 32, 1);
    testCopyToCV<float>(10, 20, 2);
    testCopyToCV<float>(90, 20, 3);
    testCopyToCV<float>(76, 2, 4);

    testCopyToCV<double>(12, 32, 1);
    testCopyToCV<double>(10, 1, 2);
    testCopyToCV<double>(23, 8, 3);
    testCopyToCV<double>(76, 2, 4);
}

}// namespace ut

}// namespace cvIO
