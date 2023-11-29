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

void frame_tl_test::init_test()
{
    {
        data::frame_tl::sptr timeline = std::make_shared<data::frame_tl>();
        timeline->init_pool_size(100, 200, core::type::FLOAT, data::frame_tl::pixel_format::gray_scale);

        CPPUNIT_ASSERT_EQUAL(std::size_t(200), timeline->get_height());
        CPPUNIT_ASSERT_EQUAL(std::size_t(100), timeline->get_width());
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), timeline->num_components());
        CPPUNIT_ASSERT_EQUAL(core::type::FLOAT, timeline->type());
        CPPUNIT_ASSERT_EQUAL(data::frame_tl::pixel_format::gray_scale, timeline->pixel_format());

        data::frame_tl::sptr timeline2 = data::frame_tl::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->get_height(), timeline2->get_height());
        CPPUNIT_ASSERT_EQUAL(timeline->get_width(), timeline2->get_width());
        CPPUNIT_ASSERT_EQUAL(timeline->num_components(), timeline2->num_components());
        CPPUNIT_ASSERT_EQUAL(timeline->type(), timeline2->type());
        CPPUNIT_ASSERT_EQUAL(timeline->pixel_format(), timeline->pixel_format());

        CPPUNIT_ASSERT(*timeline == *timeline2);
    }
    {
        data::frame_tl::sptr timeline = std::make_shared<data::frame_tl>();
        timeline->init_pool_size(100, 200, core::type::UINT8, data::frame_tl::pixel_format::rgb);

        CPPUNIT_ASSERT_EQUAL(std::size_t(200), timeline->get_height());
        CPPUNIT_ASSERT_EQUAL(std::size_t(100), timeline->get_width());
        CPPUNIT_ASSERT_EQUAL(std::size_t(3), timeline->num_components());
        CPPUNIT_ASSERT_EQUAL(core::type::UINT8, timeline->type());
        CPPUNIT_ASSERT_EQUAL(data::frame_tl::pixel_format::rgb, timeline->pixel_format());

        data::frame_tl::sptr timeline2 = data::frame_tl::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->get_height(), timeline2->get_height());
        CPPUNIT_ASSERT_EQUAL(timeline->get_width(), timeline2->get_width());
        CPPUNIT_ASSERT_EQUAL(timeline->num_components(), timeline2->num_components());
        CPPUNIT_ASSERT_EQUAL(timeline->type(), timeline2->type());
        CPPUNIT_ASSERT_EQUAL(timeline->pixel_format(), timeline->pixel_format());

        CPPUNIT_ASSERT(*timeline == *timeline2);
    }
    {
        data::frame_tl::sptr timeline = std::make_shared<data::frame_tl>();
        timeline->init_pool_size(100, 200, core::type::UINT8, data::frame_tl::pixel_format::bgr);

        CPPUNIT_ASSERT_EQUAL(std::size_t(200), timeline->get_height());
        CPPUNIT_ASSERT_EQUAL(std::size_t(100), timeline->get_width());
        CPPUNIT_ASSERT_EQUAL(std::size_t(3), timeline->num_components());
        CPPUNIT_ASSERT_EQUAL(core::type::UINT8, timeline->type());
        CPPUNIT_ASSERT_EQUAL(data::frame_tl::pixel_format::bgr, timeline->pixel_format());

        data::frame_tl::sptr timeline2 = data::frame_tl::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->get_height(), timeline2->get_height());
        CPPUNIT_ASSERT_EQUAL(timeline->get_width(), timeline2->get_width());
        CPPUNIT_ASSERT_EQUAL(timeline->num_components(), timeline2->num_components());
        CPPUNIT_ASSERT_EQUAL(timeline->type(), timeline2->type());
        CPPUNIT_ASSERT_EQUAL(timeline->pixel_format(), timeline->pixel_format());

        CPPUNIT_ASSERT(*timeline == *timeline2);
    }
    {
        data::frame_tl::sptr timeline = std::make_shared<data::frame_tl>();
        timeline->init_pool_size(100, 200, core::type::UINT8, data::frame_tl::pixel_format::rgba);

        CPPUNIT_ASSERT_EQUAL(std::size_t(200), timeline->get_height());
        CPPUNIT_ASSERT_EQUAL(std::size_t(100), timeline->get_width());
        CPPUNIT_ASSERT_EQUAL(std::size_t(4), timeline->num_components());
        CPPUNIT_ASSERT_EQUAL(core::type::UINT8, timeline->type());
        CPPUNIT_ASSERT_EQUAL(data::frame_tl::pixel_format::rgba, timeline->pixel_format());

        data::frame_tl::sptr timeline2 = data::frame_tl::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->get_height(), timeline2->get_height());
        CPPUNIT_ASSERT_EQUAL(timeline->get_width(), timeline2->get_width());
        CPPUNIT_ASSERT_EQUAL(timeline->num_components(), timeline2->num_components());
        CPPUNIT_ASSERT_EQUAL(timeline->type(), timeline2->type());
        CPPUNIT_ASSERT_EQUAL(timeline->pixel_format(), timeline->pixel_format());

        CPPUNIT_ASSERT(*timeline == *timeline2);
    }
    {
        data::frame_tl::sptr timeline = std::make_shared<data::frame_tl>();
        timeline->init_pool_size(100, 200, core::type::UINT8, data::frame_tl::pixel_format::bgra);

        CPPUNIT_ASSERT_EQUAL(std::size_t(200), timeline->get_height());
        CPPUNIT_ASSERT_EQUAL(std::size_t(100), timeline->get_width());
        CPPUNIT_ASSERT_EQUAL(std::size_t(4), timeline->num_components());
        CPPUNIT_ASSERT_EQUAL(core::type::UINT8, timeline->type());
        CPPUNIT_ASSERT_EQUAL(data::frame_tl::pixel_format::bgra, timeline->pixel_format());

        data::frame_tl::sptr timeline2 = data::frame_tl::copy(timeline);
        CPPUNIT_ASSERT_EQUAL(timeline->get_height(), timeline2->get_height());
        CPPUNIT_ASSERT_EQUAL(timeline->get_width(), timeline2->get_width());
        CPPUNIT_ASSERT_EQUAL(timeline->num_components(), timeline2->num_components());
        CPPUNIT_ASSERT_EQUAL(timeline->type(), timeline2->type());
        CPPUNIT_ASSERT_EQUAL(timeline->pixel_format(), timeline->pixel_format());

        CPPUNIT_ASSERT(*timeline == *timeline2);
    }
}

