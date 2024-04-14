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

#include "generic_tl_test.hpp"

#include <data/generic_tl.hpp>
#include <data/generic_tl.hxx>
#include <data/registry/macros.hpp>
#include <data/timeline/generic_object.hpp>
#include <data/timeline/generic_object.hxx>

#include <utest/exception.hpp>

#include <array>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::generic_tl_test);

namespace sight::data
{

//------------------------------------------------------------------------------

class float3_tl : public generic_tl<std::array<float,
                                               3> >
{
public:

    SIGHT_DECLARE_CLASS(float3_tl, data::timeline::base);
};
SIGHT_REGISTER_DATA(sight::data::float3_tl)

using float4 = std::array<float, 4>;

//------------------------------------------------------------------------------

class float4_tl : public generic_tl<float4>
{
public:

    SIGHT_DECLARE_CLASS(float4_tl, data::timeline::base);
};
SIGHT_REGISTER_DATA(sight::data::float4_tl)

//------------------------------------------------------------------------------

class test_contained
{
public:

    int m_int {};
    std::array<double, 2> m_floats {};
};

class test_class_tl : public generic_tl<test_contained>
{
public:

    SIGHT_DECLARE_CLASS(test_class_tl, data::timeline::base);
};
SIGHT_REGISTER_DATA(sight::data::test_class_tl)

namespace ut
{

//------------------------------------------------------------------------------

void generic_tl_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void generic_tl_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void generic_tl_test::push_pop_test()
{
    data::float4_tl::sptr timeline = std::make_shared<data::float4_tl>();
    timeline->init_pool_size(3);

    core::clock::type time1 = core::clock::get_time_in_milli_sec();
    core::clock::type time2 = time1 + 42;
    core::clock::type time3 = time1 + 81;

    float4 values1 = {1.0F, 5.2F, 7.5F, 1.F};
    float4 values2 = {4.0F, 5.5F, 1.5F, 2.F};
    float4 values3 = {1.0F, 3.2F, 2.5F, 0.F};

    float4 values4 = {-1.0F, 1.1F, 0.5F, -1.F};
    float4 values6 = {2.0F, 2.2F, -2.9F, 0.2F};

    // All elements set
    SPTR(data::float4_tl::buffer_t) data1 = timeline->create_buffer(time1);
    data1->set_element(values1, 0);
    data1->set_element(values2, 1);
    data1->set_element(values3, 2);
    // Stupid but we need to test that
    data1->set_element(values3, 2);
    data1->set_element(values3, 2);
    data1->set_element(values1, 1);
    data1->set_element(values2, 1);

    // Second element missing
    SPTR(data::float4_tl::buffer_t) data2 = timeline->create_buffer(time2);
    data2->set_element(values3, 0);
    data2->set_element(values4, 1);
    data2->set_element(values1, 2);

    SPTR(data::float4_tl::buffer_t) data3 = timeline->create_buffer(time3);
    data3->set_element(values4, 0);

    // Another way to set element
    data3->add_element(2);
    float4* new_values = data3->add_element(2);
    std::copy(values6.begin(), values6.end(), new_values->begin());

    timeline->push_object(data1);
    timeline->push_object(data2);
    timeline->push_object(data3);

    CPPUNIT_ASSERT(data1->get_timestamp() == time1);
    CPPUNIT_ASSERT(data2->get_timestamp() == time2);
    CPPUNIT_ASSERT(data3->get_timestamp() == time3);

    ////////////////////////////////////////////////////////////////////////////
    /// Check first data with all elements set
    {
        CSPTR(data::timeline::object) data_pushed1 = timeline->get_object(time1);
        CPPUNIT_ASSERT(data1 == data_pushed1);
        CPPUNIT_ASSERT(data_pushed1->get_timestamp() == time1);

        CSPTR(data::timeline::object) data_pushed2 = timeline->get_object(time2);
        CPPUNIT_ASSERT(data2 == data_pushed2);
        CPPUNIT_ASSERT(data_pushed2->get_timestamp() == time2);

        CSPTR(data::timeline::object) data_pushed3 = timeline->get_object(time3);
        CPPUNIT_ASSERT(data3 == data_pushed3);
        CPPUNIT_ASSERT(data_pushed3->get_timestamp() == time3);

        CSPTR(data::timeline::object) data_pushed1_bis = timeline->get_closest_object(time1 + 1.5);
        CSPTR(data::float4_tl::buffer_t) obj           =
            std::dynamic_pointer_cast<const data::float4_tl::buffer_t>(data_pushed1_bis);
        CPPUNIT_ASSERT(obj);
        CPPUNIT_ASSERT_EQUAL(obj, timeline->get_closest_buffer(time1 + 1.5));

        CPPUNIT_ASSERT_EQUAL(3U, obj->get_present_element_num());
        CPPUNIT_ASSERT_EQUAL(std::size_t(16), obj->get_element_size());
        CPPUNIT_ASSERT_EQUAL(3U, obj->get_max_element_num());
        CPPUNIT_ASSERT_EQUAL(true, obj->is_present(0));
        CPPUNIT_ASSERT_EQUAL(true, obj->is_present(1));
        CPPUNIT_ASSERT_EQUAL(true, obj->is_present(2));
        CPPUNIT_ASSERT_EQUAL(uint64_t(7), obj->get_mask());

        float4 buff_data = obj->get_element(0);
        CPPUNIT_ASSERT_EQUAL(1.0F, buff_data[0]);
        CPPUNIT_ASSERT_EQUAL(5.2F, buff_data[1]);
        CPPUNIT_ASSERT_EQUAL(7.5F, buff_data[2]);
        CPPUNIT_ASSERT_EQUAL(1.0F, buff_data[3]);

        buff_data = obj->get_element(1);
        CPPUNIT_ASSERT_EQUAL(4.0F, buff_data[0]);
        CPPUNIT_ASSERT_EQUAL(5.5F, buff_data[1]);
        CPPUNIT_ASSERT_EQUAL(1.5F, buff_data[2]);
        CPPUNIT_ASSERT_EQUAL(2.0F, buff_data[3]);

        buff_data = obj->get_element(2);
        CPPUNIT_ASSERT_EQUAL(1.0F, buff_data[0]);
        CPPUNIT_ASSERT_EQUAL(3.2F, buff_data[1]);
        CPPUNIT_ASSERT_EQUAL(2.5F, buff_data[2]);
        CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[3]);
    }

