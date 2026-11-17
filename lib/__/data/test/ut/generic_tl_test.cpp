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

#include <data/generic_tl.hpp>
#include <data/registry/macros.hpp>
#include <data/timeline/generic_object.hpp>

#include <doctest/doctest.h>

#include <algorithm>
#include <array>

namespace
{

//------------------------------------------------------------------------------

class float3_tl : public sight::data::generic_tl<std::array<float,
                                                            3> >
{
public:

    SIGHT_DECLARE_CLASS(float3_tl, sight::data::timeline::base);
};
SIGHT_REGISTER_DATA(float3_tl)

using float4 = std::array<float, 4>;

//------------------------------------------------------------------------------

class float4_tl : public sight::data::generic_tl<float4>
{
public:

    SIGHT_DECLARE_CLASS(float4_tl, sight::data::timeline::base);
};
SIGHT_REGISTER_DATA(float4_tl)

//------------------------------------------------------------------------------

class test_contained
{
public:

    int m_int {};
    std::array<double, 2> m_floats {};
};

class test_class_tl : public sight::data::generic_tl<test_contained>
{
public:

    SIGHT_DECLARE_CLASS(test_class_tl, sight::data::timeline::base);
};
SIGHT_REGISTER_DATA(test_class_tl)

} // namespace