//------------------------------------------------------------------------------

void frame_tl_test::push_test()
{
    data::frame_tl::sptr timeline = std::make_shared<data::frame_tl>();
    timeline->init_pool_size(10, 20, core::type::UINT8, data::frame_tl::pixel_format::rgb);

    CPPUNIT_ASSERT_EQUAL(std::size_t(20), timeline->get_height());
    CPPUNIT_ASSERT_EQUAL(std::size_t(10), timeline->get_width());
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), timeline->num_components());
    CPPUNIT_ASSERT_EQUAL(core::type::UINT8, timeline->type());
    CPPUNIT_ASSERT_EQUAL(data::frame_tl::pixel_format::rgb, timeline->pixel_format());

    core::clock::type time1 = core::clock::get_time_in_milli_sec();
    core::clock::type time2 = time1 + 42;

    SPTR(data::frame_tl::buffer_t) data1 = timeline->create_buffer(time1);
    std::uint8_t* buffer_data1 = data1->add_element(0);
    std::memset(buffer_data1, 1, (10LL * 20 * 3));
    SPTR(data::frame_tl::buffer_t) data2 = timeline->create_buffer(time2);
    std::uint8_t* buffer_data2 = data2->add_element(0);
    std::memset(buffer_data2, 2, (10LL * 20 * 3));

    timeline->push_object(data1);
    timeline->push_object(data2);

    CSPTR(data::timeline::object) data_pushed1 = timeline->get_object(time1);
    CPPUNIT_ASSERT(data1 == data_pushed1);

    CSPTR(data::timeline::object) data_pushed2 = timeline->get_object(time2);
    CPPUNIT_ASSERT(data2 == data_pushed2);

    CSPTR(data::timeline::object) data_pushed1_bis = timeline->get_closest_object(time1 + 1.5);
    CSPTR(data::frame_tl::buffer_t) buff           =
        std::dynamic_pointer_cast<const data::frame_tl::buffer_t>(data_pushed1_bis);
    CPPUNIT_ASSERT(buff);
    CPPUNIT_ASSERT_EQUAL(buff, timeline->get_closest_buffer(time1 + 1.5));
    const std::uint8_t* buff_data = &buff->get_element(0);
    CPPUNIT_ASSERT_EQUAL(std::uint8_t(1), buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(std::uint8_t(1), buff_data[300]);
    CPPUNIT_ASSERT_EQUAL(std::uint8_t(1), buff_data[599]);

    CSPTR(data::timeline::object) data_pushed2_bis = timeline->get_newer_object();
    CPPUNIT_ASSERT(data2 == data_pushed2_bis);

    core::clock::type time2_pushed = timeline->get_newer_timestamp();
    CPPUNIT_ASSERT_EQUAL(time2, time2_pushed);

    timeline->clear_timeline();
    CSPTR(data::timeline::object) null_obj = timeline->get_newer_object();
    CPPUNIT_ASSERT(null_obj == nullptr);
}

//------------------------------------------------------------------------------