    ////////////////////////////////////////////////////////////////////////////
    /// Check second data with one missing element
    {
        CSPTR(data::timeline::object) data_pushed3_bis = timeline->get_newer_object();
        CPPUNIT_ASSERT(data3 == data_pushed3_bis);

        core::clock::type time3_pushed = timeline->get_newer_timestamp();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(time3, time3_pushed, 0.00001);

        CSPTR(data::float4_tl::buffer_t) obj =
            std::dynamic_pointer_cast<const data::float4_tl::buffer_t>(data_pushed3_bis);
        CPPUNIT_ASSERT(obj);

        CPPUNIT_ASSERT_EQUAL(2U, obj->get_present_element_num());
        CPPUNIT_ASSERT_EQUAL(std::size_t(16), obj->get_element_size());
        CPPUNIT_ASSERT_EQUAL(3U, obj->get_max_element_num());
        CPPUNIT_ASSERT_EQUAL(true, obj->is_present(0));
        CPPUNIT_ASSERT_EQUAL(false, obj->is_present(1));
        CPPUNIT_ASSERT_EQUAL(true, obj->is_present(2));
        CPPUNIT_ASSERT_EQUAL(uint64_t(5), obj->get_mask());

        float4 buff_data = obj->get_element(0);
        CPPUNIT_ASSERT_EQUAL(-1.F, buff_data[0]);
        CPPUNIT_ASSERT_EQUAL(1.1F, buff_data[1]);
        CPPUNIT_ASSERT_EQUAL(0.5F, buff_data[2]);
        CPPUNIT_ASSERT_EQUAL(-1.F, buff_data[3]);

        buff_data = obj->get_element(1);
        CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[0]);
        CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[1]);
        CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[2]);
        CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[3]);

        buff_data = obj->get_element(2);
        CPPUNIT_ASSERT_EQUAL(2.0F, buff_data[0]);
        CPPUNIT_ASSERT_EQUAL(2.2F, buff_data[1]);
        CPPUNIT_ASSERT_EQUAL(-2.9F, buff_data[2]);
        CPPUNIT_ASSERT_EQUAL(0.2F, buff_data[3]);
    }

    ////////////////////////////////////////////////////////////////////////////
    /// Check pop method
    {
        CSPTR(data::timeline::object) data_pushed2_bis = timeline->get_closest_object(time2);
        CPPUNIT_ASSERT(data2 == data_pushed2_bis);

        CSPTR(data::timeline::object) data_popped2 = timeline->pop_object(time2);
        CPPUNIT_ASSERT(data2 == data_popped2);

        CSPTR(data::timeline::object) data_pushed3_bis = timeline->get_closest_object(time2);
        CPPUNIT_ASSERT(data3 == data_pushed3_bis);

        timeline->push_object(data2);

        data_pushed2_bis = timeline->get_closest_object(time2);
        CPPUNIT_ASSERT(data2 == data_pushed2_bis);

        data_pushed3_bis = timeline->get_closest_object(time3);
        CPPUNIT_ASSERT(data3 == data_pushed3_bis);

        CSPTR(data::timeline::object) data_popped3 = timeline->pop_object(time3);
        CPPUNIT_ASSERT(data3 == data_popped3);

        data_pushed2_bis = timeline->get_closest_object(time3);
        CPPUNIT_ASSERT(data2 == data_pushed2_bis);

        data_pushed2_bis = timeline->get_newer_object();
        CPPUNIT_ASSERT(data2 == data_pushed2_bis);

        CSPTR(data::timeline::object) data_pushed1_bis = timeline->get_closest_object(time1);
        CPPUNIT_ASSERT(data1 == data_pushed1_bis);
    }

    timeline->clear_timeline();
    CSPTR(data::timeline::object) null_obj = timeline->get_newer_object();
    CPPUNIT_ASSERT(null_obj == nullptr);
}