TEST_SUITE("sight::data::generic_tl")
{
//------------------------------------------------------------------------------

    TEST_CASE("push_pop")
    {
        auto timeline = std::make_shared<float4_tl>();
        timeline->init_pool_size(3);

        sight::core::clock::type time1 = sight::core::clock::get_time_in_milli_sec();
        sight::core::clock::type time2 = time1 + 42;
        sight::core::clock::type time3 = time1 + 81;

        float4 values1 = {1.0F, 5.2F, 7.5F, 1.F};
        float4 values2 = {4.0F, 5.5F, 1.5F, 2.F};
        float4 values3 = {1.0F, 3.2F, 2.5F, 0.F};

        float4 values4 = {-1.0F, 1.1F, 0.5F, -1.F};
        float4 values6 = {2.0F, 2.2F, -2.9F, 0.2F};

        // All elements set
        auto data1 = timeline->create_buffer(time1);
        data1->set_element(values1, 0);
        data1->set_element(values2, 1);
        data1->set_element(values3, 2);
        // Stupid but we need to test that
        data1->set_element(values3, 2);
        data1->set_element(values3, 2);
        data1->set_element(values1, 1);
        data1->set_element(values2, 1);

        // Second element missing
        auto data2 = timeline->create_buffer(time2);
        data2->set_element(values3, 0);
        data2->set_element(values4, 1);
        data2->set_element(values1, 2);

        auto     data3 = timeline->create_buffer(time3);
        data3->set_element(values4, 0);

        // Another way to set element
        data3->add_element(2);
        float4* new_values = data3->add_element(2);
        std::ranges::copy(values6, new_values->begin());

        timeline->push_object(data1);
        timeline->push_object(data2);
        timeline->push_object(data3);

        CHECK(data1->timestamp() == time1);
        CHECK(data2->timestamp() == time2);
        CHECK(data3->timestamp() == time3);

        ////////////////////////////////////////////////////////////////////////////
        /// Check first data with all elements set
        {
            sight::csptr<sight::data::timeline::object> data_pushed1 = timeline->get_object(time1);
            CHECK(data1 == data_pushed1);
            CHECK(data_pushed1->timestamp() == time1);

            sight::csptr<sight::data::timeline::object> data_pushed2 = timeline->get_object(time2);
            CHECK(data2 == data_pushed2);
            CHECK(data_pushed2->timestamp() == time2);

            sight::csptr<sight::data::timeline::object> data_pushed3 = timeline->get_object(time3);
            CHECK(data3 == data_pushed3);
            CHECK(data_pushed3->timestamp() == time3);

            sight::csptr<sight::data::timeline::object> data_pushed1_bis = timeline->get_closest_object(time1 + 1.5);

            sight::csptr<float4_tl::buffer_t> obj =
                std::dynamic_pointer_cast<const float4_tl::buffer_t>(data_pushed1_bis);
            CHECK(obj);
            CHECK_EQ(obj, timeline->get_closest_buffer(time1 + 1.5));

            CHECK_EQ(3U, obj->get_present_element_num());
            CHECK_EQ(std::size_t(16), obj->get_element_size());
            CHECK_EQ(3U, obj->max_element_num());
            CHECK_EQ(true, obj->is_present(0));
            CHECK_EQ(true, obj->is_present(1));
            CHECK_EQ(true, obj->is_present(2));
            CHECK_EQ(uint64_t(7), obj->mask());

            float4 buff_data = obj->get_element(0);
            CHECK_EQ(1.0F, buff_data[0]);
            CHECK_EQ(5.2F, buff_data[1]);
            CHECK_EQ(7.5F, buff_data[2]);
            CHECK_EQ(1.0F, buff_data[3]);

            buff_data = obj->get_element(1);
            CHECK_EQ(4.0F, buff_data[0]);
            CHECK_EQ(5.5F, buff_data[1]);
            CHECK_EQ(1.5F, buff_data[2]);
            CHECK_EQ(2.0F, buff_data[3]);

            buff_data = obj->get_element(2);
            CHECK_EQ(1.0F, buff_data[0]);
            CHECK_EQ(3.2F, buff_data[1]);
            CHECK_EQ(2.5F, buff_data[2]);
            CHECK_EQ(0.0F, buff_data[3]);
        }

        ////////////////////////////////////////////////////////////////////////////
        /// Check second data with one missing element
        {
            sight::csptr<sight::data::timeline::object> data_pushed3_bis = timeline->get_newer_object();
            CHECK(data3 == data_pushed3_bis);

            sight::core::clock::type time3_pushed = timeline->get_newer_timestamp();
            CHECK_EQ(time3, doctest::Approx(time3_pushed).epsilon(0.00001));

            sight::csptr<float4_tl::buffer_t> obj =
                std::dynamic_pointer_cast<const float4_tl::buffer_t>(data_pushed3_bis);
            CHECK(obj);

            CHECK_EQ(2U, obj->get_present_element_num());
            CHECK_EQ(std::size_t(16), obj->get_element_size());
            CHECK_EQ(3U, obj->max_element_num());
            CHECK_EQ(true, obj->is_present(0));
            CHECK_EQ(false, obj->is_present(1));
            CHECK_EQ(true, obj->is_present(2));
            CHECK_EQ(uint64_t(5), obj->mask());

            float4 buff_data = obj->get_element(0);
            CHECK_EQ(-1.F, buff_data[0]);
            CHECK_EQ(1.1F, buff_data[1]);
            CHECK_EQ(0.5F, buff_data[2]);
            CHECK_EQ(-1.F, buff_data[3]);

            buff_data = obj->get_element(1);
            CHECK_EQ(0.0F, buff_data[0]);
            CHECK_EQ(0.0F, buff_data[1]);
            CHECK_EQ(0.0F, buff_data[2]);
            CHECK_EQ(0.0F, buff_data[3]);

            buff_data = obj->get_element(2);
            CHECK_EQ(2.0F, buff_data[0]);
            CHECK_EQ(2.2F, buff_data[1]);
            CHECK_EQ(-2.9F, buff_data[2]);
            CHECK_EQ(0.2F, buff_data[3]);
        }

        ////////////////////////////////////////////////////////////////////////////
        /// Check pop method
        {
            sight::csptr<sight::data::timeline::object> data_pushed2_bis = timeline->get_closest_object(time2);
            CHECK(data2 == data_pushed2_bis);

            sight::csptr<sight::data::timeline::object> data_popped2 = timeline->pop_object(time2);
            CHECK(data2 == data_popped2);

            sight::csptr<sight::data::timeline::object> data_pushed3_bis = timeline->get_closest_object(time2);
            CHECK(data3 == data_pushed3_bis);

            timeline->push_object(data2);

            data_pushed2_bis = timeline->get_closest_object(time2);
            CHECK(data2 == data_pushed2_bis);

            data_pushed3_bis = timeline->get_closest_object(time3);
            CHECK(data3 == data_pushed3_bis);

            sight::csptr<sight::data::timeline::object> data_popped3 = timeline->pop_object(time3);
            CHECK(data3 == data_popped3);

            data_pushed2_bis = timeline->get_closest_object(time3);
            CHECK(data2 == data_pushed2_bis);

            data_pushed2_bis = timeline->get_newer_object();
            CHECK(data2 == data_pushed2_bis);

            sight::csptr<sight::data::timeline::object> data_pushed1_bis = timeline->get_closest_object(time1);
            CHECK(data1 == data_pushed1_bis);
        }

        timeline->clear_timeline();
        sight::csptr<sight::data::timeline::object> null_obj = timeline->get_newer_object();
        CHECK(null_obj == nullptr);
    }

//------------------------------------------------------------------------------

    TEST_CASE("push_class")
    {
        auto timeline = std::make_shared<test_class_tl>();
        timeline->init_pool_size(3);

        sight::core::clock::type time1 = sight::core::clock::get_time_in_milli_sec();
        sight::core::clock::type time2 = time1 + 42;

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
        auto data1 = timeline->create_buffer(time1);
        data1->set_element(test_data1, 1);
        data1->set_element(test_data2, 2);

        // Second element missing
        auto data2 = timeline->create_buffer(time2);
        data2->set_element(test_data3, 2);

        timeline->push_object(data1);
        timeline->push_object(data2);

        ////////////////////////////////////////////////////////////////////////////
        /// Check first data

        auto data_pushed1 = timeline->get_buffer(time1);
        CHECK(data_pushed1);

        CHECK_EQ(2U, data_pushed1->get_present_element_num());
        CHECK_EQ(sizeof(test_contained), data_pushed1->get_element_size());
        CHECK_EQ(3U, data_pushed1->max_element_num());
        CHECK_EQ(false, data_pushed1->is_present(0));
        CHECK_EQ(true, data_pushed1->is_present(1));
        CHECK_EQ(true, data_pushed1->is_present(2));
        CHECK_EQ(uint64_t(6), data_pushed1->mask());

        {
            const test_contained& test_data = data_pushed1->get_element(1);
            CHECK_EQ(test_data1.m_int, test_data.m_int);
            CHECK_EQ(test_data1.m_floats[0], test_data.m_floats[0]);
            CHECK_EQ(test_data1.m_floats[1], test_data.m_floats[1]);
        }
        {
            const test_contained& test_data = data_pushed1->get_element(2);
            CHECK_EQ(test_data2.m_int, test_data.m_int);
            CHECK_EQ(test_data2.m_floats[0], test_data.m_floats[0]);
            CHECK_EQ(test_data2.m_floats[1], test_data.m_floats[1]);
        }

        ////////////////////////////////////////////////////////////////////////////
        /// Check second data

        auto data_pushed2 = timeline->get_buffer(time2);
        CHECK(data_pushed2);

        CHECK_EQ(1U, data_pushed2->get_present_element_num());
        CHECK_EQ(sizeof(test_contained), data_pushed2->get_element_size());
        CHECK_EQ(3U, data_pushed2->max_element_num());
        CHECK_EQ(false, data_pushed2->is_present(0));
        CHECK_EQ(false, data_pushed2->is_present(1));
        CHECK_EQ(true, data_pushed2->is_present(2));
        CHECK_EQ(uint64_t(4), data_pushed2->mask());

        {
            const test_contained& test_data = data_pushed2->get_element(2);
            CHECK_EQ(test_data3.m_int, test_data.m_int);
            CHECK_EQ(test_data3.m_floats[0], test_data.m_floats[0]);
            CHECK_EQ(test_data3.m_floats[1], test_data.m_floats[1]);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("copy")
    {
        auto timeline = std::make_shared<float3_tl>();
        timeline->init_pool_size(3);

        sight::core::clock::type time1  = sight::core::clock::get_time_in_milli_sec();
        sight::core::clock::type time2  = time1 + 42;
        sight::core::clock::type time2b = time2 + 12;
        sight::core::clock::type time3  = time2 + 52;
        sight::core::clock::type time4  = time3 + 52;
        sight::core::clock::type time4b = time4 + 12;

        std::array<float, 3> values1 = {1.0F, 5.2F, 7.5F};
        std::array<float, 3> values2 = {4.0F, 5.5F, 1.5F};
        std::array<float, 3> values3 = {1.0F, 3.2F, 2.5F};
        std::array<float, 3> values4 = {2.0F, -.2F, 2.5F};
        std::array<float, 3> values5 = {8.0F, 9.0F, 66.F};
        std::array<float, 3> values6 = {2.0F, 1.2F, 11.F};

        auto data1 = timeline->create_buffer(time1);
        data1->set_element(values1, 0);
        CHECK(data1->timestamp() == time1);

        auto data2 = timeline->create_buffer(time2);
        data2->set_element(values2, 0);
        data2->set_element(values3, 1);
        data2->set_element(values4, 2);
        CHECK(data2->timestamp() == time2);

        auto data2b = timeline->create_buffer(time2b);
        data2b->set_element(values1, 0);
        data2b->set_element(values4, 1);
        CHECK(data2b->timestamp() == time2b);

        auto data3 = timeline->create_buffer(time3);
        data3->set_element(values5, 1);
        data3->set_element(values6, 2);
        CHECK(data3->timestamp() == time3);

        auto data4 = timeline->create_buffer(time4);
        CHECK(data4->timestamp() == time4);

        auto data4b = timeline->create_buffer(time4b);
        data4b->set_element(values5, 0);
        data4b->set_element(values3, 2);
        CHECK(data4b->timestamp() == time4b);

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
        float3_tl::sptr deep_timeline = std::make_shared<float3_tl>();
        deep_timeline->deep_copy(timeline);
        timeline.reset();

        sight::csptr<sight::data::timeline::object> deep_data_pushed1 = deep_timeline->get_object(time1);
        CHECK(deep_data_pushed1);
        CHECK(data1 != deep_data_pushed1);
        sight::csptr<float3_tl::buffer_t> obj1 =
            std::dynamic_pointer_cast<const float3_tl::buffer_t>(deep_data_pushed1);
        CHECK(obj1);
        CHECK_EQ(obj1, deep_timeline->get_buffer(time1));

        /// First element
        CHECK_EQ(1U, obj1->get_present_element_num());
        CHECK_EQ(std::size_t(12), obj1->get_element_size());
        CHECK_EQ(3U, obj1->max_element_num());
        CHECK_EQ(true, obj1->is_present(0));
        CHECK_EQ(false, obj1->is_present(1));
        CHECK_EQ(false, obj1->is_present(2));
        CHECK_EQ(uint64_t(1), obj1->mask());
        CHECK(obj1->timestamp() == time1);

        std::array<float, 3> buff_data = obj1->get_element(0);
        CHECK_EQ(1.0F, buff_data[0]);
        CHECK_EQ(5.2F, buff_data[1]);
        CHECK_EQ(7.5F, buff_data[2]);

        buff_data = obj1->get_element(1);
        CHECK_EQ(0.0F, buff_data[0]);
        CHECK_EQ(0.0F, buff_data[1]);
        CHECK_EQ(0.0F, buff_data[2]);

        buff_data = obj1->get_element(2);
        CHECK_EQ(0.0F, buff_data[0]);
        CHECK_EQ(0.0F, buff_data[1]);
        CHECK_EQ(0.0F, buff_data[2]);

        /// Second element
        sight::csptr<float3_tl::buffer_t> obj2 = deep_timeline->get_buffer(time2);

        CHECK_EQ(3U, obj2->get_present_element_num());
        CHECK_EQ(std::size_t(12), obj2->get_element_size());
        CHECK_EQ(3U, obj2->max_element_num());
        CHECK_EQ(true, obj2->is_present(0));
        CHECK_EQ(true, obj2->is_present(1));
        CHECK_EQ(true, obj2->is_present(2));
        CHECK_EQ(uint64_t(7), obj2->mask());
        CHECK(obj2->timestamp() == time2);

        buff_data = obj2->get_element(0);
        CHECK_EQ(4.0F, buff_data[0]);
        CHECK_EQ(5.5F, buff_data[1]);
        CHECK_EQ(1.5F, buff_data[2]);

        buff_data = obj2->get_element(1);
        CHECK_EQ(1.0F, buff_data[0]);
        CHECK_EQ(3.2F, buff_data[1]);
        CHECK_EQ(2.5F, buff_data[2]);

        buff_data = obj2->get_element(2);
        CHECK_EQ(2.0F, buff_data[0]);
        CHECK_EQ(-.2F, buff_data[1]);
        CHECK_EQ(2.5F, buff_data[2]);

        /// Third element
        sight::csptr<float3_tl::buffer_t> obj3 = deep_timeline->get_buffer(time3);

        CHECK_EQ(2U, obj3->get_present_element_num());
        CHECK_EQ(std::size_t(12), obj3->get_element_size());
        CHECK_EQ(3U, obj3->max_element_num());
        CHECK_EQ(false, obj3->is_present(0));
        CHECK_EQ(true, obj3->is_present(1));
        CHECK_EQ(true, obj3->is_present(2));
        CHECK_EQ(uint64_t(6), obj3->mask());
        CHECK(obj3->timestamp() == time3);

        buff_data = obj3->get_element(0);
        CHECK_EQ(0.0F, buff_data[0]);
        CHECK_EQ(0.0F, buff_data[1]);
        CHECK_EQ(0.0F, buff_data[2]);

        buff_data = obj3->get_element(1);
        CHECK_EQ(8.0F, buff_data[0]);
        CHECK_EQ(9.0F, buff_data[1]);
        CHECK_EQ(66.F, buff_data[2]);

        buff_data = obj3->get_element(2);
        CHECK_EQ(2.0F, buff_data[0]);
        CHECK_EQ(1.2F, buff_data[1]);
        CHECK_EQ(11.F, buff_data[2]);

        /// Fourth element
        sight::csptr<float3_tl::buffer_t> obj4 = deep_timeline->get_buffer(time4);

        CHECK_EQ(0U, obj4->get_present_element_num());
        CHECK_EQ(std::size_t(12), obj4->get_element_size());
        CHECK_EQ(3U, obj4->max_element_num());
        CHECK_EQ(false, obj4->is_present(0));
        CHECK_EQ(false, obj4->is_present(1));
        CHECK_EQ(false, obj4->is_present(2));
        CHECK_EQ(uint64_t(0), obj4->mask());
        CHECK(obj4->timestamp() == time4);

        buff_data = obj4->get_element(0);
        CHECK_EQ(0.0F, buff_data[0]);
        CHECK_EQ(0.0F, buff_data[1]);
        CHECK_EQ(0.0F, buff_data[2]);

        buff_data = obj4->get_element(1);
        CHECK_EQ(0.0F, buff_data[0]);
        CHECK_EQ(0.0F, buff_data[1]);
        CHECK_EQ(0.0F, buff_data[2]);

        buff_data = obj4->get_element(2);
        CHECK_EQ(0.0F, buff_data[0]);
        CHECK_EQ(0.0F, buff_data[1]);
        CHECK_EQ(0.0F, buff_data[2]);
    }

//------------------------------------------------------------------------------

    TEST_CASE("iterator")
    {
        float3_tl::sptr timeline = std::make_shared<float3_tl>();
        timeline->init_pool_size(5);

        sight::core::clock::type time1 = sight::core::clock::get_time_in_milli_sec();
        sight::core::clock::type time2 = time1 + 42;
        sight::core::clock::type time3 = time2 + 52;
        sight::core::clock::type time4 = time3 + 52;

        std::array<float, 3> values1 = {1.0F, 5.2F, 7.5F};
        std::array<float, 3> values2 = {4.0F, 5.5F, 1.5F};
        std::array<float, 3> values3 = {1.0F, 3.2F, 2.5F};
        std::array<float, 3> values4 = {2.0F, -.2F, 2.5F};
        std::array<float, 3> values5 = {8.0F, 9.0F, 66.F};
        std::array<float, 3> values6 = {2.0F, 1.2F, 11.F};

        sight::sptr<float3_tl::buffer_t> data1 = timeline->create_buffer(time1);
        data1->set_element(values1, 0);

        sight::sptr<float3_tl::buffer_t> data2 = timeline->create_buffer(time2);
        data2->set_element(values2, 0);
        data2->set_element(values3, 2);
        data2->set_element(values4, 4);

        sight::sptr<float3_tl::buffer_t> data3 = timeline->create_buffer(time3);
        data3->set_element(values5, 1);
        data3->set_element(values6, 2);

        sight::sptr<float3_tl::buffer_t> data4 = timeline->create_buffer(time4);

        timeline->push_object(data1);
        timeline->push_object(data2);
        timeline->push_object(data3);
        timeline->push_object(data4);

        sight::csptr<float3_tl::buffer_t> obj1 = timeline->get_buffer(time1);

        /// First element
        float3_tl::buffer_t::iterator it = obj1->get_presence_iterator();
        CHECK_EQ(1U, obj1->get_present_element_num());

        std::array<float, 3> buff_data = *it;

        CHECK_EQ(1.0F, buff_data[0]);
        CHECK_EQ(5.2F, buff_data[1]);
        CHECK_EQ(7.5F, buff_data[2]);

        ++it;
        CHECK_EQ(false, it.is_valid());

        /// Second element
        sight::csptr<float3_tl::buffer_t> obj2 = timeline->get_buffer(time2);
        it = obj2->get_presence_iterator();
        CHECK_EQ(3U, obj2->get_present_element_num());

        buff_data = *it;

        CHECK_EQ(4.0F, buff_data[0]);
        CHECK_EQ(5.5F, buff_data[1]);
        CHECK_EQ(1.5F, buff_data[2]);

        ++it;
        buff_data = *it;
        CHECK_EQ(1.0F, buff_data[0]);
        CHECK_EQ(3.2F, buff_data[1]);
        CHECK_EQ(2.5F, buff_data[2]);

        ++it;
        buff_data = *it;
        CHECK_EQ(2.0F, buff_data[0]);
        CHECK_EQ(-.2F, buff_data[1]);
        CHECK_EQ(2.5F, buff_data[2]);

        ++it;
        CHECK_EQ(false, it.is_valid());

        /// Third element
        sight::csptr<float3_tl::buffer_t> obj3 = timeline->get_buffer(time3);
        it = obj3->get_presence_iterator();
        CHECK_EQ(2U, obj3->get_present_element_num());

        buff_data = *it;

        CHECK_EQ(8.0F, buff_data[0]);
        CHECK_EQ(9.0F, buff_data[1]);
        CHECK_EQ(66.F, buff_data[2]);

        ++it;
        buff_data = *it;
        CHECK_EQ(2.0F, buff_data[0]);
        CHECK_EQ(1.2F, buff_data[1]);
        CHECK_EQ(11.F, buff_data[2]);

        ++it;
        CHECK_EQ(false, it.is_valid());

        /// Fourth element
        sight::csptr<float3_tl::buffer_t> obj4 = timeline->get_buffer(time4);
        it = obj4->get_presence_iterator();
        CHECK_EQ(0U, obj4->get_present_element_num());

        CHECK_EQ(false, it.is_valid());
    }

//------------------------------------------------------------------------------

    TEST_CASE("object_valid")
    {
        float3_tl::sptr timeline1 = std::make_shared<float3_tl>();
        timeline1->init_pool_size(3);
        float4_tl::sptr timeline2 = std::make_shared<float4_tl>();
        timeline2->init_pool_size(3);

        sight::core::clock::type time1 = sight::core::clock::get_time_in_milli_sec();
        auto data1                     = timeline1->create_buffer(time1);
        auto data2                     = timeline2->create_buffer(time1);

        CHECK_EQ(false, timeline1->is_object_valid(data2));
        CHECK_EQ(true, timeline1->is_object_valid(data1));

        CHECK_EQ(false, timeline2->is_object_valid(data1));
        CHECK_EQ(true, timeline2->is_object_valid(data2));
    }

//------------------------------------------------------------------------------

    TEST_CASE("equality")
    {
        auto timeline1 = std::make_shared<float3_tl>();
        auto timeline2 = std::make_shared<float3_tl>();

        CHECK(*timeline1 == *timeline2);
        CHECK(!(*timeline1 != *timeline2));

        // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(value) \
            timeline1->init_pool_size(value); \
            CHECK_MESSAGE( \
                *timeline1 != *timeline2, \
                "Timelines should be different when the first is initialized with " #value \
            ); \
            CHECK_MESSAGE( \
                !(*timeline1 == *timeline2), \
                "Timelines should be different when the first is initialized with " #value \
            ); \
            timeline2->init_pool_size(value); \
            CHECK_MESSAGE( \
                *timeline1 == *timeline2, \
                "Timelines should be equal when they are both initialized with " #value \
            ); \
            CHECK_MESSAGE( \
                !(*timeline1 != *timeline2), \
                "Timelines should be equal when they are both initialized with " #value \
            );

        TEST(1);
        TEST(2);

    #undef TEST
    }
} // TEST_SUITE("sight::data::generic_tl")
