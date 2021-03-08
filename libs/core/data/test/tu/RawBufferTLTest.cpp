/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "RawBufferTLTest.hpp"

#include <data/RawBufferTL.hpp>
#include <data/timeline/Buffer.hpp>

#include <utest/Exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::data::ut::RawBufferTLTest );

namespace sight::data
{
namespace ut
{

//------------------------------------------------------------------------------

void RawBufferTLTest::setUp()
{
    // Set up context before running a test.

}
//------------------------------------------------------------------------------

void RawBufferTLTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void RawBufferTLTest::pushTest()
{
    data::RawBufferTL::sptr timeline = data::RawBufferTL::New();
    timeline->initPoolSize(3* sizeof(float));

    core::HiResClock::HiResClockType time1 = core::HiResClock::getTimeInMilliSec();
    core::HiResClock::HiResClockType time2 = time1 + 42;

    float values1[3] = {1.0f, 5.2f, 7.5f};
    float values2[3] = {8.0f, 9.0f, 66.f};
    SPTR(data::timeline::RawBuffer) data1 = timeline->createBuffer(time1);
    std::copy(values1, values1 + 3, data1->getBuffer<float>());
    SPTR(data::timeline::RawBuffer) data2 = timeline->createBuffer(time2);
    std::copy(values2, values2 + 3, data2->getBuffer<float>());

    timeline->pushObject(data1);
    timeline->pushObject(data2);

    CSPTR(data::timeline::Object) dataPushed1 = timeline->getObject(time1);
    CPPUNIT_ASSERT(data1 == dataPushed1);

    CSPTR(data::timeline::Object) dataPushed2 = timeline->getObject(time2);
    CPPUNIT_ASSERT(data2 == dataPushed2);

    CSPTR(data::timeline::Object) dataPushed1Bis = timeline->getClosestObject(time1 + 1.5);
    CSPTR(data::timeline::RawBuffer) buff        =
        std::dynamic_pointer_cast< const data::timeline::RawBuffer >(dataPushed1Bis);
    CPPUNIT_ASSERT(buff);
    CPPUNIT_ASSERT_EQUAL(buff, timeline->getClosestBuffer(time1 + 1.5));
    float* buffData = buff->getBuffer<float>();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, buffData[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5.2, buffData[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(7.5, buffData[2], 0.00001);

    CSPTR(data::timeline::Object) dataPushed2Bis = timeline->getNewerObject();
    CPPUNIT_ASSERT(data2 == dataPushed2Bis);

    core::HiResClock::HiResClockType time2Pushed = timeline->getNewerTimestamp();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(time2, time2Pushed, 0.00001);

    timeline->clearTimeline();
    CSPTR(data::timeline::Object) nullObj = timeline->getNewerObject();
    CPPUNIT_ASSERT(nullObj == NULL);
}

//------------------------------------------------------------------------------

void RawBufferTLTest::getObjectTest()
{
    data::RawBufferTL::sptr timeline = data::RawBufferTL::New();
    timeline->initPoolSize(3* sizeof(float));

    float values[3] = {1.0f, 5.2f, 7.5f};

    SPTR(data::timeline::RawBuffer) data1 = timeline->createBuffer(1);
    std::copy(values, values + 3, data1->getBuffer<float>());
    SPTR(data::timeline::RawBuffer) data2 = timeline->createBuffer(2);
    std::copy(values, values + 3, data2->getBuffer<float>());
    SPTR(data::timeline::RawBuffer) data3 = timeline->createBuffer(3);
    std::copy(values, values + 3, data3->getBuffer<float>());
    SPTR(data::timeline::RawBuffer) data4 = timeline->createBuffer(4);
    std::copy(values, values + 3, data4->getBuffer<float>());

    timeline->pushObject(data1);
    timeline->pushObject(data2);
    timeline->pushObject(data3);
    timeline->pushObject(data4);

    CSPTR(data::timeline::Object) obj;

    obj = timeline->getObject(1);
    CPPUNIT_ASSERT(data1 == obj);
    obj = timeline->getClosestObject(0.1);
    CPPUNIT_ASSERT(data1 == obj);
    obj = timeline->getClosestObject(1.1);
    CPPUNIT_ASSERT(data1 == obj);

    obj = timeline->getObject(2);
    CPPUNIT_ASSERT(data2 == obj);
    obj = timeline->getClosestObject(1.8);
    CPPUNIT_ASSERT(data2 == obj);
    obj = timeline->getClosestObject(2, data::BufferTL::PAST);
    CPPUNIT_ASSERT(data2 == obj);
    obj = timeline->getClosestObject(2, data::BufferTL::FUTURE);
    CPPUNIT_ASSERT(data2 == obj);
    obj = timeline->getClosestObject(2, data::BufferTL::BOTH);
    CPPUNIT_ASSERT(data2 == obj);
    obj = timeline->getClosestObject(2.2);
    CPPUNIT_ASSERT(data2 == obj);

    obj = timeline->getObject(3);
    CPPUNIT_ASSERT(data3 == obj);
    obj = timeline->getClosestObject(2.6);
    CPPUNIT_ASSERT(data3 == obj);
    obj = timeline->getClosestObject(3.1);
    CPPUNIT_ASSERT(data3 == obj);

    obj = timeline->getObject(4);
    CPPUNIT_ASSERT(data4 == obj);
    obj = timeline->getClosestObject(3.9);
    CPPUNIT_ASSERT(data4 == obj);
    obj = timeline->getClosestObject(4.2);
    CPPUNIT_ASSERT(data4 == obj);

    obj = timeline->getClosestObject(0.1, data::TimeLine::PAST);
    CPPUNIT_ASSERT(NULL == obj);
    obj = timeline->getClosestObject(0.1, data::TimeLine::FUTURE);
    CPPUNIT_ASSERT(data1 == obj);

    obj = timeline->getClosestObject(1.1, data::TimeLine::PAST);
    CPPUNIT_ASSERT(data1 == obj);
    obj = timeline->getClosestObject(1.1, data::TimeLine::FUTURE);
    CPPUNIT_ASSERT(data2 == obj);

    obj = timeline->getClosestObject(1.9, data::TimeLine::PAST);
    CPPUNIT_ASSERT(data1 == obj);
    obj = timeline->getClosestObject(1.9, data::TimeLine::FUTURE);
    CPPUNIT_ASSERT(data2 == obj);

    obj = timeline->getClosestObject(2.1, data::TimeLine::PAST);
    CPPUNIT_ASSERT(data2 == obj);
    obj = timeline->getClosestObject(2.1, data::TimeLine::FUTURE);
    CPPUNIT_ASSERT(data3 == obj);

    obj = timeline->getClosestObject(2.8, data::TimeLine::PAST);
    CPPUNIT_ASSERT(data2 == obj);
    obj = timeline->getClosestObject(2.8, data::TimeLine::FUTURE);
    CPPUNIT_ASSERT(data3 == obj);

    obj = timeline->getClosestObject(3.1, data::TimeLine::PAST);
    CPPUNIT_ASSERT(data3 == obj);
    obj = timeline->getClosestObject(3.1, data::TimeLine::FUTURE);
    CPPUNIT_ASSERT(data4 == obj);

    obj = timeline->getClosestObject(3.9, data::TimeLine::PAST);
    CPPUNIT_ASSERT(data3 == obj);
    obj = timeline->getClosestObject(3.9, data::TimeLine::FUTURE);
    CPPUNIT_ASSERT(data4 == obj);

    obj = timeline->getClosestObject(4.1, data::TimeLine::PAST);
    CPPUNIT_ASSERT(data4 == obj);
    obj = timeline->getClosestObject(4.1, data::TimeLine::FUTURE);
    CPPUNIT_ASSERT(NULL == obj);

}

//------------------------------------------------------------------------------

void RawBufferTLTest::setObjectTest()
{
    data::RawBufferTL::sptr timeline = data::RawBufferTL::New();
    timeline->initPoolSize(3* sizeof(float));

    float values[3] = {1.0f, 5.2f, 7.5f};

    SPTR(data::timeline::RawBuffer) data1 = timeline->createBuffer(1);
    std::copy(values, values + 3, data1->getBuffer<float>());
    SPTR(data::timeline::RawBuffer) data2 = timeline->createBuffer(2);
    std::copy(values, values + 3, data2->getBuffer<float>());
    SPTR(data::timeline::RawBuffer) data3 = timeline->createBuffer(3);
    std::copy(values, values + 3, data3->getBuffer<float>());
    SPTR(data::timeline::RawBuffer) data4 = timeline->createBuffer(4);
    std::copy(values, values + 3, data4->getBuffer<float>());

    timeline->pushObject(data1);
    timeline->pushObject(data2);
    timeline->pushObject(data3);
    timeline->pushObject(data4);

    CSPTR(data::timeline::Object) obj;

    timeline->setObject(1, data2);
    timeline->setObject(2, data3);
    timeline->setObject(4, data3);

    obj = timeline->getObject(1);
    CPPUNIT_ASSERT(data2 == obj);

    obj = timeline->getClosestObject(2.2);
    CPPUNIT_ASSERT(data3 == obj);

    obj = timeline->getClosestObject(3.8);
    CPPUNIT_ASSERT(data3 == obj);

    obj = timeline->getObject(3);
    CPPUNIT_ASSERT(data3 == obj);

    timeline->setObject(3, data1);

    obj = timeline->getObject(3);
    CPPUNIT_ASSERT(data1 == obj);
}

//------------------------------------------------------------------------------

void RawBufferTLTest::modifyTimeTest()
{
    data::RawBufferTL::sptr timeline = data::RawBufferTL::New();
    timeline->initPoolSize(3* sizeof(float));

    float values[3] = {2.0f, 1.2f, 6.5f};

    SPTR(data::timeline::RawBuffer) data1 = timeline->createBuffer(1);
    std::copy(values, values + 3, data1->getBuffer<float>());
    SPTR(data::timeline::RawBuffer) data2 = timeline->createBuffer(2);
    std::copy(values, values + 3, data2->getBuffer<float>());
    SPTR(data::timeline::RawBuffer) data3 = timeline->createBuffer(3);
    std::copy(values, values + 3, data3->getBuffer<float>());
    SPTR(data::timeline::RawBuffer) data4 = timeline->createBuffer(4);
    std::copy(values, values + 3, data4->getBuffer<float>());

    timeline->pushObject(data1);
    timeline->pushObject(data2);
    timeline->pushObject(data3);
    timeline->pushObject(data4);

    CSPTR(data::timeline::Object) obj;

    obj = timeline->getObject(1);
    CPPUNIT_ASSERT(data1 == obj);
    obj = timeline->getClosestObject(0.1);
    CPPUNIT_ASSERT(data1 == obj);
    obj = timeline->getClosestObject(1.1);
    CPPUNIT_ASSERT(data1 == obj);

    obj = timeline->getObject(2);
    CPPUNIT_ASSERT(data2 == obj);
    obj = timeline->getClosestObject(4.1);
    CPPUNIT_ASSERT(data4 == obj);

    timeline->modifyTime(1, 5);
    timeline->modifyTime(2, 1);
    timeline->modifyTime(5, 6);
    timeline->modifyTime(3, 7);

    obj = timeline->getObject(1);
    CPPUNIT_ASSERT(data2 == obj);

    obj = timeline->getClosestObject(6.2);
    CPPUNIT_ASSERT(data1 == obj);

    obj = timeline->getClosestObject(3.1);
    CPPUNIT_ASSERT(data4 == obj);

    obj = timeline->getObject(7);
    CPPUNIT_ASSERT(data3 == obj);
}

//------------------------------------------------------------------------------

void RawBufferTLTest::copyTest()
{
    data::RawBufferTL::sptr timeline = data::RawBufferTL::New();
    timeline->initPoolSize(3* sizeof(float));

    core::HiResClock::HiResClockType time1 = core::HiResClock::getTimeInMilliSec();
    core::HiResClock::HiResClockType time2 = time1 + 42;

    float values1[3] = {1.0f, 5.2f, 7.5f};
    float values2[3] = {8.0f, 9.0f, 66.f};
    SPTR(data::timeline::RawBuffer) data1 = timeline->createBuffer(time1);
    std::copy(values1, values1 + 3, data1->getBuffer<float>());
    SPTR(data::timeline::RawBuffer) data2 = timeline->createBuffer(time2);
    std::copy(values2, values2 + 3, data2->getBuffer<float>());

    timeline->pushObject(data1);
    timeline->pushObject(data2);

    // deepCopy test
    data::RawBufferTL::sptr deepTimeline = data::RawBufferTL::New();
    deepTimeline->deepCopy(timeline);
    CSPTR(data::timeline::Object) deepDataPushed1 = deepTimeline->getObject(time1);
    CPPUNIT_ASSERT(deepDataPushed1);
    CPPUNIT_ASSERT(data1 != deepDataPushed1);
    CSPTR(data::timeline::RawBuffer) buff1 =
        std::dynamic_pointer_cast< const data::timeline::RawBuffer >(deepDataPushed1);
    CPPUNIT_ASSERT(buff1);
    CPPUNIT_ASSERT_EQUAL(buff1, deepTimeline->getBuffer(time1));
    float* buffData1 = buff1->getBuffer<float>();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, buffData1[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5.2, buffData1[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(7.5, buffData1[2], 0.00001);

    CSPTR(data::timeline::Object) deepDataPushed2 = deepTimeline->getObject(time2);
    CPPUNIT_ASSERT(deepDataPushed2);
    CPPUNIT_ASSERT(data2 != deepDataPushed2);
    CSPTR(data::timeline::RawBuffer) buff2 =
        std::dynamic_pointer_cast< const data::timeline::RawBuffer >(deepDataPushed2);
    CPPUNIT_ASSERT(buff2);
    float* buffData2 = buff2->getBuffer<float>();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(8.0, buffData2[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0, buffData2[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(66., buffData2[2], 0.00001);
}

} //namespace ut
} //namespace sight::data
