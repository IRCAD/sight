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

#include "FrameTLTest.hpp"

#include <arData/FrameTL.hpp>
#include <arData/timeline/Buffer.hpp>

#include <fwTest/Exception.hpp>

#include <algorithm>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::arData::ut::FrameTLTest );

namespace arData
{
namespace ut
{

static ::fwTest::Exception fwTestException(""); // force link with fwTest

//------------------------------------------------------------------------------

void FrameTLTest::setUp()
{
    // Set up context before running a test.

}
//------------------------------------------------------------------------------

void FrameTLTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void FrameTLTest::initTest()
{
    {
        // deprecated
        ::arData::FrameTL::sptr timeline = ::arData::FrameTL::New();
        timeline->initPoolSize(10, 20, ::fwTools::Type::s_UINT16, 1);

        CPPUNIT_ASSERT_EQUAL(size_t(20), timeline->getHeight());
        CPPUNIT_ASSERT_EQUAL(size_t(10), timeline->getWidth());
        CPPUNIT_ASSERT_EQUAL(size_t(1), timeline->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_UINT16, timeline->getType());
        CPPUNIT_ASSERT_EQUAL(arData::FrameTL::PixelFormat::GRAY_SCALE, timeline->getPixelFormat());

        ::arData::FrameTL::sptr timeline2 = ::arData::FrameTL::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->getHeight(), timeline2->getHeight());
        CPPUNIT_ASSERT_EQUAL(timeline->getWidth(), timeline2->getWidth());
        CPPUNIT_ASSERT_EQUAL(timeline->getNumberOfComponents(), timeline2->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(timeline->getType(), timeline2->getType());
        CPPUNIT_ASSERT_EQUAL(timeline->getPixelFormat(), timeline->getPixelFormat());
    }
    {
        // deprecated
        ::arData::FrameTL::sptr timeline = ::arData::FrameTL::New();
        timeline->initPoolSize(12, 22, ::fwTools::Type::s_UINT8, 3);

        CPPUNIT_ASSERT_EQUAL(size_t(22), timeline->getHeight());
        CPPUNIT_ASSERT_EQUAL(size_t(12), timeline->getWidth());
        CPPUNIT_ASSERT_EQUAL(size_t(3), timeline->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_UINT8, timeline->getType());
        CPPUNIT_ASSERT_EQUAL(arData::FrameTL::PixelFormat::RGB, timeline->getPixelFormat());

        ::arData::FrameTL::sptr timeline2 = ::arData::FrameTL::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->getHeight(), timeline2->getHeight());
        CPPUNIT_ASSERT_EQUAL(timeline->getWidth(), timeline2->getWidth());
        CPPUNIT_ASSERT_EQUAL(timeline->getNumberOfComponents(), timeline2->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(timeline->getType(), timeline2->getType());
        CPPUNIT_ASSERT_EQUAL(timeline->getPixelFormat(), timeline->getPixelFormat());
    }
    {
        // deprecated
        ::arData::FrameTL::sptr timeline = ::arData::FrameTL::New();
        timeline->initPoolSize(112, 122, ::fwTools::Type::s_UINT8, 4);

        CPPUNIT_ASSERT_EQUAL(size_t(122), timeline->getHeight());
        CPPUNIT_ASSERT_EQUAL(size_t(112), timeline->getWidth());
        CPPUNIT_ASSERT_EQUAL(size_t(4), timeline->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_UINT8, timeline->getType());
        CPPUNIT_ASSERT_EQUAL(arData::FrameTL::PixelFormat::RGBA, timeline->getPixelFormat());

        ::arData::FrameTL::sptr timeline2 = ::arData::FrameTL::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->getHeight(), timeline2->getHeight());
        CPPUNIT_ASSERT_EQUAL(timeline->getWidth(), timeline2->getWidth());
        CPPUNIT_ASSERT_EQUAL(timeline->getNumberOfComponents(), timeline2->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(timeline->getType(), timeline2->getType());
        CPPUNIT_ASSERT_EQUAL(timeline->getPixelFormat(), timeline->getPixelFormat());
    }
    {
        // deprecated
        ::arData::FrameTL::sptr timeline = ::arData::FrameTL::New();
        timeline->initPoolSize(11, 23, ::fwTools::Type::s_UINT8, 2);

        CPPUNIT_ASSERT_EQUAL(size_t(23), timeline->getHeight());
        CPPUNIT_ASSERT_EQUAL(size_t(11), timeline->getWidth());
        CPPUNIT_ASSERT_EQUAL(size_t(2), timeline->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_UINT8, timeline->getType());
        CPPUNIT_ASSERT_EQUAL(arData::FrameTL::PixelFormat::UNDEFINED, timeline->getPixelFormat());

        ::arData::FrameTL::sptr timeline2 = ::arData::FrameTL::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->getHeight(), timeline2->getHeight());
        CPPUNIT_ASSERT_EQUAL(timeline->getWidth(), timeline2->getWidth());
        CPPUNIT_ASSERT_EQUAL(timeline->getNumberOfComponents(), timeline2->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(timeline->getType(), timeline2->getType());
        CPPUNIT_ASSERT_EQUAL(timeline->getPixelFormat(), timeline->getPixelFormat());
    }
    {
        ::arData::FrameTL::sptr timeline = ::arData::FrameTL::New();
        timeline->initPoolSize(100, 200, ::fwTools::Type::s_FLOAT, ::arData::FrameTL::PixelFormat::GRAY_SCALE);

        CPPUNIT_ASSERT_EQUAL(size_t(200), timeline->getHeight());
        CPPUNIT_ASSERT_EQUAL(size_t(100), timeline->getWidth());
        CPPUNIT_ASSERT_EQUAL(size_t(1), timeline->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_FLOAT, timeline->getType());
        CPPUNIT_ASSERT_EQUAL(arData::FrameTL::PixelFormat::GRAY_SCALE, timeline->getPixelFormat());

        ::arData::FrameTL::sptr timeline2 = ::arData::FrameTL::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->getHeight(), timeline2->getHeight());
        CPPUNIT_ASSERT_EQUAL(timeline->getWidth(), timeline2->getWidth());
        CPPUNIT_ASSERT_EQUAL(timeline->getNumberOfComponents(), timeline2->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(timeline->getType(), timeline2->getType());
        CPPUNIT_ASSERT_EQUAL(timeline->getPixelFormat(), timeline->getPixelFormat());
    }
    {
        ::arData::FrameTL::sptr timeline = ::arData::FrameTL::New();
        timeline->initPoolSize(100, 200, ::fwTools::Type::s_UINT8, ::arData::FrameTL::PixelFormat::RGB);

        CPPUNIT_ASSERT_EQUAL(size_t(200), timeline->getHeight());
        CPPUNIT_ASSERT_EQUAL(size_t(100), timeline->getWidth());
        CPPUNIT_ASSERT_EQUAL(size_t(3), timeline->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_UINT8, timeline->getType());
        CPPUNIT_ASSERT_EQUAL(arData::FrameTL::PixelFormat::RGB, timeline->getPixelFormat());

        ::arData::FrameTL::sptr timeline2 = ::arData::FrameTL::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->getHeight(), timeline2->getHeight());
        CPPUNIT_ASSERT_EQUAL(timeline->getWidth(), timeline2->getWidth());
        CPPUNIT_ASSERT_EQUAL(timeline->getNumberOfComponents(), timeline2->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(timeline->getType(), timeline2->getType());
        CPPUNIT_ASSERT_EQUAL(timeline->getPixelFormat(), timeline->getPixelFormat());
    }
    {
        ::arData::FrameTL::sptr timeline = ::arData::FrameTL::New();
        timeline->initPoolSize(100, 200, ::fwTools::Type::s_UINT8, ::arData::FrameTL::PixelFormat::BGR);

        CPPUNIT_ASSERT_EQUAL(size_t(200), timeline->getHeight());
        CPPUNIT_ASSERT_EQUAL(size_t(100), timeline->getWidth());
        CPPUNIT_ASSERT_EQUAL(size_t(3), timeline->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_UINT8, timeline->getType());
        CPPUNIT_ASSERT_EQUAL(arData::FrameTL::PixelFormat::BGR, timeline->getPixelFormat());

        ::arData::FrameTL::sptr timeline2 = ::arData::FrameTL::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->getHeight(), timeline2->getHeight());
        CPPUNIT_ASSERT_EQUAL(timeline->getWidth(), timeline2->getWidth());
        CPPUNIT_ASSERT_EQUAL(timeline->getNumberOfComponents(), timeline2->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(timeline->getType(), timeline2->getType());
        CPPUNIT_ASSERT_EQUAL(timeline->getPixelFormat(), timeline->getPixelFormat());
    }
    {
        ::arData::FrameTL::sptr timeline = ::arData::FrameTL::New();
        timeline->initPoolSize(100, 200, ::fwTools::Type::s_UINT8, ::arData::FrameTL::PixelFormat::RGBA);

        CPPUNIT_ASSERT_EQUAL(size_t(200), timeline->getHeight());
        CPPUNIT_ASSERT_EQUAL(size_t(100), timeline->getWidth());
        CPPUNIT_ASSERT_EQUAL(size_t(4), timeline->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_UINT8, timeline->getType());
        CPPUNIT_ASSERT_EQUAL(arData::FrameTL::PixelFormat::RGBA, timeline->getPixelFormat());

        ::arData::FrameTL::sptr timeline2 = ::arData::FrameTL::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->getHeight(), timeline2->getHeight());
        CPPUNIT_ASSERT_EQUAL(timeline->getWidth(), timeline2->getWidth());
        CPPUNIT_ASSERT_EQUAL(timeline->getNumberOfComponents(), timeline2->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(timeline->getType(), timeline2->getType());
        CPPUNIT_ASSERT_EQUAL(timeline->getPixelFormat(), timeline->getPixelFormat());
    }
    {
        ::arData::FrameTL::sptr timeline = ::arData::FrameTL::New();
        timeline->initPoolSize(100, 200, ::fwTools::Type::s_UINT8, ::arData::FrameTL::PixelFormat::BGRA);

        CPPUNIT_ASSERT_EQUAL(size_t(200), timeline->getHeight());
        CPPUNIT_ASSERT_EQUAL(size_t(100), timeline->getWidth());
        CPPUNIT_ASSERT_EQUAL(size_t(4), timeline->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_UINT8, timeline->getType());
        CPPUNIT_ASSERT_EQUAL(arData::FrameTL::PixelFormat::BGRA, timeline->getPixelFormat());

        ::arData::FrameTL::sptr timeline2 = ::arData::FrameTL::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->getHeight(), timeline2->getHeight());
        CPPUNIT_ASSERT_EQUAL(timeline->getWidth(), timeline2->getWidth());
        CPPUNIT_ASSERT_EQUAL(timeline->getNumberOfComponents(), timeline2->getNumberOfComponents());
        CPPUNIT_ASSERT_EQUAL(timeline->getType(), timeline2->getType());
        CPPUNIT_ASSERT_EQUAL(timeline->getPixelFormat(), timeline->getPixelFormat());
    }
}

//------------------------------------------------------------------------------

void FrameTLTest::pushTest()
{
    ::arData::FrameTL::sptr timeline = ::arData::FrameTL::New();
    timeline->initPoolSize(10, 20, ::fwTools::Type::s_UINT8, ::arData::FrameTL::PixelFormat::RGB);

    CPPUNIT_ASSERT_EQUAL(size_t(20), timeline->getHeight());
    CPPUNIT_ASSERT_EQUAL(size_t(10), timeline->getWidth());
    CPPUNIT_ASSERT_EQUAL(size_t(3), timeline->getNumberOfComponents());
    CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_UINT8, timeline->getType());
    CPPUNIT_ASSERT_EQUAL(arData::FrameTL::PixelFormat::RGB, timeline->getPixelFormat());