void frame_tl_test::copy_test()
{
    data::frame_tl::sptr timeline = std::make_shared<data::frame_tl>();
    timeline->init_pool_size(11, 22, core::type::UINT8, data::frame_tl::pixel_format::rgba);

    CPPUNIT_ASSERT_EQUAL(std::size_t(22), timeline->get_height());
    CPPUNIT_ASSERT_EQUAL(std::size_t(11), timeline->get_width());
    CPPUNIT_ASSERT_EQUAL(std::size_t(4), timeline->num_components());
    CPPUNIT_ASSERT_EQUAL(core::type::UINT8, timeline->type());
    CPPUNIT_ASSERT_EQUAL(data::frame_tl::pixel_format::rgba, timeline->pixel_format());

    core::clock::type time1 = core::clock::get_time_in_milli_sec();
    core::clock::type time2 = time1 + 125;

    SPTR(data::frame_tl::buffer_t) data1 = timeline->create_buffer(time1);
    std::uint8_t* buffer_data1 = data1->add_element(0);
    std::memset(buffer_data1, 1, (11LL * 22 * 4));
    SPTR(data::frame_tl::buffer_t) data2 = timeline->create_buffer(time2);
    std::uint8_t* buffer_data2 = data2->add_element(0);
    std::memset(buffer_data2, 2, (11LL * 22 * 4));

    timeline->push_object(data1);
    timeline->push_object(data2);

    CSPTR(data::timeline::object) data_pushed1 = timeline->get_object(time1);
    CPPUNIT_ASSERT(data1 == data_pushed1);

    CSPTR(data::timeline::object) data_pushed2 = timeline->get_object(time2);
    CPPUNIT_ASSERT(data2 == data_pushed2);

    data::frame_tl::sptr copied_timeline = data::frame_tl::copy(timeline);

    CSPTR(data::timeline::object) copied_data1 = copied_timeline->get_closest_object(time1);
    CPPUNIT_ASSERT_EQUAL(time1, copied_data1->get_timestamp());
    CSPTR(data::frame_tl::buffer_t) copied_buff =
        std::dynamic_pointer_cast<const data::frame_tl::buffer_t>(copied_data1);
    CPPUNIT_ASSERT(copied_buff);
    CSPTR(data::frame_tl::buffer_t) buff =
        std::dynamic_pointer_cast<const data::frame_tl::buffer_t>(timeline->get_closest_buffer(time1));
    const std::uint8_t* copied_buff_data = &copied_buff->get_element(0);
    const std::uint8_t* buff_data        = &buff->get_element(0);

    for(std::size_t i = 0 ; i < 10LL * 20 * 4 ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(buff_data[i], copied_buff_data[i]);
    }

    const core::clock::type copied_time2 = copied_timeline->get_newer_timestamp();
    CPPUNIT_ASSERT_EQUAL(time2, copied_time2);

    timeline->clear_timeline();
    CSPTR(data::timeline::object) null_obj = timeline->get_newer_object();
    CPPUNIT_ASSERT(null_obj == nullptr);

    CSPTR(data::timeline::object) copied_data2 = copied_timeline->get_closest_buffer(time2);
    CPPUNIT_ASSERT(copied_data2);
    CPPUNIT_ASSERT_EQUAL(time2, copied_data2->get_timestamp());

    copied_timeline->clear_timeline();
    CSPTR(data::timeline::object) null_obj2 = timeline->get_newer_object();
    CPPUNIT_ASSERT(null_obj == nullptr);
}

//------------------------------------------------------------------------------

void frame_tl_test::equality_test()
{
    auto frame1 = std::make_shared<data::frame_tl>();
    auto frame2 = std::make_shared<data::frame_tl>();

    CPPUNIT_ASSERT(*frame1 == *frame2 && !(*frame1 != *frame2));

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(...) \
    frame1->init_pool_size(__VA_ARGS__); \
    CPPUNIT_ASSERT_MESSAGE( \
        "Frames must be different when the first is set with " #__VA_ARGS__, \
        *frame1 != *frame2 && !(*frame1 == *frame2) \
    ); \
    frame2->init_pool_size(__VA_ARGS__); \
    CPPUNIT_ASSERT_MESSAGE( \
        "Frames must be equal when they are both set with " #__VA_ARGS__, \
        *frame1 == *frame2 && !(*frame1 != *frame2) \
    );

    TEST(1, 1, core::type::UINT8, data::frame_tl::pixel_format::rgba);
    TEST(1, 2, core::type::UINT8, data::frame_tl::pixel_format::rgba);
    TEST(1, 1, core::type::INT8, data::frame_tl::pixel_format::rgba);
    TEST(1, 1, core::type::UINT8, data::frame_tl::pixel_format::rgb);

    #undef TEST
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut
