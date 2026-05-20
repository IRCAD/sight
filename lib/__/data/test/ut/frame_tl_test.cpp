/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <data/frame_tl.hpp>
#include <data/timeline/buffer.hpp>

#include <utest/exception.hpp>

#include <doctest/doctest.h>

#include <algorithm>

TEST_SUITE("sight::data::frame_tl")
{
//------------------------------------------------------------------------------

    TEST_CASE("init")
    {
        {
            sight::data::frame_tl::sptr timeline = std::make_shared<sight::data::frame_tl>();
            timeline->init_pool_size(
                100,
                200,
                sight::core::type::FLOAT,
                sight::data::frame_tl::pixel_format::gray_scale
            );

            CHECK_EQ(std::size_t(200), timeline->get_height());
            CHECK_EQ(std::size_t(100), timeline->get_width());
            CHECK_EQ(std::size_t(1), timeline->num_components());
            CHECK_EQ(sight::core::type::FLOAT, timeline->type());
            CHECK_EQ(sight::data::frame_tl::pixel_format::gray_scale, timeline->pixel_format());

            sight::data::frame_tl::sptr timeline2 = sight::data::frame_tl::copy(timeline);
            CHECK_EQ(timeline->get_height(), timeline2->get_height());
            CHECK_EQ(timeline->get_width(), timeline2->get_width());
            CHECK_EQ(timeline->num_components(), timeline2->num_components());
            CHECK_EQ(timeline->type(), timeline2->type());
            CHECK_EQ(timeline->pixel_format(), timeline->pixel_format());

            CHECK(*timeline == *timeline2);
        }
        {
            sight::data::frame_tl::sptr timeline = std::make_shared<sight::data::frame_tl>();
            timeline->init_pool_size(100, 200, sight::core::type::UINT8, sight::data::frame_tl::pixel_format::rgb);

            CHECK_EQ(std::size_t(200), timeline->get_height());
            CHECK_EQ(std::size_t(100), timeline->get_width());
            CHECK_EQ(std::size_t(3), timeline->num_components());
            CHECK_EQ(sight::core::type::UINT8, timeline->type());
            CHECK_EQ(sight::data::frame_tl::pixel_format::rgb, timeline->pixel_format());

            sight::data::frame_tl::sptr timeline2 = sight::data::frame_tl::copy(timeline);
            CHECK_EQ(timeline->get_height(), timeline2->get_height());
            CHECK_EQ(timeline->get_width(), timeline2->get_width());
            CHECK_EQ(timeline->num_components(), timeline2->num_components());
            CHECK_EQ(timeline->type(), timeline2->type());
            CHECK_EQ(timeline->pixel_format(), timeline->pixel_format());

            CHECK(*timeline == *timeline2);
        }
        {
            sight::data::frame_tl::sptr timeline = std::make_shared<sight::data::frame_tl>();
            timeline->init_pool_size(100, 200, sight::core::type::UINT8, sight::data::frame_tl::pixel_format::bgr);

            CHECK_EQ(std::size_t(200), timeline->get_height());
            CHECK_EQ(std::size_t(100), timeline->get_width());
            CHECK_EQ(std::size_t(3), timeline->num_components());
            CHECK_EQ(sight::core::type::UINT8, timeline->type());
            CHECK_EQ(sight::data::frame_tl::pixel_format::bgr, timeline->pixel_format());

            sight::data::frame_tl::sptr timeline2 = sight::data::frame_tl::copy(timeline);
            CHECK_EQ(timeline->get_height(), timeline2->get_height());
            CHECK_EQ(timeline->get_width(), timeline2->get_width());
            CHECK_EQ(timeline->num_components(), timeline2->num_components());
            CHECK_EQ(timeline->type(), timeline2->type());
            CHECK_EQ(timeline->pixel_format(), timeline->pixel_format());

            CHECK(*timeline == *timeline2);
        }
        {
            sight::data::frame_tl::sptr timeline = std::make_shared<sight::data::frame_tl>();
            timeline->init_pool_size(100, 200, sight::core::type::UINT8, sight::data::frame_tl::pixel_format::rgba);

            CHECK_EQ(std::size_t(200), timeline->get_height());
            CHECK_EQ(std::size_t(100), timeline->get_width());
            CHECK_EQ(std::size_t(4), timeline->num_components());
            CHECK_EQ(sight::core::type::UINT8, timeline->type());
            CHECK_EQ(sight::data::frame_tl::pixel_format::rgba, timeline->pixel_format());

            sight::data::frame_tl::sptr timeline2 = sight::data::frame_tl::copy(timeline);
            CHECK_EQ(timeline->get_height(), timeline2->get_height());
            CHECK_EQ(timeline->get_width(), timeline2->get_width());
            CHECK_EQ(timeline->num_components(), timeline2->num_components());
            CHECK_EQ(timeline->type(), timeline2->type());
            CHECK_EQ(timeline->pixel_format(), timeline->pixel_format());

            CHECK(*timeline == *timeline2);
        }
        {
            sight::data::frame_tl::sptr timeline = std::make_shared<sight::data::frame_tl>();
            timeline->init_pool_size(100, 200, sight::core::type::UINT8, sight::data::frame_tl::pixel_format::bgra);

            CHECK_EQ(std::size_t(200), timeline->get_height());
            CHECK_EQ(std::size_t(100), timeline->get_width());
            CHECK_EQ(std::size_t(4), timeline->num_components());
            CHECK_EQ(sight::core::type::UINT8, timeline->type());
            CHECK_EQ(sight::data::frame_tl::pixel_format::bgra, timeline->pixel_format());

            sight::data::frame_tl::sptr timeline2 = sight::data::frame_tl::copy(timeline);
            CHECK_EQ(timeline->get_height(), timeline2->get_height());
            CHECK_EQ(timeline->get_width(), timeline2->get_width());
            CHECK_EQ(timeline->num_components(), timeline2->num_components());
            CHECK_EQ(timeline->type(), timeline2->type());
            CHECK_EQ(timeline->pixel_format(), timeline->pixel_format());

            CHECK(*timeline == *timeline2);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("push")
    {
        sight::data::frame_tl::sptr timeline = std::make_shared<sight::data::frame_tl>();
        timeline->init_pool_size(10, 20, sight::core::type::UINT8, sight::data::frame_tl::pixel_format::rgb);

        CHECK_EQ(std::size_t(20), timeline->get_height());
        CHECK_EQ(std::size_t(10), timeline->get_width());
        CHECK_EQ(std::size_t(3), timeline->num_components());
        CHECK_EQ(sight::core::type::UINT8, timeline->type());
        CHECK_EQ(sight::data::frame_tl::pixel_format::rgb, timeline->pixel_format());

        sight::core::clock::type time1 = sight::core::clock::get_time_in_milli_sec();
        sight::core::clock::type time2 = time1 + 42;

        SPTR(sight::data::frame_tl::buffer_t) data1 = timeline->create_buffer(time1);
        std::uint8_t* buffer_data1 = data1->add_element(0);
        std::memset(buffer_data1, 1, (10LL * 20 * 3));
        SPTR(sight::data::frame_tl::buffer_t) data2 = timeline->create_buffer(time2);
        std::uint8_t* buffer_data2 = data2->add_element(0);
        std::memset(buffer_data2, 2, (10LL * 20 * 3));

        timeline->push_object(data1);
        timeline->push_object(data2);

        CSPTR(sight::data::timeline::object) data_pushed1 = timeline->get_object(time1);
        CHECK(data1 == data_pushed1);

        CSPTR(sight::data::timeline::object) data_pushed2 = timeline->get_object(time2);
        CHECK(data2 == data_pushed2);

        CSPTR(sight::data::timeline::object) data_pushed1_bis = timeline->get_closest_object(time1 + 1.5);
        CSPTR(sight::data::frame_tl::buffer_t) buff           =
            std::dynamic_pointer_cast<const sight::data::frame_tl::buffer_t>(data_pushed1_bis);
        CHECK(buff);
        CHECK_EQ(buff, timeline->get_closest_buffer(time1 + 1.5));
        const std::uint8_t* buff_data = &buff->get_element(0);
        CHECK_EQ(std::uint8_t(1), buff_data[0]);
        CHECK_EQ(std::uint8_t(1), buff_data[300]);
        CHECK_EQ(std::uint8_t(1), buff_data[599]);

        CSPTR(sight::data::timeline::object) data_pushed2_bis = timeline->get_newer_object();
        CHECK(data2 == data_pushed2_bis);

        sight::core::clock::type time2_pushed = timeline->get_newer_timestamp();
        CHECK_EQ(time2, time2_pushed);

        timeline->clear_timeline();
        CSPTR(sight::data::timeline::object) null_obj = timeline->get_newer_object();
        CHECK(null_obj == nullptr);
    }

//------------------------------------------------------------------------------

    TEST_CASE("copy")
    {
        sight::data::frame_tl::sptr timeline = std::make_shared<sight::data::frame_tl>();
        timeline->init_pool_size(11, 22, sight::core::type::UINT8, sight::data::frame_tl::pixel_format::rgba);

        CHECK_EQ(std::size_t(22), timeline->get_height());
        CHECK_EQ(std::size_t(11), timeline->get_width());
        CHECK_EQ(std::size_t(4), timeline->num_components());
        CHECK_EQ(sight::core::type::UINT8, timeline->type());
        CHECK_EQ(sight::data::frame_tl::pixel_format::rgba, timeline->pixel_format());

        sight::core::clock::type time1 = sight::core::clock::get_time_in_milli_sec();
        sight::core::clock::type time2 = time1 + 125;

        SPTR(sight::data::frame_tl::buffer_t) data1 = timeline->create_buffer(time1);
        std::uint8_t* buffer_data1 = data1->add_element(0);
        std::memset(buffer_data1, 1, (11LL * 22 * 4));
        SPTR(sight::data::frame_tl::buffer_t) data2 = timeline->create_buffer(time2);
        std::uint8_t* buffer_data2 = data2->add_element(0);
        std::memset(buffer_data2, 2, (11LL * 22 * 4));

        timeline->push_object(data1);
        timeline->push_object(data2);

        CSPTR(sight::data::timeline::object) data_pushed1 = timeline->get_object(time1);
        CHECK(data1 == data_pushed1);

        CSPTR(sight::data::timeline::object) data_pushed2 = timeline->get_object(time2);
        CHECK(data2 == data_pushed2);

        sight::data::frame_tl::sptr copied_timeline = sight::data::frame_tl::copy(timeline);

        CSPTR(sight::data::timeline::object) copied_data1 = copied_timeline->get_closest_object(time1);
        CHECK_EQ(time1, copied_data1->get_timestamp());
        CSPTR(sight::data::frame_tl::buffer_t) copied_buff =
            std::dynamic_pointer_cast<const sight::data::frame_tl::buffer_t>(copied_data1);
        CHECK(copied_buff);
        CSPTR(sight::data::frame_tl::buffer_t) buff =
            std::dynamic_pointer_cast<const sight::data::frame_tl::buffer_t>(timeline->get_closest_buffer(time1));
        const std::uint8_t* copied_buff_data = &copied_buff->get_element(0);
        const std::uint8_t* buff_data        = &buff->get_element(0);

        for(std::size_t i = 0 ; i < 10LL * 20 * 4 ; ++i)
        {
            CHECK_EQ(buff_data[i], copied_buff_data[i]);
        }

        const sight::core::clock::type copied_time2 = copied_timeline->get_newer_timestamp();
        CHECK_EQ(time2, copied_time2);

        timeline->clear_timeline();
        CSPTR(sight::data::timeline::object) null_obj = timeline->get_newer_object();
        CHECK(null_obj == nullptr);

        CSPTR(sight::data::timeline::object) copied_data2 = copied_timeline->get_closest_buffer(time2);
        CHECK(copied_data2);
        CHECK_EQ(time2, copied_data2->get_timestamp());

        copied_timeline->clear_timeline();
        CSPTR(sight::data::timeline::object) null_obj2 = timeline->get_newer_object();
        CHECK(null_obj == nullptr);
    }

//------------------------------------------------------------------------------

    TEST_CASE("equality")
    {
        auto frame1 = std::make_shared<sight::data::frame_tl>();
        auto frame2 = std::make_shared<sight::data::frame_tl>();

        CHECK(*frame1 == *frame2);
        CHECK(!(*frame1 != *frame2));

        // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(...) \
            frame1->init_pool_size(__VA_ARGS__); \
            CHECK_MESSAGE( \
                !(*frame1 == *frame2), \
                "Frames must be different when the first is set with " #__VA_ARGS__ \
            ); \
            CHECK_MESSAGE( \
                *frame1 != *frame2, \
                "Frames must be different when the first is set with " #__VA_ARGS__ \
            ); \
            frame2->init_pool_size(__VA_ARGS__); \
            CHECK_MESSAGE( \
                *frame1 == *frame2, \
                "Frames must be equal when they are both set with " #__VA_ARGS__ \
            ); \
            CHECK_MESSAGE( \
                !(*frame1 != *frame2), \
                "Frames must be equal when they are both set with " #__VA_ARGS__ \
            );

        TEST(1, 1, sight::core::type::UINT8, sight::data::frame_tl::pixel_format::rgba);
        TEST(1, 2, sight::core::type::UINT8, sight::data::frame_tl::pixel_format::rgba);
        TEST(1, 1, sight::core::type::INT8, sight::data::frame_tl::pixel_format::rgba);
        TEST(1, 1, sight::core::type::UINT8, sight::data::frame_tl::pixel_format::rgb);

    #undef TEST
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::data::frame_tl")