//------------------------------------------------------------------------------

void generic_tl_test::push_class_test()
{
    data::test_class_tl::sptr timeline = std::make_shared<data::test_class_tl>();
    timeline->init_pool_size(3);

    core::clock::type time1 = core::clock::get_time_in_milli_sec();
    core::clock::type time2 = time1 + 42;

    test_contained test_data1;
    test_data1.m_int       = 87;
    test_data1.m_floats[0] = 12.79;
    test_data1.m_floats[1] = -459.59F;

    test_contained test_data2;
    test_data2.m_int       = -45471;
    test_data2.m_floats[0] = 68.49;
    test_data2.m_floats[1] = -9.994F;

    test_contained test_data3;
    test_data3.m_int       = 447471;
    test_data3.m_floats[0] = 6822.49;
    test_data3.m_floats[1] = -92194.47F;

    // All elements set
    SPTR(data::test_class_tl::buffer_t) data1 = timeline->create_buffer(time1);
    data1->set_element(test_data1, 1);
    data1->set_element(test_data2, 2);

    // Second element missing
    SPTR(data::test_class_tl::buffer_t) data2 = timeline->create_buffer(time2);
    data2->set_element(test_data3, 2);

    timeline->push_object(data1);
    timeline->push_object(data2);

    ////////////////////////////////////////////////////////////////////////////
    /// Check first data

    CSPTR(data::test_class_tl::buffer_t) data_pushed1 = timeline->get_buffer(time1);
    CPPUNIT_ASSERT(data_pushed1);

    CPPUNIT_ASSERT_EQUAL(2U, data_pushed1->get_present_element_num());
    CPPUNIT_ASSERT_EQUAL(sizeof(test_contained), data_pushed1->get_element_size());
    CPPUNIT_ASSERT_EQUAL(3U, data_pushed1->get_max_element_num());
    CPPUNIT_ASSERT_EQUAL(false, data_pushed1->is_present(0));
    CPPUNIT_ASSERT_EQUAL(true, data_pushed1->is_present(1));
    CPPUNIT_ASSERT_EQUAL(true, data_pushed1->is_present(2));
    CPPUNIT_ASSERT_EQUAL(uint64_t(6), data_pushed1->get_mask());

    {
        const test_contained& test_data = data_pushed1->get_element(1);
        CPPUNIT_ASSERT_EQUAL(test_data1.m_int, test_data.m_int);
        CPPUNIT_ASSERT_EQUAL(test_data1.m_floats[0], test_data.m_floats[0]);
        CPPUNIT_ASSERT_EQUAL(test_data1.m_floats[1], test_data.m_floats[1]);
    }
    {
        const test_contained& test_data = data_pushed1->get_element(2);
        CPPUNIT_ASSERT_EQUAL(test_data2.m_int, test_data.m_int);
        CPPUNIT_ASSERT_EQUAL(test_data2.m_floats[0], test_data.m_floats[0]);
        CPPUNIT_ASSERT_EQUAL(test_data2.m_floats[1], test_data.m_floats[1]);
    }

    ////////////////////////////////////////////////////////////////////////////
    /// Check second data

    CSPTR(data::test_class_tl::buffer_t) data_pushed2 = timeline->get_buffer(time2);
    CPPUNIT_ASSERT(data_pushed2);

    CPPUNIT_ASSERT_EQUAL(1U, data_pushed2->get_present_element_num());
    CPPUNIT_ASSERT_EQUAL(sizeof(test_contained), data_pushed2->get_element_size());
    CPPUNIT_ASSERT_EQUAL(3U, data_pushed2->get_max_element_num());
    CPPUNIT_ASSERT_EQUAL(false, data_pushed2->is_present(0));
    CPPUNIT_ASSERT_EQUAL(false, data_pushed2->is_present(1));
    CPPUNIT_ASSERT_EQUAL(true, data_pushed2->is_present(2));
    CPPUNIT_ASSERT_EQUAL(uint64_t(4), data_pushed2->get_mask());

    {
        const test_contained& test_data = data_pushed2->get_element(2);
        CPPUNIT_ASSERT_EQUAL(test_data3.m_int, test_data.m_int);
        CPPUNIT_ASSERT_EQUAL(test_data3.m_floats[0], test_data.m_floats[0]);
        CPPUNIT_ASSERT_EQUAL(test_data3.m_floats[1], test_data.m_floats[1]);
    }
}