    ::fwCore::HiResClock::HiResClockType time1 = ::fwCore::HiResClock::getTimeInMilliSec();
    ::fwCore::HiResClock::HiResClockType time2 = time1 + 42;

    SPTR(::arData::FrameTL::BufferType) data1 = timeline->createBuffer(time1);
    ::fwTools::Type::UInt8Type* bufferData1   = data1->addElement(0);
    std::fill(bufferData1, bufferData1+(10*20*3), 1);
    SPTR(::arData::FrameTL::BufferType) data2 = timeline->createBuffer(time2);
    ::fwTools::Type::UInt8Type* bufferData2   = data2->addElement(0);
    std::fill(bufferData2, bufferData2+(10*20*3), 2);

    timeline->pushObject(data1);
    timeline->pushObject(data2);

    CSPTR(::arData::timeline::Object) dataPushed1 = timeline->getObject(time1);
    CPPUNIT_ASSERT(data1 == dataPushed1);

    CSPTR(::arData::timeline::Object) dataPushed2 = timeline->getObject(time2);
    CPPUNIT_ASSERT(data2 == dataPushed2);

    CSPTR(::arData::timeline::Object) dataPushed1Bis = timeline->getClosestObject(time1 + 1.5);
    CSPTR(::arData::FrameTL::BufferType) buff        =
        std::dynamic_pointer_cast< const ::arData::FrameTL::BufferType >(dataPushed1Bis);
    CPPUNIT_ASSERT(buff);
    CPPUNIT_ASSERT_EQUAL(buff, timeline->getClosestBuffer(time1 + 1.5));
    const ::fwTools::Type::UInt8Type* buffData = &buff->getElement(0);
    CPPUNIT_ASSERT_EQUAL(::fwTools::Type::UInt8Type(1), buffData[0]);
    CPPUNIT_ASSERT_EQUAL(::fwTools::Type::UInt8Type(1), buffData[300]);
    CPPUNIT_ASSERT_EQUAL(::fwTools::Type::UInt8Type(1), buffData[599]);

