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

#include "raw_buffer_tl_test.hpp"

#include <data/raw_buffer_tl.hpp>
#include <data/timeline/buffer.hpp>

#include <utest/exception.hpp>

#include <array>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::raw_buffer_tl_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void raw_buffer_tl_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void raw_buffer_tl_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void raw_buffer_tl_test::push_test()
{
    auto timeline = std::make_shared<data::raw_buffer_tl>();
    timeline->init_pool_size(3 * sizeof(float));

    core::clock::type time1 = core::clock::get_time_in_milli_sec();
    core::clock::type time2 = time1 + 42;

    std::array values1 = {1.0F, 5.2F, 7.5F};
    std::array values2 = {8.0F, 9.0F, 66.F};
    SPTR(data::timeline::raw_buffer) data1 = timeline->create_buffer(time1);
    std::copy(values1.begin(), values1.end(), data1->buffer<float>());
    SPTR(data::timeline::raw_buffer) data2 = timeline->create_buffer(time2);
    std::copy(values2.begin(), values2.end(), data2->buffer<float>());

    timeline->push_object(data1);
    timeline->push_object(data2);

    CSPTR(data::timeline::object) data_pushed1 = timeline->get_object(time1);
    CPPUNIT_ASSERT(data1 == data_pushed1);

    CSPTR(data::timeline::object) data_pushed2 = timeline->get_object(time2);
    CPPUNIT_ASSERT(data2 == data_pushed2);

    CSPTR(data::timeline::object) data_pushed1_bis = timeline->get_closest_object(time1 + 1.5);
    CSPTR(data::timeline::raw_buffer) buff         =
        std::dynamic_pointer_cast<const data::timeline::raw_buffer>(data_pushed1_bis);
    CPPUNIT_ASSERT(buff);
    CPPUNIT_ASSERT_EQUAL(buff, timeline->get_closest_buffer(time1 + 1.5));
    auto* buff_data = buff->buffer<float>();
    CPPUNIT_ASSERT_EQUAL(1.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(5.2F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(7.5F, buff_data[2]);

    CSPTR(data::timeline::object) data_pushed2_bis = timeline->get_newer_object();
    CPPUNIT_ASSERT(data2 == data_pushed2_bis);

    core::clock::type time2_pushed = timeline->get_newer_timestamp();
    CPPUNIT_ASSERT_EQUAL(time2, time2_pushed);

    timeline->clear_timeline();
    CSPTR(data::timeline::object) null_obj = timeline->get_newer_object();
    CPPUNIT_ASSERT(null_obj == nullptr);
}

//------------------------------------------------------------------------------

void raw_buffer_tl_test::get_object_test()
{
    auto timeline = std::make_shared<data::raw_buffer_tl>();
    timeline->init_pool_size(3 * sizeof(float));

    std::array values = {1.0F, 5.2F, 7.5F};

    SPTR(data::timeline::raw_buffer) data1 = timeline->create_buffer(1);
    std::copy(values.begin(), values.end(), data1->buffer<float>());
    SPTR(data::timeline::raw_buffer) data2 = timeline->create_buffer(2);
    std::copy(values.begin(), values.end(), data2->buffer<float>());
    SPTR(data::timeline::raw_buffer) data3 = timeline->create_buffer(3);
    std::copy(values.begin(), values.end(), data3->buffer<float>());
    SPTR(data::timeline::raw_buffer) data4 = timeline->create_buffer(4);
    std::copy(values.begin(), values.end(), data4->buffer<float>());

    timeline->push_object(data1);
    timeline->push_object(data2);
    timeline->push_object(data3);
    timeline->push_object(data4);

    CSPTR(data::timeline::object) obj;

    obj = timeline->get_object(1);
    CPPUNIT_ASSERT(data1 == obj);
    obj = timeline->get_closest_object(0.1);
    CPPUNIT_ASSERT(data1 == obj);
    obj = timeline->get_closest_object(1.1);
    CPPUNIT_ASSERT(data1 == obj);

    obj = timeline->get_object(2);
    CPPUNIT_ASSERT(data2 == obj);
    obj = timeline->get_closest_object(1.8);
    CPPUNIT_ASSERT(data2 == obj);
    obj = timeline->get_closest_object(2, data::timeline::past);
    CPPUNIT_ASSERT(data2 == obj);
    obj = timeline->get_closest_object(2, data::timeline::future);
    CPPUNIT_ASSERT(data2 == obj);
    obj = timeline->get_closest_object(2, data::timeline::both);
    CPPUNIT_ASSERT(data2 == obj);
    obj = timeline->get_closest_object(2.2);
    CPPUNIT_ASSERT(data2 == obj);

    obj = timeline->get_object(3);
    CPPUNIT_ASSERT(data3 == obj);
    obj = timeline->get_closest_object(2.6);
    CPPUNIT_ASSERT(data3 == obj);
    obj = timeline->get_closest_object(3.1);
    CPPUNIT_ASSERT(data3 == obj);

    obj = timeline->get_object(4);
    CPPUNIT_ASSERT(data4 == obj);
    obj = timeline->get_closest_object(3.9);
    CPPUNIT_ASSERT(data4 == obj);
    obj = timeline->get_closest_object(4.2);
    CPPUNIT_ASSERT(data4 == obj);

    obj = timeline->get_closest_object(0.1, data::timeline::past);
    CPPUNIT_ASSERT(nullptr == obj);
    obj = timeline->get_closest_object(0.1, data::timeline::future);
    CPPUNIT_ASSERT(data1 == obj);

    obj = timeline->get_closest_object(1.1, data::timeline::past);
    CPPUNIT_ASSERT(data1 == obj);
    obj = timeline->get_closest_object(1.1, data::timeline::future);
    CPPUNIT_ASSERT(data2 == obj);

    obj = timeline->get_closest_object(1.9, data::timeline::past);
    CPPUNIT_ASSERT(data1 == obj);
    obj = timeline->get_closest_object(1.9, data::timeline::future);
    CPPUNIT_ASSERT(data2 == obj);

    obj = timeline->get_closest_object(2.1, data::timeline::past);
    CPPUNIT_ASSERT(data2 == obj);
    obj = timeline->get_closest_object(2.1, data::timeline::future);
    CPPUNIT_ASSERT(data3 == obj);

    obj = timeline->get_closest_object(2.8, data::timeline::past);
    CPPUNIT_ASSERT(data2 == obj);
    obj = timeline->get_closest_object(2.8, data::timeline::future);
    CPPUNIT_ASSERT(data3 == obj);

    obj = timeline->get_closest_object(3.1, data::timeline::past);
    CPPUNIT_ASSERT(data3 == obj);
    obj = timeline->get_closest_object(3.1, data::timeline::future);
    CPPUNIT_ASSERT(data4 == obj);

    obj = timeline->get_closest_object(3.9, data::timeline::past);
    CPPUNIT_ASSERT(data3 == obj);
    obj = timeline->get_closest_object(3.9, data::timeline::future);
    CPPUNIT_ASSERT(data4 == obj);

    obj = timeline->get_closest_object(4.1, data::timeline::past);
    CPPUNIT_ASSERT(data4 == obj);
    obj = timeline->get_closest_object(4.1, data::timeline::future);
    CPPUNIT_ASSERT(nullptr == obj);
}

//------------------------------------------------------------------------------

void raw_buffer_tl_test::set_object_test()
{
    auto timeline = std::make_shared<data::raw_buffer_tl>();
    timeline->init_pool_size(3 * sizeof(float));

    std::array values = {1.0F, 5.2F, 7.5F};

    SPTR(data::timeline::raw_buffer) data1 = timeline->create_buffer(1);
    std::copy(values.begin(), values.end(), data1->buffer<float>());
    SPTR(data::timeline::raw_buffer) data2 = timeline->create_buffer(2);
    std::copy(values.begin(), values.end(), data2->buffer<float>());
    SPTR(data::timeline::raw_buffer) data3 = timeline->create_buffer(3);
    std::copy(values.begin(), values.end(), data3->buffer<float>());
    SPTR(data::timeline::raw_buffer) data4 = timeline->create_buffer(4);
    std::copy(values.begin(), values.end(), data4->buffer<float>());

    timeline->push_object(data1);
    timeline->push_object(data2);
    timeline->push_object(data3);
    timeline->push_object(data4);

    CSPTR(data::timeline::object) obj;

    timeline->set_object(1, data2);
    timeline->set_object(2, data3);
    timeline->set_object(4, data3);

    obj = timeline->get_object(1);
    CPPUNIT_ASSERT(data2 == obj);

    obj = timeline->get_closest_object(2.2);
    CPPUNIT_ASSERT(data3 == obj);

    obj = timeline->get_closest_object(3.8);
    CPPUNIT_ASSERT(data3 == obj);

    obj = timeline->get_object(3);
    CPPUNIT_ASSERT(data3 == obj);

    timeline->set_object(3, data1);

    obj = timeline->get_object(3);
    CPPUNIT_ASSERT(data1 == obj);
}

//------------------------------------------------------------------------------

void raw_buffer_tl_test::modify_time_test()
{
    auto timeline = std::make_shared<data::raw_buffer_tl>();
    timeline->init_pool_size(3 * sizeof(float));

    std::array values = {2.0F, 1.2F, 6.5F};

    SPTR(data::timeline::raw_buffer) data1 = timeline->create_buffer(1);
    std::copy(values.begin(), values.end(), data1->buffer<float>());
    SPTR(data::timeline::raw_buffer) data2 = timeline->create_buffer(2);
    std::copy(values.begin(), values.end(), data2->buffer<float>());
    SPTR(data::timeline::raw_buffer) data3 = timeline->create_buffer(3);
    std::copy(values.begin(), values.end(), data3->buffer<float>());
    SPTR(data::timeline::raw_buffer) data4 = timeline->create_buffer(4);
    std::copy(values.begin(), values.end(), data4->buffer<float>());

    timeline->push_object(data1);
    timeline->push_object(data2);
    timeline->push_object(data3);
    timeline->push_object(data4);

    CSPTR(data::timeline::object) obj;

    obj = timeline->get_object(1);
    CPPUNIT_ASSERT(data1 == obj);
    obj = timeline->get_closest_object(0.1);
    CPPUNIT_ASSERT(data1 == obj);
    obj = timeline->get_closest_object(1.1);
    CPPUNIT_ASSERT(data1 == obj);

    obj = timeline->get_object(2);
    CPPUNIT_ASSERT(data2 == obj);
    obj = timeline->get_closest_object(4.1);
    CPPUNIT_ASSERT(data4 == obj);

    timeline->modify_time(1, 5);
    timeline->modify_time(2, 1);
    timeline->modify_time(5, 6);
    timeline->modify_time(3, 7);

    obj = timeline->get_object(1);
    CPPUNIT_ASSERT(data2 == obj);

    obj = timeline->get_closest_object(6.2);
    CPPUNIT_ASSERT(data1 == obj);

    obj = timeline->get_closest_object(3.1);
    CPPUNIT_ASSERT(data4 == obj);

    obj = timeline->get_object(7);
    CPPUNIT_ASSERT(data3 == obj);
}

//------------------------------------------------------------------------------

void raw_buffer_tl_test::copy_test()
{
    auto timeline = std::make_shared<data::raw_buffer_tl>();
    timeline->init_pool_size(3 * sizeof(float));

    core::clock::type time1 = core::clock::get_time_in_milli_sec();
    core::clock::type time2 = time1 + 42;

    std::array values1 = {1.0F, 5.2F, 7.5F};
    std::array values2 = {8.0F, 9.0F, 66.F};
    SPTR(data::timeline::raw_buffer) data1 = timeline->create_buffer(time1);
    std::copy(values1.begin(), values1.end(), data1->buffer<float>());
    SPTR(data::timeline::raw_buffer) data2 = timeline->create_buffer(time2);
    std::copy(values2.begin(), values2.end(), data2->buffer<float>());

    timeline->push_object(data1);
    timeline->push_object(data2);

    // deep_copy test
    auto deep_timeline = std::make_shared<data::raw_buffer_tl>();
    CPPUNIT_ASSERT(*timeline != *deep_timeline);

    deep_timeline->deep_copy(timeline);
    CPPUNIT_ASSERT(*timeline == *deep_timeline);

    CSPTR(data::timeline::object) deep_data_pushed1 = deep_timeline->get_object(time1);
    CPPUNIT_ASSERT(deep_data_pushed1);
    CPPUNIT_ASSERT(data1 != deep_data_pushed1);
    CSPTR(data::timeline::raw_buffer) buff1 =
        std::dynamic_pointer_cast<const data::timeline::raw_buffer>(deep_data_pushed1);
    CPPUNIT_ASSERT(buff1);
    CPPUNIT_ASSERT_EQUAL(buff1, deep_timeline->get_buffer(time1));
    auto* buff_data1 = buff1->buffer<float>();
    CPPUNIT_ASSERT_EQUAL(1.0F, buff_data1[0]);
    CPPUNIT_ASSERT_EQUAL(5.2F, buff_data1[1]);
    CPPUNIT_ASSERT_EQUAL(7.5F, buff_data1[2]);

    CSPTR(data::timeline::object) deep_data_pushed2 = deep_timeline->get_object(time2);
    CPPUNIT_ASSERT(deep_data_pushed2);
    CPPUNIT_ASSERT(data2 != deep_data_pushed2);
    CSPTR(data::timeline::raw_buffer) buff2 =
        std::dynamic_pointer_cast<const data::timeline::raw_buffer>(deep_data_pushed2);
    CPPUNIT_ASSERT(buff2);
    auto* buff_data2 = buff2->buffer<float>();
    CPPUNIT_ASSERT_EQUAL(8.0F, buff_data2[0]);
    CPPUNIT_ASSERT_EQUAL(9.0F, buff_data2[1]);
    CPPUNIT_ASSERT_EQUAL(66.F, buff_data2[2]);
}

} // namespace sight::data::ut
