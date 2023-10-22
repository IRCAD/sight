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

class Float3TL : public generic_tl<std::array<float,
                                              3> >
{
public:

    SIGHT_DECLARE_CLASS(Float3TL, data::timeline::base);
};
SIGHT_REGISTER_DATA(sight::data::Float3TL)

using float4 = std::array<float, 4>;

//------------------------------------------------------------------------------

class Float4TL : public generic_tl<float4>
{
public:

    SIGHT_DECLARE_CLASS(Float4TL, data::timeline::base);
};
SIGHT_REGISTER_DATA(sight::data::Float4TL)

//------------------------------------------------------------------------------

class TestContained
{
public:

    int m_int {};
    std::array<double, 2> m_floats {};
};

class TestClassTL : public generic_tl<TestContained>
{
public:

    SIGHT_DECLARE_CLASS(TestClassTL, data::timeline::base);
};
SIGHT_REGISTER_DATA(sight::data::TestClassTL)

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

void generic_tl_test::pushPopTest()
{
    data::Float4TL::sptr timeline = std::make_shared<data::Float4TL>();
    timeline->initPoolSize(3);

    core::hires_clock::type time1 = core::hires_clock::get_time_in_milli_sec();
    core::hires_clock::type time2 = time1 + 42;
    core::hires_clock::type time3 = time1 + 81;

    float4 values1 = {1.0F, 5.2F, 7.5F, 1.F};
    float4 values2 = {4.0F, 5.5F, 1.5F, 2.F};
    float4 values3 = {1.0F, 3.2F, 2.5F, 0.F};

    float4 values4 = {-1.0F, 1.1F, 0.5F, -1.F};
    float4 values6 = {2.0F, 2.2F, -2.9F, 0.2F};

    // All elements set
    SPTR(data::Float4TL::buffer_t) data1 = timeline->createBuffer(time1);
    data1->setElement(values1, 0);
    data1->setElement(values2, 1);
    data1->setElement(values3, 2);
    // Stupid but we need to test that
    data1->setElement(values3, 2);
    data1->setElement(values3, 2);
    data1->setElement(values1, 1);
    data1->setElement(values2, 1);

    // Second element missing
    SPTR(data::Float4TL::buffer_t) data2 = timeline->createBuffer(time2);
    data2->setElement(values3, 0);
    data2->setElement(values4, 1);
    data2->setElement(values1, 2);

    SPTR(data::Float4TL::buffer_t) data3 = timeline->createBuffer(time3);
    data3->setElement(values4, 0);

    // Another way to set element
    data3->addElement(2);
    float4* new_values = data3->addElement(2);
    std::copy(values6.begin(), values6.end(), new_values->begin());

    timeline->pushObject(data1);
    timeline->pushObject(data2);
    timeline->pushObject(data3);

    CPPUNIT_ASSERT(data1->getTimestamp() == time1);
    CPPUNIT_ASSERT(data2->getTimestamp() == time2);
    CPPUNIT_ASSERT(data3->getTimestamp() == time3);

    ////////////////////////////////////////////////////////////////////////////
    /// Check first data with all elements set
    {
        CSPTR(data::timeline::object) data_pushed1 = timeline->getObject(time1);
        CPPUNIT_ASSERT(data1 == data_pushed1);
        CPPUNIT_ASSERT(data_pushed1->getTimestamp() == time1);

        CSPTR(data::timeline::object) data_pushed2 = timeline->getObject(time2);
        CPPUNIT_ASSERT(data2 == data_pushed2);
        CPPUNIT_ASSERT(data_pushed2->getTimestamp() == time2);

        CSPTR(data::timeline::object) data_pushed3 = timeline->getObject(time3);
        CPPUNIT_ASSERT(data3 == data_pushed3);
        CPPUNIT_ASSERT(data_pushed3->getTimestamp() == time3);

        CSPTR(data::timeline::object) data_pushed1_bis = timeline->getClosestObject(time1 + 1.5);
        CSPTR(data::Float4TL::buffer_t) obj            =
            std::dynamic_pointer_cast<const data::Float4TL::buffer_t>(data_pushed1_bis);
        CPPUNIT_ASSERT(obj);
        CPPUNIT_ASSERT_EQUAL(obj, timeline->getClosestBuffer(time1 + 1.5));

        CPPUNIT_ASSERT_EQUAL(3U, obj->getPresentElementNum());
        CPPUNIT_ASSERT_EQUAL(std::size_t(16), obj->getElementSize());
        CPPUNIT_ASSERT_EQUAL(3U, obj->getMaxElementNum());
        CPPUNIT_ASSERT_EQUAL(true, obj->isPresent(0));
        CPPUNIT_ASSERT_EQUAL(true, obj->isPresent(1));
        CPPUNIT_ASSERT_EQUAL(true, obj->isPresent(2));
        CPPUNIT_ASSERT_EQUAL(uint64_t(7), obj->getMask());

        float4 buff_data = obj->getElement(0);
        CPPUNIT_ASSERT_EQUAL(1.0F, buff_data[0]);
        CPPUNIT_ASSERT_EQUAL(5.2F, buff_data[1]);
        CPPUNIT_ASSERT_EQUAL(7.5F, buff_data[2]);
        CPPUNIT_ASSERT_EQUAL(1.0F, buff_data[3]);

        buff_data = obj->getElement(1);
        CPPUNIT_ASSERT_EQUAL(4.0F, buff_data[0]);
        CPPUNIT_ASSERT_EQUAL(5.5F, buff_data[1]);
        CPPUNIT_ASSERT_EQUAL(1.5F, buff_data[2]);
        CPPUNIT_ASSERT_EQUAL(2.0F, buff_data[3]);

        buff_data = obj->getElement(2);
        CPPUNIT_ASSERT_EQUAL(1.0F, buff_data[0]);
        CPPUNIT_ASSERT_EQUAL(3.2F, buff_data[1]);
        CPPUNIT_ASSERT_EQUAL(2.5F, buff_data[2]);
        CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[3]);
    }

    ////////////////////////////////////////////////////////////////////////////
    /// Check second data with one missing element
    {
        CSPTR(data::timeline::object) data_pushed3_bis = timeline->getNewerObject();
        CPPUNIT_ASSERT(data3 == data_pushed3_bis);

        core::hires_clock::type time3_pushed = timeline->getNewerTimestamp();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(time3, time3_pushed, 0.00001);

        CSPTR(data::Float4TL::buffer_t) obj =
            std::dynamic_pointer_cast<const data::Float4TL::buffer_t>(data_pushed3_bis);
        CPPUNIT_ASSERT(obj);

        CPPUNIT_ASSERT_EQUAL(2U, obj->getPresentElementNum());
        CPPUNIT_ASSERT_EQUAL(std::size_t(16), obj->getElementSize());
        CPPUNIT_ASSERT_EQUAL(3U, obj->getMaxElementNum());
        CPPUNIT_ASSERT_EQUAL(true, obj->isPresent(0));
        CPPUNIT_ASSERT_EQUAL(false, obj->isPresent(1));
        CPPUNIT_ASSERT_EQUAL(true, obj->isPresent(2));
        CPPUNIT_ASSERT_EQUAL(uint64_t(5), obj->getMask());

        float4 buff_data = obj->getElement(0);
        CPPUNIT_ASSERT_EQUAL(-1.F, buff_data[0]);
        CPPUNIT_ASSERT_EQUAL(1.1F, buff_data[1]);
        CPPUNIT_ASSERT_EQUAL(0.5F, buff_data[2]);
        CPPUNIT_ASSERT_EQUAL(-1.F, buff_data[3]);

        buff_data = obj->getElement(1);
        CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[0]);
        CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[1]);
        CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[2]);
        CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[3]);

        buff_data = obj->getElement(2);
        CPPUNIT_ASSERT_EQUAL(2.0F, buff_data[0]);
        CPPUNIT_ASSERT_EQUAL(2.2F, buff_data[1]);
        CPPUNIT_ASSERT_EQUAL(-2.9F, buff_data[2]);
        CPPUNIT_ASSERT_EQUAL(0.2F, buff_data[3]);
    }

    ////////////////////////////////////////////////////////////////////////////
    /// Check pop method
    {
        CSPTR(data::timeline::object) data_pushed2_bis = timeline->getClosestObject(time2);
        CPPUNIT_ASSERT(data2 == data_pushed2_bis);

        CSPTR(data::timeline::object) data_popped2 = timeline->popObject(time2);
        CPPUNIT_ASSERT(data2 == data_popped2);

        CSPTR(data::timeline::object) data_pushed3_bis = timeline->getClosestObject(time2);
        CPPUNIT_ASSERT(data3 == data_pushed3_bis);

        timeline->pushObject(data2);

        data_pushed2_bis = timeline->getClosestObject(time2);
        CPPUNIT_ASSERT(data2 == data_pushed2_bis);

        data_pushed3_bis = timeline->getClosestObject(time3);
        CPPUNIT_ASSERT(data3 == data_pushed3_bis);

        CSPTR(data::timeline::object) data_popped3 = timeline->popObject(time3);
        CPPUNIT_ASSERT(data3 == data_popped3);

        data_pushed2_bis = timeline->getClosestObject(time3);
        CPPUNIT_ASSERT(data2 == data_pushed2_bis);

        data_pushed2_bis = timeline->getNewerObject();
        CPPUNIT_ASSERT(data2 == data_pushed2_bis);

        CSPTR(data::timeline::object) data_pushed1_bis = timeline->getClosestObject(time1);
        CPPUNIT_ASSERT(data1 == data_pushed1_bis);
    }

    timeline->clearTimeline();
    CSPTR(data::timeline::object) null_obj = timeline->getNewerObject();
    CPPUNIT_ASSERT(null_obj == nullptr);
}