    CSPTR(::arData::timeline::Object) dataPushed2Bis = timeline->getNewerObject();
    CPPUNIT_ASSERT(data2 == dataPushed2Bis);

    ::fwCore::HiResClock::HiResClockType time2Pushed = timeline->getNewerTimestamp();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(time2, time2Pushed, 0.00001);

    timeline->clearTimeline();
    CSPTR(::arData::timeline::Object) nullObj = timeline->getNewerObject();
    CPPUNIT_ASSERT(nullObj == NULL);
}

//------------------------------------------------------------------------------

void FrameTLTest::copyTest()
{
    ::arData::FrameTL::sptr timeline = ::arData::FrameTL::New();
    timeline->initPoolSize(11, 22, ::fwTools::Type::s_UINT8, ::arData::FrameTL::PixelFormat::RGBA);

    CPPUNIT_ASSERT_EQUAL(size_t(22), timeline->getHeight());
    CPPUNIT_ASSERT_EQUAL(size_t(11), timeline->getWidth());
    CPPUNIT_ASSERT_EQUAL(size_t(4), timeline->getNumberOfComponents());
    CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_UINT8, timeline->getType());
    CPPUNIT_ASSERT_EQUAL(arData::FrameTL::PixelFormat::RGBA, timeline->getPixelFormat());

    ::fwCore::HiResClock::HiResClockType time1 = ::fwCore::HiResClock::getTimeInMilliSec();
    ::fwCore::HiResClock::HiResClockType time2 = time1 + 125;

    SPTR(::arData::FrameTL::BufferType) data1 = timeline->createBuffer(time1);
    ::fwTools::Type::UInt8Type* bufferData1   = data1->addElement(0);
    std::fill(bufferData1, bufferData1+(11*22*4), 1);
    SPTR(::arData::FrameTL::BufferType) data2 = timeline->createBuffer(time2);
    ::fwTools::Type::UInt8Type* bufferData2   = data2->addElement(0);
    std::fill(bufferData2, bufferData2+(11*22*4), 2);

    timeline->pushObject(data1);
    timeline->pushObject(data2);

    CSPTR(::arData::timeline::Object) dataPushed1 = timeline->getObject(time1);
    CPPUNIT_ASSERT(data1 == dataPushed1);

    CSPTR(::arData::timeline::Object) dataPushed2 = timeline->getObject(time2);
    CPPUNIT_ASSERT(data2 == dataPushed2);

    ::arData::FrameTL::sptr copiedTimeline = ::arData::FrameTL::copy(timeline);

    CSPTR(::arData::timeline::Object) copiedData1 = copiedTimeline->getClosestObject(time1);
    CPPUNIT_ASSERT_EQUAL(time1, copiedData1->getTimestamp());
    CSPTR(::arData::FrameTL::BufferType) copiedBuff =
        std::dynamic_pointer_cast< const ::arData::FrameTL::BufferType >(copiedData1);
    CPPUNIT_ASSERT(copiedBuff);
    CSPTR(::arData::FrameTL::BufferType) buff =
        std::dynamic_pointer_cast< const ::arData::FrameTL::BufferType >(timeline->getClosestBuffer(time1));
    const ::fwTools::Type::UInt8Type* copiedBuffData = &copiedBuff->getElement(0);
    const ::fwTools::Type::UInt8Type* buffData       = &buff->getElement(0);

    for (size_t i = 0; i < 10*20*4; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(buffData[i], copiedBuffData[i]);
    }

    const ::fwCore::HiResClock::HiResClockType copiedTime2 = copiedTimeline->getNewerTimestamp();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(time2, copiedTime2, 0.00001);

    timeline->clearTimeline();
    CSPTR(::arData::timeline::Object) nullObj = timeline->getNewerObject();
    CPPUNIT_ASSERT(nullObj == NULL);

    CSPTR(::arData::timeline::Object) copiedData2 = copiedTimeline->getClosestBuffer(time2);
    CPPUNIT_ASSERT(copiedData2);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(time2, copiedData2->getTimestamp(), 0.00001);

    copiedTimeline->clearTimeline();
    CSPTR(::arData::timeline::Object) nullObj2 = timeline->getNewerObject();
    CPPUNIT_ASSERT(nullObj == NULL);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace arData
