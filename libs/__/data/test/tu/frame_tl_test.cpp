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

#include "frame_tl_test.hpp"

#include <data/frame_tl.hpp>
#include <data/timeline/buffer.hpp>

#include <utest/exception.hpp>

#include <algorithm>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::frame_tl_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void frame_tl_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void frame_tl_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void frame_tl_test::initTest()
{
    {
        data::frame_tl::sptr timeline = std::make_shared<data::frame_tl>();
        timeline->initPoolSize(100, 200, core::type::FLOAT, data::frame_tl::PixelFormat::GRAY_SCALE);

        CPPUNIT_ASSERT_EQUAL(std::size_t(200), timeline->getHeight());
        CPPUNIT_ASSERT_EQUAL(std::size_t(100), timeline->getWidth());
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), timeline->numComponents());
        CPPUNIT_ASSERT_EQUAL(core::type::FLOAT, timeline->getType());
        CPPUNIT_ASSERT_EQUAL(data::frame_tl::PixelFormat::GRAY_SCALE, timeline->getPixelFormat());

        data::frame_tl::sptr timeline2 = data::frame_tl::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->getHeight(), timeline2->getHeight());
        CPPUNIT_ASSERT_EQUAL(timeline->getWidth(), timeline2->getWidth());
        CPPUNIT_ASSERT_EQUAL(timeline->numComponents(), timeline2->numComponents());
        CPPUNIT_ASSERT_EQUAL(timeline->getType(), timeline2->getType());
        CPPUNIT_ASSERT_EQUAL(timeline->getPixelFormat(), timeline->getPixelFormat());

        CPPUNIT_ASSERT(*timeline == *timeline2);
    }
    {
        data::frame_tl::sptr timeline = std::make_shared<data::frame_tl>();
        timeline->initPoolSize(100, 200, core::type::UINT8, data::frame_tl::PixelFormat::RGB);

        CPPUNIT_ASSERT_EQUAL(std::size_t(200), timeline->getHeight());
        CPPUNIT_ASSERT_EQUAL(std::size_t(100), timeline->getWidth());
        CPPUNIT_ASSERT_EQUAL(std::size_t(3), timeline->numComponents());
        CPPUNIT_ASSERT_EQUAL(core::type::UINT8, timeline->getType());
        CPPUNIT_ASSERT_EQUAL(data::frame_tl::PixelFormat::RGB, timeline->getPixelFormat());

        data::frame_tl::sptr timeline2 = data::frame_tl::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->getHeight(), timeline2->getHeight());
        CPPUNIT_ASSERT_EQUAL(timeline->getWidth(), timeline2->getWidth());
        CPPUNIT_ASSERT_EQUAL(timeline->numComponents(), timeline2->numComponents());
        CPPUNIT_ASSERT_EQUAL(timeline->getType(), timeline2->getType());
        CPPUNIT_ASSERT_EQUAL(timeline->getPixelFormat(), timeline->getPixelFormat());

        CPPUNIT_ASSERT(*timeline == *timeline2);
    }
    {
        data::frame_tl::sptr timeline = std::make_shared<data::frame_tl>();
        timeline->initPoolSize(100, 200, core::type::UINT8, data::frame_tl::PixelFormat::BGR);

        CPPUNIT_ASSERT_EQUAL(std::size_t(200), timeline->getHeight());
        CPPUNIT_ASSERT_EQUAL(std::size_t(100), timeline->getWidth());
        CPPUNIT_ASSERT_EQUAL(std::size_t(3), timeline->numComponents());
        CPPUNIT_ASSERT_EQUAL(core::type::UINT8, timeline->getType());
        CPPUNIT_ASSERT_EQUAL(data::frame_tl::PixelFormat::BGR, timeline->getPixelFormat());

        data::frame_tl::sptr timeline2 = data::frame_tl::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->getHeight(), timeline2->getHeight());
        CPPUNIT_ASSERT_EQUAL(timeline->getWidth(), timeline2->getWidth());
        CPPUNIT_ASSERT_EQUAL(timeline->numComponents(), timeline2->numComponents());
        CPPUNIT_ASSERT_EQUAL(timeline->getType(), timeline2->getType());
        CPPUNIT_ASSERT_EQUAL(timeline->getPixelFormat(), timeline->getPixelFormat());

        CPPUNIT_ASSERT(*timeline == *timeline2);
    }
    {
        data::frame_tl::sptr timeline = std::make_shared<data::frame_tl>();
        timeline->initPoolSize(100, 200, core::type::UINT8, data::frame_tl::PixelFormat::RGBA);

        CPPUNIT_ASSERT_EQUAL(std::size_t(200), timeline->getHeight());
        CPPUNIT_ASSERT_EQUAL(std::size_t(100), timeline->getWidth());
        CPPUNIT_ASSERT_EQUAL(std::size_t(4), timeline->numComponents());
        CPPUNIT_ASSERT_EQUAL(core::type::UINT8, timeline->getType());
        CPPUNIT_ASSERT_EQUAL(data::frame_tl::PixelFormat::RGBA, timeline->getPixelFormat());

        data::frame_tl::sptr timeline2 = data::frame_tl::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->getHeight(), timeline2->getHeight());
        CPPUNIT_ASSERT_EQUAL(timeline->getWidth(), timeline2->getWidth());
        CPPUNIT_ASSERT_EQUAL(timeline->numComponents(), timeline2->numComponents());
        CPPUNIT_ASSERT_EQUAL(timeline->getType(), timeline2->getType());
        CPPUNIT_ASSERT_EQUAL(timeline->getPixelFormat(), timeline->getPixelFormat());

        CPPUNIT_ASSERT(*timeline == *timeline2);
    }
    {
        data::frame_tl::sptr timeline = std::make_shared<data::frame_tl>();
        timeline->initPoolSize(100, 200, core::type::UINT8, data::frame_tl::PixelFormat::BGRA);

        CPPUNIT_ASSERT_EQUAL(std::size_t(200), timeline->getHeight());
        CPPUNIT_ASSERT_EQUAL(std::size_t(100), timeline->getWidth());
        CPPUNIT_ASSERT_EQUAL(std::size_t(4), timeline->numComponents());
        CPPUNIT_ASSERT_EQUAL(core::type::UINT8, timeline->getType());
        CPPUNIT_ASSERT_EQUAL(data::frame_tl::PixelFormat::BGRA, timeline->getPixelFormat());

        data::frame_tl::sptr timeline2 = data::frame_tl::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->getHeight(), timeline2->getHeight());
        CPPUNIT_ASSERT_EQUAL(timeline->getWidth(), timeline2->getWidth());
        CPPUNIT_ASSERT_EQUAL(timeline->numComponents(), timeline2->numComponents());
        CPPUNIT_ASSERT_EQUAL(timeline->getType(), timeline2->getType());
        CPPUNIT_ASSERT_EQUAL(timeline->getPixelFormat(), timeline->getPixelFormat());

        CPPUNIT_ASSERT(*timeline == *timeline2);
    }
}