//------------------------------------------------------------------------------

void generic_tl_test::pushClassTest()
{
    data::TestClassTL::sptr timeline = std::make_shared<data::TestClassTL>();
    timeline->initPoolSize(3);

    core::hires_clock::type time1 = core::hires_clock::get_time_in_milli_sec();
    core::hires_clock::type time2 = time1 + 42;

    TestContained test_data1;
    test_data1.m_int       = 87;
    test_data1.m_floats[0] = 12.79;
    test_data1.m_floats[1] = -459.59F;

    TestContained test_data2;
    test_data2.m_int       = -45471;
    test_data2.m_floats[0] = 68.49;
    test_data2.m_floats[1] = -9.994F;

    TestContained test_data3;
    test_data3.m_int       = 447471;
    test_data3.m_floats[0] = 6822.49;
    test_data3.m_floats[1] = -92194.47F;

    // All elements set
    SPTR(data::TestClassTL::buffer_t) data1 = timeline->createBuffer(time1);
    data1->setElement(test_data1, 1);
    data1->setElement(test_data2, 2);

    // Second element missing
    SPTR(data::TestClassTL::buffer_t) data2 = timeline->createBuffer(time2);
    data2->setElement(test_data3, 2);

    timeline->pushObject(data1);
    timeline->pushObject(data2);

    ////////////////////////////////////////////////////////////////////////////
    /// Check first data

    CSPTR(data::TestClassTL::buffer_t) data_pushed1 = timeline->get_buffer(time1);
    CPPUNIT_ASSERT(data_pushed1);

    CPPUNIT_ASSERT_EQUAL(2U, data_pushed1->getPresentElementNum());
    CPPUNIT_ASSERT_EQUAL(sizeof(TestContained), data_pushed1->getElementSize());
    CPPUNIT_ASSERT_EQUAL(3U, data_pushed1->getMaxElementNum());
    CPPUNIT_ASSERT_EQUAL(false, data_pushed1->isPresent(0));
    CPPUNIT_ASSERT_EQUAL(true, data_pushed1->isPresent(1));
    CPPUNIT_ASSERT_EQUAL(true, data_pushed1->isPresent(2));
    CPPUNIT_ASSERT_EQUAL(uint64_t(6), data_pushed1->getMask());

    {
        const TestContained& test_data = data_pushed1->getElement(1);
        CPPUNIT_ASSERT_EQUAL(test_data1.m_int, test_data.m_int);
        CPPUNIT_ASSERT_EQUAL(test_data1.m_floats[0], test_data.m_floats[0]);
        CPPUNIT_ASSERT_EQUAL(test_data1.m_floats[1], test_data.m_floats[1]);
    }
    {
        const TestContained& test_data = data_pushed1->getElement(2);
        CPPUNIT_ASSERT_EQUAL(test_data2.m_int, test_data.m_int);
        CPPUNIT_ASSERT_EQUAL(test_data2.m_floats[0], test_data.m_floats[0]);
        CPPUNIT_ASSERT_EQUAL(test_data2.m_floats[1], test_data.m_floats[1]);
    }

    ////////////////////////////////////////////////////////////////////////////
    /// Check second data

    CSPTR(data::TestClassTL::buffer_t) data_pushed2 = timeline->get_buffer(time2);
    CPPUNIT_ASSERT(data_pushed2);

    CPPUNIT_ASSERT_EQUAL(1U, data_pushed2->getPresentElementNum());
    CPPUNIT_ASSERT_EQUAL(sizeof(TestContained), data_pushed2->getElementSize());
    CPPUNIT_ASSERT_EQUAL(3U, data_pushed2->getMaxElementNum());
    CPPUNIT_ASSERT_EQUAL(false, data_pushed2->isPresent(0));
    CPPUNIT_ASSERT_EQUAL(false, data_pushed2->isPresent(1));
    CPPUNIT_ASSERT_EQUAL(true, data_pushed2->isPresent(2));
    CPPUNIT_ASSERT_EQUAL(uint64_t(4), data_pushed2->getMask());

    {
        const TestContained& test_data = data_pushed2->getElement(2);
        CPPUNIT_ASSERT_EQUAL(test_data3.m_int, test_data.m_int);
        CPPUNIT_ASSERT_EQUAL(test_data3.m_floats[0], test_data.m_floats[0]);
        CPPUNIT_ASSERT_EQUAL(test_data3.m_floats[1], test_data.m_floats[1]);
    }
}