//------------------------------------------------------------------------------

void generic_tl_test::copy_test()
{
    data::float3_tl::sptr timeline = std::make_shared<data::float3_tl>();
    timeline->init_pool_size(3);

    core::clock::type time1  = core::clock::get_time_in_milli_sec();
    core::clock::type time2  = time1 + 42;
    core::clock::type time2b = time2 + 12;
    core::clock::type time3  = time2 + 52;
    core::clock::type time4  = time3 + 52;
    core::clock::type time4b = time4 + 12;

    std::array<float, 3> values1 = {1.0F, 5.2F, 7.5F};
    std::array<float, 3> values2 = {4.0F, 5.5F, 1.5F};
    std::array<float, 3> values3 = {1.0F, 3.2F, 2.5F};
    std::array<float, 3> values4 = {2.0F, -.2F, 2.5F};
    std::array<float, 3> values5 = {8.0F, 9.0F, 66.F};
    std::array<float, 3> values6 = {2.0F, 1.2F, 11.F};

    SPTR(data::float3_tl::buffer_t) data1 = timeline->create_buffer(time1);
    data1->set_element(values1, 0);
    CPPUNIT_ASSERT(data1->get_timestamp() == time1);

    SPTR(data::float3_tl::buffer_t) data2 = timeline->create_buffer(time2);
    data2->set_element(values2, 0);
    data2->set_element(values3, 1);
    data2->set_element(values4, 2);
    CPPUNIT_ASSERT(data2->get_timestamp() == time2);

    SPTR(data::float3_tl::buffer_t) data2b = timeline->create_buffer(time2b);
    data2b->set_element(values1, 0);
    data2b->set_element(values4, 1);
    CPPUNIT_ASSERT(data2b->get_timestamp() == time2b);

    SPTR(data::float3_tl::buffer_t) data3 = timeline->create_buffer(time3);
    data3->set_element(values5, 1);
    data3->set_element(values6, 2);
    CPPUNIT_ASSERT(data3->get_timestamp() == time3);

    SPTR(data::float3_tl::buffer_t) data4 = timeline->create_buffer(time4);
    CPPUNIT_ASSERT(data4->get_timestamp() == time4);

    SPTR(data::float3_tl::buffer_t) data4b = timeline->create_buffer(time4b);
    data4b->set_element(values5, 0);
    data4b->set_element(values3, 2);
    CPPUNIT_ASSERT(data4b->get_timestamp() == time4b);

    timeline->push_object(data1);
    timeline->push_object(data2);
    timeline->push_object(data2b);
    timeline->push_object(data3);
    timeline->push_object(data4);
    timeline->push_object(data4b);

    // remove some objects
    timeline->pop_object(time2b);
    timeline->pop_object(time4b);

    // deep_copy test
    data::float3_tl::sptr deep_timeline = std::make_shared<data::float3_tl>();
    deep_timeline->deep_copy(timeline);
    timeline.reset();

    CSPTR(data::timeline::object) deep_data_pushed1 = deep_timeline->get_object(time1);
    CPPUNIT_ASSERT(deep_data_pushed1);
    CPPUNIT_ASSERT(data1 != deep_data_pushed1);
    CSPTR(data::float3_tl::buffer_t) obj1 =
        std::dynamic_pointer_cast<const data::float3_tl::buffer_t>(deep_data_pushed1);
    CPPUNIT_ASSERT(obj1);
    CPPUNIT_ASSERT_EQUAL(obj1, deep_timeline->get_buffer(time1));

    /// First element
    CPPUNIT_ASSERT_EQUAL(1U, obj1->get_present_element_num());
    CPPUNIT_ASSERT_EQUAL(std::size_t(12), obj1->get_element_size());
    CPPUNIT_ASSERT_EQUAL(3U, obj1->get_max_element_num());
    CPPUNIT_ASSERT_EQUAL(true, obj1->is_present(0));
    CPPUNIT_ASSERT_EQUAL(false, obj1->is_present(1));
    CPPUNIT_ASSERT_EQUAL(false, obj1->is_present(2));
    CPPUNIT_ASSERT_EQUAL(uint64_t(1), obj1->get_mask());
    CPPUNIT_ASSERT(obj1->get_timestamp() == time1);

    std::array<float, 3> buff_data = obj1->get_element(0);
    CPPUNIT_ASSERT_EQUAL(1.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(5.2F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(7.5F, buff_data[2]);

    buff_data = obj1->get_element(1);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[2]);

    buff_data = obj1->get_element(2);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[2]);

    /// Second element
    CSPTR(data::float3_tl::buffer_t) obj2 = deep_timeline->get_buffer(time2);

    CPPUNIT_ASSERT_EQUAL(3U, obj2->get_present_element_num());
    CPPUNIT_ASSERT_EQUAL(std::size_t(12), obj2->get_element_size());
    CPPUNIT_ASSERT_EQUAL(3U, obj2->get_max_element_num());
    CPPUNIT_ASSERT_EQUAL(true, obj2->is_present(0));
    CPPUNIT_ASSERT_EQUAL(true, obj2->is_present(1));
    CPPUNIT_ASSERT_EQUAL(true, obj2->is_present(2));
    CPPUNIT_ASSERT_EQUAL(uint64_t(7), obj2->get_mask());
    CPPUNIT_ASSERT(obj2->get_timestamp() == time2);

    buff_data = obj2->get_element(0);
    CPPUNIT_ASSERT_EQUAL(4.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(5.5F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(1.5F, buff_data[2]);

    buff_data = obj2->get_element(1);
    CPPUNIT_ASSERT_EQUAL(1.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(3.2F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(2.5F, buff_data[2]);

    buff_data = obj2->get_element(2);
    CPPUNIT_ASSERT_EQUAL(2.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(-.2F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(2.5F, buff_data[2]);

    /// Third element
    CSPTR(data::float3_tl::buffer_t) obj3 = deep_timeline->get_buffer(time3);

    CPPUNIT_ASSERT_EQUAL(2U, obj3->get_present_element_num());
    CPPUNIT_ASSERT_EQUAL(std::size_t(12), obj3->get_element_size());
    CPPUNIT_ASSERT_EQUAL(3U, obj3->get_max_element_num());
    CPPUNIT_ASSERT_EQUAL(false, obj3->is_present(0));
    CPPUNIT_ASSERT_EQUAL(true, obj3->is_present(1));
    CPPUNIT_ASSERT_EQUAL(true, obj3->is_present(2));
    CPPUNIT_ASSERT_EQUAL(uint64_t(6), obj3->get_mask());
    CPPUNIT_ASSERT(obj3->get_timestamp() == time3);

    buff_data = obj3->get_element(0);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[2]);

    buff_data = obj3->get_element(1);
    CPPUNIT_ASSERT_EQUAL(8.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(9.0F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(66.F, buff_data[2]);

    buff_data = obj3->get_element(2);
    CPPUNIT_ASSERT_EQUAL(2.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(1.2F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(11.F, buff_data[2]);

    /// Fourth element
    CSPTR(data::float3_tl::buffer_t) obj4 = deep_timeline->get_buffer(time4);

    CPPUNIT_ASSERT_EQUAL(0U, obj4->get_present_element_num());
    CPPUNIT_ASSERT_EQUAL(std::size_t(12), obj4->get_element_size());
    CPPUNIT_ASSERT_EQUAL(3U, obj4->get_max_element_num());
    CPPUNIT_ASSERT_EQUAL(false, obj4->is_present(0));
    CPPUNIT_ASSERT_EQUAL(false, obj4->is_present(1));
    CPPUNIT_ASSERT_EQUAL(false, obj4->is_present(2));
    CPPUNIT_ASSERT_EQUAL(uint64_t(0), obj4->get_mask());
    CPPUNIT_ASSERT(obj4->get_timestamp() == time4);

    buff_data = obj4->get_element(0);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[2]);

    buff_data = obj4->get_element(1);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[2]);

    buff_data = obj4->get_element(2);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[2]);
}

//------------------------------------------------------------------------------

void generic_tl_test::iterator_test()
{
    data::float3_tl::sptr timeline = std::make_shared<data::float3_tl>();
    timeline->init_pool_size(5);

    core::clock::type time1 = core::clock::get_time_in_milli_sec();
    core::clock::type time2 = time1 + 42;
    core::clock::type time3 = time2 + 52;
    core::clock::type time4 = time3 + 52;

    std::array<float, 3> values1 = {1.0F, 5.2F, 7.5F};
    std::array<float, 3> values2 = {4.0F, 5.5F, 1.5F};
    std::array<float, 3> values3 = {1.0F, 3.2F, 2.5F};
    std::array<float, 3> values4 = {2.0F, -.2F, 2.5F};
    std::array<float, 3> values5 = {8.0F, 9.0F, 66.F};
    std::array<float, 3> values6 = {2.0F, 1.2F, 11.F};

    SPTR(data::float3_tl::buffer_t) data1 = timeline->create_buffer(time1);
    data1->set_element(values1, 0);

    SPTR(data::float3_tl::buffer_t) data2 = timeline->create_buffer(time2);
    data2->set_element(values2, 0);
    data2->set_element(values3, 2);
    data2->set_element(values4, 4);

    SPTR(data::float3_tl::buffer_t) data3 = timeline->create_buffer(time3);
    data3->set_element(values5, 1);
    data3->set_element(values6, 2);

    SPTR(data::float3_tl::buffer_t) data4 = timeline->create_buffer(time4);

    timeline->push_object(data1);
    timeline->push_object(data2);
    timeline->push_object(data3);
    timeline->push_object(data4);

    CSPTR(data::float3_tl::buffer_t) obj1 = timeline->get_buffer(time1);

    /// First element
    data::float3_tl::buffer_t::iterator it = obj1->get_presence_iterator();
    CPPUNIT_ASSERT_EQUAL(1U, obj1->get_present_element_num());

    std::array<float, 3> buff_data = *it;

    CPPUNIT_ASSERT_EQUAL(1.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(5.2F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(7.5F, buff_data[2]);

    ++it;
    CPPUNIT_ASSERT_EQUAL(false, it.is_valid());

    /// Second element
    CSPTR(data::float3_tl::buffer_t) obj2 = timeline->get_buffer(time2);
    it                                    = obj2->get_presence_iterator();
    CPPUNIT_ASSERT_EQUAL(3U, obj2->get_present_element_num());

    buff_data = *it;

    CPPUNIT_ASSERT_EQUAL(4.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(5.5F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(1.5F, buff_data[2]);

    ++it;
    buff_data = *it;
    CPPUNIT_ASSERT_EQUAL(1.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(3.2F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(2.5F, buff_data[2]);

    ++it;
    buff_data = *it;
    CPPUNIT_ASSERT_EQUAL(2.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(-.2F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(2.5F, buff_data[2]);

    ++it;
    CPPUNIT_ASSERT_EQUAL(false, it.is_valid());

    /// Third element
    CSPTR(data::float3_tl::buffer_t) obj3 = timeline->get_buffer(time3);
    it                                    = obj3->get_presence_iterator();
    CPPUNIT_ASSERT_EQUAL(2U, obj3->get_present_element_num());

    buff_data = *it;

    CPPUNIT_ASSERT_EQUAL(8.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(9.0F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(66.F, buff_data[2]);

    ++it;
    buff_data = *it;
    CPPUNIT_ASSERT_EQUAL(2.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(1.2F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(11.F, buff_data[2]);

    ++it;
    CPPUNIT_ASSERT_EQUAL(false, it.is_valid());

    /// Fourth element
    CSPTR(data::float3_tl::buffer_t) obj4 = timeline->get_buffer(time4);
    it                                    = obj4->get_presence_iterator();
    CPPUNIT_ASSERT_EQUAL(0U, obj4->get_present_element_num());

    CPPUNIT_ASSERT_EQUAL(false, it.is_valid());
}

//------------------------------------------------------------------------------

void generic_tl_test::object_valid()
{
    data::float3_tl::sptr timeline1 = std::make_shared<data::float3_tl>();
    timeline1->init_pool_size(3);
    data::float4_tl::sptr timeline2 = std::make_shared<data::float4_tl>();
    timeline2->init_pool_size(3);

    core::clock::type time1 = core::clock::get_time_in_milli_sec();
    SPTR(data::float3_tl::buffer_t) data1 = timeline1->create_buffer(time1);
    SPTR(data::float4_tl::buffer_t) data2 = timeline2->create_buffer(time1);

    CPPUNIT_ASSERT_EQUAL(false, timeline1->is_object_valid(data2));
    CPPUNIT_ASSERT_EQUAL(true, timeline1->is_object_valid(data1));

    CPPUNIT_ASSERT_EQUAL(false, timeline2->is_object_valid(data1));
    CPPUNIT_ASSERT_EQUAL(true, timeline2->is_object_valid(data2));
}

//------------------------------------------------------------------------------

void generic_tl_test::equality_test()
{
    auto timeline1 = std::make_shared<data::float3_tl>();
    auto timeline2 = std::make_shared<data::float3_tl>();

    CPPUNIT_ASSERT(*timeline1 == *timeline2 && !(*timeline1 != *timeline2));

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(value) \
    timeline1->init_pool_size(value); \
    CPPUNIT_ASSERT_MESSAGE( \
        "Timelines should be different when the first is initialized with " #value, \
        *timeline1 != *timeline2 && !(*timeline1 == *timeline2) \
    ); \
    timeline2->init_pool_size(value); \
    CPPUNIT_ASSERT_MESSAGE( \
        "Timelines should be equal when they are both initialized with " #value, \
        *timeline1 == *timeline2 && !(*timeline1 != *timeline2) \
    );

    TEST(1);
    TEST(2);

    #undef TEST
}

} //namespace ut

} //namespace sight::data
