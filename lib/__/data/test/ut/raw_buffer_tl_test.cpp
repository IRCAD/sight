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

#include <data/raw_buffer_tl.hpp>
#include <data/timeline/buffer.hpp>

#include <utest/exception.hpp>

#include <doctest/doctest.h>

#include <array>

TEST_SUITE("sight::data::raw_buffer_tl")
{
//------------------------------------------------------------------------------

    TEST_CASE("push")
    {
        auto timeline = std::make_shared<sight::data::raw_buffer_tl>();
        timeline->init_pool_size(3 * sizeof(float));

        sight::core::clock::type time1 = sight::core::clock::get_time_in_milli_sec();
        sight::core::clock::type time2 = time1 + 42;

        std::array values1 = {1.0F, 5.2F, 7.5F};
        std::array values2 = {8.0F, 9.0F, 66.F};
        SPTR(sight::data::timeline::raw_buffer) data1 = timeline->create_buffer(time1);
        std::ranges::copy(values1, data1->buffer<float>());
        SPTR(sight::data::timeline::raw_buffer) data2 = timeline->create_buffer(time2);
        std::ranges::copy(values2, data2->buffer<float>());

        timeline->push_object(data1);
        timeline->push_object(data2);

        CSPTR(sight::data::timeline::object) data_pushed1 = timeline->get_object(time1);
        CHECK(data1 == data_pushed1);

        CSPTR(sight::data::timeline::object) data_pushed2 = timeline->get_object(time2);
        CHECK(data2 == data_pushed2);

        CSPTR(sight::data::timeline::object) data_pushed1_bis = timeline->get_closest_object(time1 + 1.5);
        CSPTR(sight::data::timeline::raw_buffer) buff         =
            std::dynamic_pointer_cast<const sight::data::timeline::raw_buffer>(data_pushed1_bis);
        CHECK(buff);
        CHECK_EQ(buff, timeline->get_closest_buffer(time1 + 1.5));
        auto* buff_data = buff->buffer<float>();
        CHECK_EQ(1.0F, buff_data[0]);
        CHECK_EQ(5.2F, buff_data[1]);
        CHECK_EQ(7.5F, buff_data[2]);

        CSPTR(sight::data::timeline::object) data_pushed2_bis = timeline->get_newer_object();
        CHECK(data2 == data_pushed2_bis);

        sight::core::clock::type time2_pushed = timeline->get_newer_timestamp();
        CHECK_EQ(time2, time2_pushed);

        timeline->clear_timeline();
        CSPTR(sight::data::timeline::object) null_obj = timeline->get_newer_object();
        CHECK(null_obj == nullptr);
    }

//------------------------------------------------------------------------------

    TEST_CASE("get_object")
    {
        auto timeline = std::make_shared<sight::data::raw_buffer_tl>();
        timeline->init_pool_size(3 * sizeof(float));

        std::array values = {1.0F, 5.2F, 7.5F};

        SPTR(sight::data::timeline::raw_buffer) data1 = timeline->create_buffer(1);
        std::ranges::copy(values, data1->buffer<float>());
        SPTR(sight::data::timeline::raw_buffer) data2 = timeline->create_buffer(2);
        std::ranges::copy(values, data2->buffer<float>());
        SPTR(sight::data::timeline::raw_buffer) data3 = timeline->create_buffer(3);
        std::ranges::copy(values, data3->buffer<float>());
        SPTR(sight::data::timeline::raw_buffer) data4 = timeline->create_buffer(4);
        std::ranges::copy(values, data4->buffer<float>());

        timeline->push_object(data1);
        timeline->push_object(data2);
        timeline->push_object(data3);
        timeline->push_object(data4);

        CSPTR(sight::data::timeline::object) obj;

        obj = timeline->get_object(1);
        CHECK(data1 == obj);
        obj = timeline->get_closest_object(0.1);
        CHECK(data1 == obj);
        obj = timeline->get_closest_object(1.1);
        CHECK(data1 == obj);

        obj = timeline->get_object(2);
        CHECK(data2 == obj);
        obj = timeline->get_closest_object(1.8);
        CHECK(data2 == obj);
        obj = timeline->get_closest_object(2, sight::data::timeline::past);
        CHECK(data2 == obj);
        obj = timeline->get_closest_object(2, sight::data::timeline::future);
        CHECK(data2 == obj);
        obj = timeline->get_closest_object(2, sight::data::timeline::both);
        CHECK(data2 == obj);
        obj = timeline->get_closest_object(2.2);
        CHECK(data2 == obj);

        obj = timeline->get_object(3);
        CHECK(data3 == obj);
        obj = timeline->get_closest_object(2.6);
        CHECK(data3 == obj);
        obj = timeline->get_closest_object(3.1);
        CHECK(data3 == obj);

        obj = timeline->get_object(4);
        CHECK(data4 == obj);
        obj = timeline->get_closest_object(3.9);
        CHECK(data4 == obj);
        obj = timeline->get_closest_object(4.2);
        CHECK(data4 == obj);

        obj = timeline->get_closest_object(0.1, sight::data::timeline::past);
        CHECK(nullptr == obj);
        obj = timeline->get_closest_object(0.1, sight::data::timeline::future);
        CHECK(data1 == obj);

        obj = timeline->get_closest_object(1.1, sight::data::timeline::past);
        CHECK(data1 == obj);
        obj = timeline->get_closest_object(1.1, sight::data::timeline::future);
        CHECK(data2 == obj);

        obj = timeline->get_closest_object(1.9, sight::data::timeline::past);
        CHECK(data1 == obj);
        obj = timeline->get_closest_object(1.9, sight::data::timeline::future);
        CHECK(data2 == obj);

        obj = timeline->get_closest_object(2.1, sight::data::timeline::past);
        CHECK(data2 == obj);
        obj = timeline->get_closest_object(2.1, sight::data::timeline::future);
        CHECK(data3 == obj);

        obj = timeline->get_closest_object(2.8, sight::data::timeline::past);
        CHECK(data2 == obj);
        obj = timeline->get_closest_object(2.8, sight::data::timeline::future);
        CHECK(data3 == obj);

        obj = timeline->get_closest_object(3.1, sight::data::timeline::past);
        CHECK(data3 == obj);
        obj = timeline->get_closest_object(3.1, sight::data::timeline::future);
        CHECK(data4 == obj);

        obj = timeline->get_closest_object(3.9, sight::data::timeline::past);
        CHECK(data3 == obj);
        obj = timeline->get_closest_object(3.9, sight::data::timeline::future);
        CHECK(data4 == obj);

        obj = timeline->get_closest_object(4.1, sight::data::timeline::past);
        CHECK(data4 == obj);
        obj = timeline->get_closest_object(4.1, sight::data::timeline::future);
        CHECK(nullptr == obj);
    }

//------------------------------------------------------------------------------

    TEST_CASE("set_object")
    {
        auto timeline = std::make_shared<sight::data::raw_buffer_tl>();
        timeline->init_pool_size(3 * sizeof(float));

        std::array values = {1.0F, 5.2F, 7.5F};

        SPTR(sight::data::timeline::raw_buffer) data1 = timeline->create_buffer(1);
        std::ranges::copy(values, data1->buffer<float>());
        SPTR(sight::data::timeline::raw_buffer) data2 = timeline->create_buffer(2);
        std::ranges::copy(values, data2->buffer<float>());
        SPTR(sight::data::timeline::raw_buffer) data3 = timeline->create_buffer(3);
        std::ranges::copy(values, data3->buffer<float>());
        SPTR(sight::data::timeline::raw_buffer) data4 = timeline->create_buffer(4);
        std::ranges::copy(values, data4->buffer<float>());

        timeline->push_object(data1);
        timeline->push_object(data2);
        timeline->push_object(data3);
        timeline->push_object(data4);

        CSPTR(sight::data::timeline::object) obj;

        timeline->set_object(1, data2);
        timeline->set_object(2, data3);
        timeline->set_object(4, data3);

        obj = timeline->get_object(1);
        CHECK(data2 == obj);

        obj = timeline->get_closest_object(2.2);
        CHECK(data3 == obj);

        obj = timeline->get_closest_object(3.8);
        CHECK(data3 == obj);

        obj = timeline->get_object(3);
        CHECK(data3 == obj);

        timeline->set_object(3, data1);

        obj = timeline->get_object(3);
        CHECK(data1 == obj);
    }

//------------------------------------------------------------------------------

    TEST_CASE("modify_time")
    {
        auto timeline = std::make_shared<sight::data::raw_buffer_tl>();
        timeline->init_pool_size(3 * sizeof(float));

        std::array values = {2.0F, 1.2F, 6.5F};

        SPTR(sight::data::timeline::raw_buffer) data1 = timeline->create_buffer(1);
        std::ranges::copy(values, data1->buffer<float>());
        SPTR(sight::data::timeline::raw_buffer) data2 = timeline->create_buffer(2);
        std::ranges::copy(values, data2->buffer<float>());
        SPTR(sight::data::timeline::raw_buffer) data3 = timeline->create_buffer(3);
        std::ranges::copy(values, data3->buffer<float>());
        SPTR(sight::data::timeline::raw_buffer) data4 = timeline->create_buffer(4);
        std::ranges::copy(values, data4->buffer<float>());

        timeline->push_object(data1);
        timeline->push_object(data2);
        timeline->push_object(data3);
        timeline->push_object(data4);

        CSPTR(sight::data::timeline::object) obj;

        obj = timeline->get_object(1);
        CHECK(data1 == obj);
        obj = timeline->get_closest_object(0.1);
        CHECK(data1 == obj);
        obj = timeline->get_closest_object(1.1);
        CHECK(data1 == obj);

        obj = timeline->get_object(2);
        CHECK(data2 == obj);
        obj = timeline->get_closest_object(4.1);
        CHECK(data4 == obj);

        timeline->modify_time(1, 5);
        timeline->modify_time(2, 1);
        timeline->modify_time(5, 6);
        timeline->modify_time(3, 7);

        obj = timeline->get_object(1);
        CHECK(data2 == obj);

        obj = timeline->get_closest_object(6.2);
        CHECK(data1 == obj);

        obj = timeline->get_closest_object(3.1);
        CHECK(data4 == obj);

        obj = timeline->get_object(7);
        CHECK(data3 == obj);
    }

//------------------------------------------------------------------------------

    TEST_CASE("copy")
    {
        auto timeline = std::make_shared<sight::data::raw_buffer_tl>();
        timeline->init_pool_size(3 * sizeof(float));

        sight::core::clock::type time1 = sight::core::clock::get_time_in_milli_sec();
        sight::core::clock::type time2 = time1 + 42;

        std::array values1 = {1.0F, 5.2F, 7.5F};
        std::array values2 = {8.0F, 9.0F, 66.F};
        SPTR(sight::data::timeline::raw_buffer) data1 = timeline->create_buffer(time1);
        std::ranges::copy(values1, data1->buffer<float>());
        SPTR(sight::data::timeline::raw_buffer) data2 = timeline->create_buffer(time2);
        std::ranges::copy(values2, data2->buffer<float>());

        timeline->push_object(data1);
        timeline->push_object(data2);

        // deep_copy test
        auto deep_timeline = std::make_shared<sight::data::raw_buffer_tl>();
        CHECK(*timeline != *deep_timeline);

        deep_timeline->deep_copy(timeline);
        CHECK(*timeline == *deep_timeline);

        CSPTR(sight::data::timeline::object) deep_data_pushed1 = deep_timeline->get_object(time1);
        CHECK(deep_data_pushed1);
        CHECK(data1 != deep_data_pushed1);
        CSPTR(sight::data::timeline::raw_buffer) buff1 =
            std::dynamic_pointer_cast<const sight::data::timeline::raw_buffer>(deep_data_pushed1);
        CHECK(buff1);
        CHECK_EQ(buff1, deep_timeline->get_buffer(time1));
        auto* buff_data1 = buff1->buffer<float>();
        CHECK_EQ(1.0F, buff_data1[0]);
        CHECK_EQ(5.2F, buff_data1[1]);
        CHECK_EQ(7.5F, buff_data1[2]);

        CSPTR(sight::data::timeline::object) deep_data_pushed2 = deep_timeline->get_object(time2);
        CHECK(deep_data_pushed2);
        CHECK(data2 != deep_data_pushed2);
        CSPTR(sight::data::timeline::raw_buffer) buff2 =
            std::dynamic_pointer_cast<const sight::data::timeline::raw_buffer>(deep_data_pushed2);
        CHECK(buff2);
        auto* buff_data2 = buff2->buffer<float>();
        CHECK_EQ(8.0F, buff_data2[0]);
        CHECK_EQ(9.0F, buff_data2[1]);
        CHECK_EQ(66.F, buff_data2[2]);
    }
} // TEST_SUITE("sight::data::raw_buffer_tl")