//------------------------------------------------------------------------------

void generic_tl_test::copyTest()
{
    data::Float3TL::sptr timeline = std::make_shared<data::Float3TL>();
    timeline->initPoolSize(3);

    core::hires_clock::type time1  = core::hires_clock::get_time_in_milli_sec();
    core::hires_clock::type time2  = time1 + 42;
    core::hires_clock::type time2b = time2 + 12;
    core::hires_clock::type time3  = time2 + 52;
    core::hires_clock::type time4  = time3 + 52;
    core::hires_clock::type time4b = time4 + 12;

    std::array<float, 3> values1 = {1.0F, 5.2F, 7.5F};
    std::array<float, 3> values2 = {4.0F, 5.5F, 1.5F};
    std::array<float, 3> values3 = {1.0F, 3.2F, 2.5F};
    std::array<float, 3> values4 = {2.0F, -.2F, 2.5F};
    std::array<float, 3> values5 = {8.0F, 9.0F, 66.F};
    std::array<float, 3> values6 = {2.0F, 1.2F, 11.F};

    SPTR(data::Float3TL::buffer_t) data1 = timeline->createBuffer(time1);
    data1->setElement(values1, 0);
    CPPUNIT_ASSERT(data1->getTimestamp() == time1);

    SPTR(data::Float3TL::buffer_t) data2 = timeline->createBuffer(time2);
    data2->setElement(values2, 0);
    data2->setElement(values3, 1);
    data2->setElement(values4, 2);
    CPPUNIT_ASSERT(data2->getTimestamp() == time2);

    SPTR(data::Float3TL::buffer_t) data2b = timeline->createBuffer(time2b);
    data2b->setElement(values1, 0);
    data2b->setElement(values4, 1);
    CPPUNIT_ASSERT(data2b->getTimestamp() == time2b);

    SPTR(data::Float3TL::buffer_t) data3 = timeline->createBuffer(time3);
    data3->setElement(values5, 1);
    data3->setElement(values6, 2);
    CPPUNIT_ASSERT(data3->getTimestamp() == time3);

    SPTR(data::Float3TL::buffer_t) data4 = timeline->createBuffer(time4);
    CPPUNIT_ASSERT(data4->getTimestamp() == time4);

    SPTR(data::Float3TL::buffer_t) data4b = timeline->createBuffer(time4b);
    data4b->setElement(values5, 0);
    data4b->setElement(values3, 2);
    CPPUNIT_ASSERT(data4b->getTimestamp() == time4b);

    timeline->pushObject(data1);
    timeline->pushObject(data2);
    timeline->pushObject(data2b);
    timeline->pushObject(data3);
    timeline->pushObject(data4);
    timeline->pushObject(data4b);

    // remove some objects
    timeline->popObject(time2b);
    timeline->popObject(time4b);

    // deep_copy test
    data::Float3TL::sptr deep_timeline = std::make_shared<data::Float3TL>();
    deep_timeline->deep_copy(timeline);
    timeline.reset();

    CSPTR(data::timeline::object) deep_data_pushed1 = deep_timeline->getObject(time1);
    CPPUNIT_ASSERT(deep_data_pushed1);
    CPPUNIT_ASSERT(data1 != deep_data_pushed1);
    CSPTR(data::Float3TL::buffer_t) obj1 =
        std::dynamic_pointer_cast<const data::Float3TL::buffer_t>(deep_data_pushed1);
    CPPUNIT_ASSERT(obj1);
    CPPUNIT_ASSERT_EQUAL(obj1, deep_timeline->get_buffer(time1));

    /// First element
    CPPUNIT_ASSERT_EQUAL(1U, obj1->getPresentElementNum());
    CPPUNIT_ASSERT_EQUAL(std::size_t(12), obj1->getElementSize());
    CPPUNIT_ASSERT_EQUAL(3U, obj1->getMaxElementNum());
    CPPUNIT_ASSERT_EQUAL(true, obj1->isPresent(0));
    CPPUNIT_ASSERT_EQUAL(false, obj1->isPresent(1));
    CPPUNIT_ASSERT_EQUAL(false, obj1->isPresent(2));
    CPPUNIT_ASSERT_EQUAL(uint64_t(1), obj1->getMask());
    CPPUNIT_ASSERT(obj1->getTimestamp() == time1);

    std::array<float, 3> buff_data = obj1->getElement(0);
    CPPUNIT_ASSERT_EQUAL(1.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(5.2F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(7.5F, buff_data[2]);

    buff_data = obj1->getElement(1);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[2]);

    buff_data = obj1->getElement(2);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[2]);

    /// Second element
    CSPTR(data::Float3TL::buffer_t) obj2 = deep_timeline->get_buffer(time2);

    CPPUNIT_ASSERT_EQUAL(3U, obj2->getPresentElementNum());
    CPPUNIT_ASSERT_EQUAL(std::size_t(12), obj2->getElementSize());
    CPPUNIT_ASSERT_EQUAL(3U, obj2->getMaxElementNum());
    CPPUNIT_ASSERT_EQUAL(true, obj2->isPresent(0));
    CPPUNIT_ASSERT_EQUAL(true, obj2->isPresent(1));
    CPPUNIT_ASSERT_EQUAL(true, obj2->isPresent(2));
    CPPUNIT_ASSERT_EQUAL(uint64_t(7), obj2->getMask());
    CPPUNIT_ASSERT(obj2->getTimestamp() == time2);

    buff_data = obj2->getElement(0);
    CPPUNIT_ASSERT_EQUAL(4.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(5.5F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(1.5F, buff_data[2]);

    buff_data = obj2->getElement(1);
    CPPUNIT_ASSERT_EQUAL(1.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(3.2F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(2.5F, buff_data[2]);

    buff_data = obj2->getElement(2);
    CPPUNIT_ASSERT_EQUAL(2.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(-.2F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(2.5F, buff_data[2]);

    /// Third element
    CSPTR(data::Float3TL::buffer_t) obj3 = deep_timeline->get_buffer(time3);

    CPPUNIT_ASSERT_EQUAL(2U, obj3->getPresentElementNum());
    CPPUNIT_ASSERT_EQUAL(std::size_t(12), obj3->getElementSize());
    CPPUNIT_ASSERT_EQUAL(3U, obj3->getMaxElementNum());
    CPPUNIT_ASSERT_EQUAL(false, obj3->isPresent(0));
    CPPUNIT_ASSERT_EQUAL(true, obj3->isPresent(1));
    CPPUNIT_ASSERT_EQUAL(true, obj3->isPresent(2));
    CPPUNIT_ASSERT_EQUAL(uint64_t(6), obj3->getMask());
    CPPUNIT_ASSERT(obj3->getTimestamp() == time3);

    buff_data = obj3->getElement(0);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[2]);

    buff_data = obj3->getElement(1);
    CPPUNIT_ASSERT_EQUAL(8.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(9.0F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(66.F, buff_data[2]);

    buff_data = obj3->getElement(2);
    CPPUNIT_ASSERT_EQUAL(2.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(1.2F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(11.F, buff_data[2]);

    /// Fourth element
    CSPTR(data::Float3TL::buffer_t) obj4 = deep_timeline->get_buffer(time4);

    CPPUNIT_ASSERT_EQUAL(0U, obj4->getPresentElementNum());
    CPPUNIT_ASSERT_EQUAL(std::size_t(12), obj4->getElementSize());
    CPPUNIT_ASSERT_EQUAL(3U, obj4->getMaxElementNum());
    CPPUNIT_ASSERT_EQUAL(false, obj4->isPresent(0));
    CPPUNIT_ASSERT_EQUAL(false, obj4->isPresent(1));
    CPPUNIT_ASSERT_EQUAL(false, obj4->isPresent(2));
    CPPUNIT_ASSERT_EQUAL(uint64_t(0), obj4->getMask());
    CPPUNIT_ASSERT(obj4->getTimestamp() == time4);

    buff_data = obj4->getElement(0);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[2]);

    buff_data = obj4->getElement(1);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[2]);

    buff_data = obj4->getElement(2);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(0.0F, buff_data[2]);
}

//------------------------------------------------------------------------------

void generic_tl_test::iteratorTest()
{
    data::Float3TL::sptr timeline = std::make_shared<data::Float3TL>();
    timeline->initPoolSize(5);

    core::hires_clock::type time1 = core::hires_clock::get_time_in_milli_sec();
    core::hires_clock::type time2 = time1 + 42;
    core::hires_clock::type time3 = time2 + 52;
    core::hires_clock::type time4 = time3 + 52;

    std::array<float, 3> values1 = {1.0F, 5.2F, 7.5F};
    std::array<float, 3> values2 = {4.0F, 5.5F, 1.5F};
    std::array<float, 3> values3 = {1.0F, 3.2F, 2.5F};
    std::array<float, 3> values4 = {2.0F, -.2F, 2.5F};
    std::array<float, 3> values5 = {8.0F, 9.0F, 66.F};
    std::array<float, 3> values6 = {2.0F, 1.2F, 11.F};

    SPTR(data::Float3TL::buffer_t) data1 = timeline->createBuffer(time1);
    data1->setElement(values1, 0);

    SPTR(data::Float3TL::buffer_t) data2 = timeline->createBuffer(time2);
    data2->setElement(values2, 0);
    data2->setElement(values3, 2);
    data2->setElement(values4, 4);

    SPTR(data::Float3TL::buffer_t) data3 = timeline->createBuffer(time3);
    data3->setElement(values5, 1);
    data3->setElement(values6, 2);

    SPTR(data::Float3TL::buffer_t) data4 = timeline->createBuffer(time4);

    timeline->pushObject(data1);
    timeline->pushObject(data2);
    timeline->pushObject(data3);
    timeline->pushObject(data4);

    CSPTR(data::Float3TL::buffer_t) obj1 = timeline->get_buffer(time1);

    /// First element
    data::Float3TL::buffer_t::iterator it = obj1->getPresenceIterator();
    CPPUNIT_ASSERT_EQUAL(1U, obj1->getPresentElementNum());

    std::array<float, 3> buff_data = *it;

    CPPUNIT_ASSERT_EQUAL(1.0F, buff_data[0]);
    CPPUNIT_ASSERT_EQUAL(5.2F, buff_data[1]);
    CPPUNIT_ASSERT_EQUAL(7.5F, buff_data[2]);

    ++it;
    CPPUNIT_ASSERT_EQUAL(false, it.isValid());

    /// Second element
    CSPTR(data::Float3TL::buffer_t) obj2 = timeline->get_buffer(time2);
    it                                   = obj2->getPresenceIterator();
    CPPUNIT_ASSERT_EQUAL(3U, obj2->getPresentElementNum());

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
    CPPUNIT_ASSERT_EQUAL(false, it.isValid());

    /// Third element
    CSPTR(data::Float3TL::buffer_t) obj3 = timeline->get_buffer(time3);
    it                                   = obj3->getPresenceIterator();
    CPPUNIT_ASSERT_EQUAL(2U, obj3->getPresentElementNum());

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
    CPPUNIT_ASSERT_EQUAL(false, it.isValid());

    /// Fourth element
    CSPTR(data::Float3TL::buffer_t) obj4 = timeline->get_buffer(time4);
    it                                   = obj4->getPresenceIterator();
    CPPUNIT_ASSERT_EQUAL(0U, obj4->getPresentElementNum());

    CPPUNIT_ASSERT_EQUAL(false, it.isValid());
}

//------------------------------------------------------------------------------

void generic_tl_test::objectValid()
{
    data::Float3TL::sptr timeline1 = std::make_shared<data::Float3TL>();
    timeline1->initPoolSize(3);
    data::Float4TL::sptr timeline2 = std::make_shared<data::Float4TL>();
    timeline2->initPoolSize(3);

    core::hires_clock::type time1 = core::hires_clock::get_time_in_milli_sec();
    SPTR(data::Float3TL::buffer_t) data1 = timeline1->createBuffer(time1);
    SPTR(data::Float4TL::buffer_t) data2 = timeline2->createBuffer(time1);

    CPPUNIT_ASSERT_EQUAL(false, timeline1->isObjectValid(data2));
    CPPUNIT_ASSERT_EQUAL(true, timeline1->isObjectValid(data1));

    CPPUNIT_ASSERT_EQUAL(false, timeline2->isObjectValid(data1));
    CPPUNIT_ASSERT_EQUAL(true, timeline2->isObjectValid(data2));
}

//------------------------------------------------------------------------------

void generic_tl_test::equalityTest()
{
    auto timeline1 = std::make_shared<data::Float3TL>();
    auto timeline2 = std::make_shared<data::Float3TL>();

    CPPUNIT_ASSERT(*timeline1 == *timeline2 && !(*timeline1 != *timeline2));

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(value) \
    timeline1->initPoolSize(value); \
    CPPUNIT_ASSERT_MESSAGE( \
        "Timelines should be different when the first is initialized with " #value, \
        *timeline1 != *timeline2 && !(*timeline1 == *timeline2) \
    ); \
    timeline2->initPoolSize(value); \
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