//------------------------------------------------------------------------------

void frame_tl_test::pushTest()
{
    data::frame_tl::sptr timeline = std::make_shared<data::frame_tl>();
    timeline->initPoolSize(10, 20, core::type::UINT8, data::frame_tl::PixelFormat::RGB);

    CPPUNIT_ASSERT_EQUAL(std::size_t(20), timeline->getHeight());
    CPPUNIT_ASSERT_EQUAL(std::size_t(10), timeline->getWidth());
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), timeline->numComponents());
    CPPUNIT_ASSERT_EQUAL(core::type::UINT8, timeline->getType());
    CPPUNIT_ASSERT_EQUAL(data::frame_tl::PixelFormat::RGB, timeline->getPixelFormat());

    core::hires_clock::type time1 = core::hires_clock::get_time_in_milli_sec();
    core::hires_clock::type time2 = time1 + 42;

    SPTR(data::frame_tl::buffer_t) data1 = timeline->createBuffer(time1);
    std::uint8_t* buffer_data1 = data1->addElement(0);
    std::memset(buffer_data1, 1, (10LL * 20 * 3));
    SPTR(data::frame_tl::buffer_t) data2 = timeline->createBuffer(time2);
    std::uint8_t* buffer_data2 = data2->addElement(0);
    std::memset(buffer_data2, 2, (10LL * 20 * 3));

    timeline->pushObject(data1);
    timeline->pushObject(data2);

    CSPTR(data::timeline::object) data_pushed1 = timeline->getObject(time1);
    CPPUNIT_ASSERT(data1 == data_pushed1);

    CSPTR(data::timeline::object) data_pushed2 = timeline->getObject(time2);
    CPPUNIT_ASSERT(data2 == data_pushed2);

    CSPTR(data::timeline::object) data_pushed1_bis = timeline->getClosestObject(time1 + 1.5);
    CSPTR(data::frame_tl::buffer_t) buff           =
        std::dynamic_pointer_cast<const data::frame_tl::buffer_t>(data_pushed1_bis);
    CPPUNIT_ASSERT(buff);
    CPPUNIT_ASSERT_EQUAL(buff, timeline->getClosestBuffer(time1 + 1.5));
    const std::uint8_t* buff_data = &buff->getElement(0);
    CPPUNIT_ASSERT_EQUAL(std::uint8_t(1), buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(std::uint8_t(1), buff_data[300]);
    CPPUNIT_ASSERT_EQUAL(std::uint8_t(1), buff_data[599]);

    CSPTR(data::timeline::object) data_pushed2_bis = timeline->getNewerObject();
    CPPUNIT_ASSERT(data2 == data_pushed2_bis);

    core::hires_clock::type time2_pushed = timeline->getNewerTimestamp();
    CPPUNIT_ASSERT_EQUAL(time2, time2_pushed);

    timeline->clearTimeline();
    CSPTR(data::timeline::object) null_obj = timeline->getNewerObject();
    CPPUNIT_ASSERT(null_obj == nullptr);
}

//------------------------------------------------------------------------------

void frame_tl_test::copyTest()
{
    data::frame_tl::sptr timeline = std::make_shared<data::frame_tl>();
    timeline->initPoolSize(11, 22, core::type::UINT8, data::frame_tl::PixelFormat::RGBA);

    CPPUNIT_ASSERT_EQUAL(std::size_t(22), timeline->getHeight());
    CPPUNIT_ASSERT_EQUAL(std::size_t(11), timeline->getWidth());
    CPPUNIT_ASSERT_EQUAL(std::size_t(4), timeline->numComponents());
    CPPUNIT_ASSERT_EQUAL(core::type::UINT8, timeline->getType());
    CPPUNIT_ASSERT_EQUAL(data::frame_tl::PixelFormat::RGBA, timeline->getPixelFormat());

    core::hires_clock::type time1 = core::hires_clock::get_time_in_milli_sec();
    core::hires_clock::type time2 = time1 + 125;

    SPTR(data::frame_tl::buffer_t) data1 = timeline->createBuffer(time1);
    std::uint8_t* buffer_data1 = data1->addElement(0);
    std::memset(buffer_data1, 1, (11LL * 22 * 4));
    SPTR(data::frame_tl::buffer_t) data2 = timeline->createBuffer(time2);
    std::uint8_t* buffer_data2 = data2->addElement(0);
    std::memset(buffer_data2, 2, (11LL * 22 * 4));

    timeline->pushObject(data1);
    timeline->pushObject(data2);

    CSPTR(data::timeline::object) data_pushed1 = timeline->getObject(time1);
    CPPUNIT_ASSERT(data1 == data_pushed1);

    CSPTR(data::timeline::object) data_pushed2 = timeline->getObject(time2);
    CPPUNIT_ASSERT(data2 == data_pushed2);

    data::frame_tl::sptr copied_timeline = data::frame_tl::copy(timeline);

    CSPTR(data::timeline::object) copied_data1 = copied_timeline->getClosestObject(time1);
    CPPUNIT_ASSERT_EQUAL(time1, copied_data1->getTimestamp());
    CSPTR(data::frame_tl::buffer_t) copied_buff =
        std::dynamic_pointer_cast<const data::frame_tl::buffer_t>(copied_data1);
    CPPUNIT_ASSERT(copied_buff);
    CSPTR(data::frame_tl::buffer_t) buff =
        std::dynamic_pointer_cast<const data::frame_tl::buffer_t>(timeline->getClosestBuffer(time1));
    const std::uint8_t* copied_buff_data = &copied_buff->getElement(0);
    const std::uint8_t* buff_data        = &buff->getElement(0);

    for(std::size_t i = 0 ; i < 10LL * 20 * 4 ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(buff_data[i], copied_buff_data[i]);
    }

    const core::hires_clock::type copied_time2 = copied_timeline->getNewerTimestamp();
    CPPUNIT_ASSERT_EQUAL(time2, copied_time2);

    timeline->clearTimeline();
    CSPTR(data::timeline::object) null_obj = timeline->getNewerObject();
    CPPUNIT_ASSERT(null_obj == nullptr);

    CSPTR(data::timeline::object) copied_data2 = copied_timeline->getClosestBuffer(time2);
    CPPUNIT_ASSERT(copied_data2);
    CPPUNIT_ASSERT_EQUAL(time2, copied_data2->getTimestamp());

    copied_timeline->clearTimeline();
    CSPTR(data::timeline::object) null_obj2 = timeline->getNewerObject();
    CPPUNIT_ASSERT(null_obj == nullptr);
}

//------------------------------------------------------------------------------

void frame_tl_test::equalityTest()
{
    auto frame1 = std::make_shared<data::frame_tl>();
    auto frame2 = std::make_shared<data::frame_tl>();

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

    TEST(1, 1, core::type::UINT8, data::frame_tl::PixelFormat::RGBA);
    TEST(1, 2, core::type::UINT8, data::frame_tl::PixelFormat::RGBA);
    TEST(1, 1, core::type::INT8, data::frame_tl::PixelFormat::RGBA);
    TEST(1, 1, core::type::UINT8, data::frame_tl::PixelFormat::RGB);

    #undef TEST
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut
