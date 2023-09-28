/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <data/FrameTL.hpp>
#include <data/timeline/Buffer.hpp>

#include <utest/exception.hpp>

#include <algorithm>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::FrameTLTest);

namespace sight::data::ut
{

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
        data::FrameTL::sptr timeline = std::make_shared<data::FrameTL>();
        timeline->initPoolSize(100, 200, core::type::FLOAT, data::FrameTL::PixelFormat::GRAY_SCALE);

        CPPUNIT_ASSERT_EQUAL(std::size_t(200), timeline->getHeight());
        CPPUNIT_ASSERT_EQUAL(std::size_t(100), timeline->getWidth());
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), timeline->numComponents());
        CPPUNIT_ASSERT_EQUAL(core::type::FLOAT, timeline->getType());
        CPPUNIT_ASSERT_EQUAL(data::FrameTL::PixelFormat::GRAY_SCALE, timeline->getPixelFormat());

        data::FrameTL::sptr timeline2 = data::FrameTL::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->getHeight(), timeline2->getHeight());
        CPPUNIT_ASSERT_EQUAL(timeline->getWidth(), timeline2->getWidth());
        CPPUNIT_ASSERT_EQUAL(timeline->numComponents(), timeline2->numComponents());
        CPPUNIT_ASSERT_EQUAL(timeline->getType(), timeline2->getType());
        CPPUNIT_ASSERT_EQUAL(timeline->getPixelFormat(), timeline->getPixelFormat());

        CPPUNIT_ASSERT(*timeline == *timeline2);
    }
    {
        data::FrameTL::sptr timeline = std::make_shared<data::FrameTL>();
        timeline->initPoolSize(100, 200, core::type::UINT8, data::FrameTL::PixelFormat::RGB);

        CPPUNIT_ASSERT_EQUAL(std::size_t(200), timeline->getHeight());
        CPPUNIT_ASSERT_EQUAL(std::size_t(100), timeline->getWidth());
        CPPUNIT_ASSERT_EQUAL(std::size_t(3), timeline->numComponents());
        CPPUNIT_ASSERT_EQUAL(core::type::UINT8, timeline->getType());
        CPPUNIT_ASSERT_EQUAL(data::FrameTL::PixelFormat::RGB, timeline->getPixelFormat());

        data::FrameTL::sptr timeline2 = data::FrameTL::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->getHeight(), timeline2->getHeight());
        CPPUNIT_ASSERT_EQUAL(timeline->getWidth(), timeline2->getWidth());
        CPPUNIT_ASSERT_EQUAL(timeline->numComponents(), timeline2->numComponents());
        CPPUNIT_ASSERT_EQUAL(timeline->getType(), timeline2->getType());
        CPPUNIT_ASSERT_EQUAL(timeline->getPixelFormat(), timeline->getPixelFormat());

        CPPUNIT_ASSERT(*timeline == *timeline2);
    }
    {
        data::FrameTL::sptr timeline = std::make_shared<data::FrameTL>();
        timeline->initPoolSize(100, 200, core::type::UINT8, data::FrameTL::PixelFormat::BGR);

        CPPUNIT_ASSERT_EQUAL(std::size_t(200), timeline->getHeight());
        CPPUNIT_ASSERT_EQUAL(std::size_t(100), timeline->getWidth());
        CPPUNIT_ASSERT_EQUAL(std::size_t(3), timeline->numComponents());
        CPPUNIT_ASSERT_EQUAL(core::type::UINT8, timeline->getType());
        CPPUNIT_ASSERT_EQUAL(data::FrameTL::PixelFormat::BGR, timeline->getPixelFormat());

        data::FrameTL::sptr timeline2 = data::FrameTL::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->getHeight(), timeline2->getHeight());
        CPPUNIT_ASSERT_EQUAL(timeline->getWidth(), timeline2->getWidth());
        CPPUNIT_ASSERT_EQUAL(timeline->numComponents(), timeline2->numComponents());
        CPPUNIT_ASSERT_EQUAL(timeline->getType(), timeline2->getType());
        CPPUNIT_ASSERT_EQUAL(timeline->getPixelFormat(), timeline->getPixelFormat());

        CPPUNIT_ASSERT(*timeline == *timeline2);
    }
    {
        data::FrameTL::sptr timeline = std::make_shared<data::FrameTL>();
        timeline->initPoolSize(100, 200, core::type::UINT8, data::FrameTL::PixelFormat::RGBA);

        CPPUNIT_ASSERT_EQUAL(std::size_t(200), timeline->getHeight());
        CPPUNIT_ASSERT_EQUAL(std::size_t(100), timeline->getWidth());
        CPPUNIT_ASSERT_EQUAL(std::size_t(4), timeline->numComponents());
        CPPUNIT_ASSERT_EQUAL(core::type::UINT8, timeline->getType());
        CPPUNIT_ASSERT_EQUAL(data::FrameTL::PixelFormat::RGBA, timeline->getPixelFormat());

        data::FrameTL::sptr timeline2 = data::FrameTL::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->getHeight(), timeline2->getHeight());
        CPPUNIT_ASSERT_EQUAL(timeline->getWidth(), timeline2->getWidth());
        CPPUNIT_ASSERT_EQUAL(timeline->numComponents(), timeline2->numComponents());
        CPPUNIT_ASSERT_EQUAL(timeline->getType(), timeline2->getType());
        CPPUNIT_ASSERT_EQUAL(timeline->getPixelFormat(), timeline->getPixelFormat());

        CPPUNIT_ASSERT(*timeline == *timeline2);
    }
    {
        data::FrameTL::sptr timeline = std::make_shared<data::FrameTL>();
        timeline->initPoolSize(100, 200, core::type::UINT8, data::FrameTL::PixelFormat::BGRA);

        CPPUNIT_ASSERT_EQUAL(std::size_t(200), timeline->getHeight());
        CPPUNIT_ASSERT_EQUAL(std::size_t(100), timeline->getWidth());
        CPPUNIT_ASSERT_EQUAL(std::size_t(4), timeline->numComponents());
        CPPUNIT_ASSERT_EQUAL(core::type::UINT8, timeline->getType());
        CPPUNIT_ASSERT_EQUAL(data::FrameTL::PixelFormat::BGRA, timeline->getPixelFormat());

        data::FrameTL::sptr timeline2 = data::FrameTL::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->getHeight(), timeline2->getHeight());
        CPPUNIT_ASSERT_EQUAL(timeline->getWidth(), timeline2->getWidth());
        CPPUNIT_ASSERT_EQUAL(timeline->numComponents(), timeline2->numComponents());
        CPPUNIT_ASSERT_EQUAL(timeline->getType(), timeline2->getType());
        CPPUNIT_ASSERT_EQUAL(timeline->getPixelFormat(), timeline->getPixelFormat());

        CPPUNIT_ASSERT(*timeline == *timeline2);
    }
}

//------------------------------------------------------------------------------

void FrameTLTest::pushTest()
{
    data::FrameTL::sptr timeline = std::make_shared<data::FrameTL>();
    timeline->initPoolSize(10, 20, core::type::UINT8, data::FrameTL::PixelFormat::RGB);

    CPPUNIT_ASSERT_EQUAL(std::size_t(20), timeline->getHeight());
    CPPUNIT_ASSERT_EQUAL(std::size_t(10), timeline->getWidth());
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), timeline->numComponents());
    CPPUNIT_ASSERT_EQUAL(core::type::UINT8, timeline->getType());
    CPPUNIT_ASSERT_EQUAL(data::FrameTL::PixelFormat::RGB, timeline->getPixelFormat());

    core::hires_clock::type time1 = core::hires_clock::get_time_in_milli_sec();
    core::hires_clock::type time2 = time1 + 42;

    SPTR(data::FrameTL::BufferType) data1 = timeline->createBuffer(time1);
    std::uint8_t* bufferData1 = data1->addElement(0);
    std::memset(bufferData1, 1, (10LL * 20 * 3));
    SPTR(data::FrameTL::BufferType) data2 = timeline->createBuffer(time2);
    std::uint8_t* bufferData2 = data2->addElement(0);
    std::memset(bufferData2, 2, (10LL * 20 * 3));

    timeline->pushObject(data1);
    timeline->pushObject(data2);

    CSPTR(data::timeline::Object) dataPushed1 = timeline->getObject(time1);
    CPPUNIT_ASSERT(data1 == dataPushed1);

    CSPTR(data::timeline::Object) dataPushed2 = timeline->getObject(time2);
    CPPUNIT_ASSERT(data2 == dataPushed2);

    CSPTR(data::timeline::Object) dataPushed1Bis = timeline->getClosestObject(time1 + 1.5);
    CSPTR(data::FrameTL::BufferType) buff        =
        std::dynamic_pointer_cast<const data::FrameTL::BufferType>(dataPushed1Bis);
    CPPUNIT_ASSERT(buff);
    CPPUNIT_ASSERT_EQUAL(buff, timeline->getClosestBuffer(time1 + 1.5));
    const std::uint8_t* buffData = &buff->getElement(0);
    CPPUNIT_ASSERT_EQUAL(std::uint8_t(1), buffData[0]);
    CPPUNIT_ASSERT_EQUAL(std::uint8_t(1), buffData[300]);
    CPPUNIT_ASSERT_EQUAL(std::uint8_t(1), buffData[599]);

    CSPTR(data::timeline::Object) dataPushed2Bis = timeline->getNewerObject();
    CPPUNIT_ASSERT(data2 == dataPushed2Bis);

    core::hires_clock::type time2Pushed = timeline->getNewerTimestamp();
    CPPUNIT_ASSERT_EQUAL(time2, time2Pushed);

    timeline->clearTimeline();
    CSPTR(data::timeline::Object) nullObj = timeline->getNewerObject();
    CPPUNIT_ASSERT(nullObj == nullptr);
}

//------------------------------------------------------------------------------

void FrameTLTest::copyTest()
{
    data::FrameTL::sptr timeline = std::make_shared<data::FrameTL>();
    timeline->initPoolSize(11, 22, core::type::UINT8, data::FrameTL::PixelFormat::RGBA);

    CPPUNIT_ASSERT_EQUAL(std::size_t(22), timeline->getHeight());
    CPPUNIT_ASSERT_EQUAL(std::size_t(11), timeline->getWidth());
    CPPUNIT_ASSERT_EQUAL(std::size_t(4), timeline->numComponents());
    CPPUNIT_ASSERT_EQUAL(core::type::UINT8, timeline->getType());
    CPPUNIT_ASSERT_EQUAL(data::FrameTL::PixelFormat::RGBA, timeline->getPixelFormat());

    core::hires_clock::type time1 = core::hires_clock::get_time_in_milli_sec();
    core::hires_clock::type time2 = time1 + 125;

    SPTR(data::FrameTL::BufferType) data1 = timeline->createBuffer(time1);
    std::uint8_t* bufferData1 = data1->addElement(0);
    std::memset(bufferData1, 1, (11LL * 22 * 4));
    SPTR(data::FrameTL::BufferType) data2 = timeline->createBuffer(time2);
    std::uint8_t* bufferData2 = data2->addElement(0);
    std::memset(bufferData2, 2, (11LL * 22 * 4));

    timeline->pushObject(data1);
    timeline->pushObject(data2);

    CSPTR(data::timeline::Object) dataPushed1 = timeline->getObject(time1);
    CPPUNIT_ASSERT(data1 == dataPushed1);

    CSPTR(data::timeline::Object) dataPushed2 = timeline->getObject(time2);
    CPPUNIT_ASSERT(data2 == dataPushed2);

    data::FrameTL::sptr copiedTimeline = data::FrameTL::copy(timeline);

    CSPTR(data::timeline::Object) copiedData1 = copiedTimeline->getClosestObject(time1);
    CPPUNIT_ASSERT_EQUAL(time1, copiedData1->getTimestamp());
    CSPTR(data::FrameTL::BufferType) copiedBuff =
        std::dynamic_pointer_cast<const data::FrameTL::BufferType>(copiedData1);
    CPPUNIT_ASSERT(copiedBuff);
    CSPTR(data::FrameTL::BufferType) buff =
        std::dynamic_pointer_cast<const data::FrameTL::BufferType>(timeline->getClosestBuffer(time1));
    const std::uint8_t* copiedBuffData = &copiedBuff->getElement(0);
    const std::uint8_t* buffData       = &buff->getElement(0);

    for(std::size_t i = 0 ; i < 10LL * 20 * 4 ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(buffData[i], copiedBuffData[i]);
    }

    const core::hires_clock::type copiedTime2 = copiedTimeline->getNewerTimestamp();
    CPPUNIT_ASSERT_EQUAL(time2, copiedTime2);

    timeline->clearTimeline();
    CSPTR(data::timeline::Object) nullObj = timeline->getNewerObject();
    CPPUNIT_ASSERT(nullObj == nullptr);

    CSPTR(data::timeline::Object) copiedData2 = copiedTimeline->getClosestBuffer(time2);
    CPPUNIT_ASSERT(copiedData2);
    CPPUNIT_ASSERT_EQUAL(time2, copiedData2->getTimestamp());

    copiedTimeline->clearTimeline();
    CSPTR(data::timeline::Object) nullObj2 = timeline->getNewerObject();
    CPPUNIT_ASSERT(nullObj == nullptr);
}

//------------------------------------------------------------------------------

void FrameTLTest::equalityTest()
{
    auto frame1 = std::make_shared<data::FrameTL>();
    auto frame2 = std::make_shared<data::FrameTL>();

    CPPUNIT_ASSERT(*frame1 == *frame2 && !(*frame1 != *frame2));

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(...) \
    frame1->initPoolSize(__VA_ARGS__); \
    CPPUNIT_ASSERT_MESSAGE( \
        "Frames must be different when the first is set with " #__VA_ARGS__, \
        *frame1 != *frame2 && !(*frame1 == *frame2) \
    ); \
    frame2->initPoolSize(__VA_ARGS__); \
    CPPUNIT_ASSERT_MESSAGE( \
        "Frames must be equal when they are both set with " #__VA_ARGS__, \
        *frame1 == *frame2 && !(*frame1 != *frame2) \
    );

    TEST(1, 1, core::type::UINT8, data::FrameTL::PixelFormat::RGBA);
    TEST(1, 2, core::type::UINT8, data::FrameTL::PixelFormat::RGBA);
    TEST(1, 1, core::type::INT8, data::FrameTL::PixelFormat::RGBA);
    TEST(1, 1, core::type::UINT8, data::FrameTL::PixelFormat::RGB);

    #undef TEST
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut
