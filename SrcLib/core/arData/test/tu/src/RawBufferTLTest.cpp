/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arData/RawBufferTL.hpp>
#include <arData/timeline/Buffer.hpp>

#include <fwTest/Exception.hpp>

#include "RawBufferTLTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::arData::ut::RawBufferTLTest );

namespace arData
{
namespace ut
{

static ::fwTest::Exception fwTestException(""); // force link with fwTest

void RawBufferTLTest::setUp()
{
    // Set up context before running a test.

}
void RawBufferTLTest::tearDown()
{
    // Clean up after the test run.
}

void RawBufferTLTest::pushTest()
{
    ::arData::RawBufferTL::sptr timeline = ::arData::RawBufferTL::New();
    timeline->initPoolSize(3* sizeof(float));

    ::fwCore::HiResClock::HiResClockType time1 = ::fwCore::HiResClock::getTimeInMilliSec();
    ::fwCore::HiResClock::HiResClockType time2 = time1 + 42;

    float values1[3] = {1.0f, 5.2f, 7.5f};
    float values2[3] = {8.0f, 9.0f, 66.f};
    SPTR(::arData::timeline::RawBuffer) data1 = timeline->createBuffer(time1);
    std::copy(values1, values1 + 3, data1->getBuffer<float>());
    SPTR(::arData::timeline::RawBuffer) data2 = timeline->createBuffer(time2);
    std::copy(values2, values2 + 3, data2->getBuffer<float>());

    timeline->pushObject(data1);
    timeline->pushObject(data2);


    CSPTR(::arData::timeline::Object) dataPushed1 = timeline->getObject(time1);
    CPPUNIT_ASSERT(data1 == dataPushed1);

    CSPTR(::arData::timeline::Object) dataPushed2 = timeline->getObject(time2);
    CPPUNIT_ASSERT(data2 == dataPushed2);


    CSPTR(::arData::timeline::Object) dataPushed1Bis = timeline->getClosestObject(time1 + 1.5);
    CSPTR(::arData::timeline::RawBuffer) buff        =
        std::dynamic_pointer_cast< const ::arData::timeline::RawBuffer >(dataPushed1Bis);
    CPPUNIT_ASSERT(buff);
    CPPUNIT_ASSERT_EQUAL(buff, timeline->getClosestBuffer(time1 + 1.5));
    float* buffData = buff->getBuffer<float>();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, buffData[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5.2, buffData[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(7.5, buffData[2], 0.00001);

    CSPTR(::arData::timeline::Object) dataPushed2Bis = timeline->getNewerObject();
    CPPUNIT_ASSERT(data2 == dataPushed2Bis);

    ::fwCore::HiResClock::HiResClockType time2Pushed = timeline->getNewerTimestamp();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(time2, time2Pushed, 0.00001);

    timeline->clearTimeline();
    CSPTR(::arData::timeline::Object) nullObj = timeline->getNewerObject();
    CPPUNIT_ASSERT(nullObj == NULL);
}

//------------------------------------------------------------------------------

void RawBufferTLTest::getObjectTest()
{
    ::arData::RawBufferTL::sptr timeline = ::arData::RawBufferTL::New();
    timeline->initPoolSize(3* sizeof(float));

    float values[3] = {1.0f, 5.2f, 7.5f};

    SPTR(::arData::timeline::RawBuffer) data1 = timeline->createBuffer(1);
    std::copy(values, values + 3, data1->getBuffer<float>());
    SPTR(::arData::timeline::RawBuffer) data2 = timeline->createBuffer(2);
    std::copy(values, values + 3, data2->getBuffer<float>());
    SPTR(::arData::timeline::RawBuffer) data3 = timeline->createBuffer(3);
    std::copy(values, values + 3, data3->getBuffer<float>());
    SPTR(::arData::timeline::RawBuffer) data4 = timeline->createBuffer(4);
    std::copy(values, values + 3, data4->getBuffer<float>());

    timeline->pushObject(data1);
    timeline->pushObject(data2);
    timeline->pushObject(data3);
    timeline->pushObject(data4);


    CSPTR(::arData::timeline::Object) obj;

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
    obj = timeline->getClosestObject(2, ::arData::BufferTL::PAST);
    CPPUNIT_ASSERT(data2 == obj);
    obj = timeline->getClosestObject(2, ::arData::BufferTL::FUTURE);
    CPPUNIT_ASSERT(data2 == obj);
    obj = timeline->getClosestObject(2, ::arData::BufferTL::BOTH);
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

    obj = timeline->getClosestObject(0.1, ::arData::TimeLine::PAST);
    CPPUNIT_ASSERT(NULL == obj);
    obj = timeline->getClosestObject(0.1, ::arData::TimeLine::FUTURE);
    CPPUNIT_ASSERT(data1 == obj);

    obj = timeline->getClosestObject(1.1, ::arData::TimeLine::PAST);
    CPPUNIT_ASSERT(data1 == obj);
    obj = timeline->getClosestObject(1.1, ::arData::TimeLine::FUTURE);
    CPPUNIT_ASSERT(data2 == obj);

    obj = timeline->getClosestObject(1.9, ::arData::TimeLine::PAST);
    CPPUNIT_ASSERT(data1 == obj);
    obj = timeline->getClosestObject(1.9, ::arData::TimeLine::FUTURE);
    CPPUNIT_ASSERT(data2 == obj);

    obj = timeline->getClosestObject(2.1, ::arData::TimeLine::PAST);
    CPPUNIT_ASSERT(data2 == obj);
    obj = timeline->getClosestObject(2.1, ::arData::TimeLine::FUTURE);
    CPPUNIT_ASSERT(data3 == obj);

    obj = timeline->getClosestObject(2.8, ::arData::TimeLine::PAST);
    CPPUNIT_ASSERT(data2 == obj);
    obj = timeline->getClosestObject(2.8, ::arData::TimeLine::FUTURE);
    CPPUNIT_ASSERT(data3 == obj);

    obj = timeline->getClosestObject(3.1, ::arData::TimeLine::PAST);
    CPPUNIT_ASSERT(data3 == obj);
    obj = timeline->getClosestObject(3.1, ::arData::TimeLine::FUTURE);
    CPPUNIT_ASSERT(data4 == obj);

    obj = timeline->getClosestObject(3.9, ::arData::TimeLine::PAST);
    CPPUNIT_ASSERT(data3 == obj);
    obj = timeline->getClosestObject(3.9, ::arData::TimeLine::FUTURE);
    CPPUNIT_ASSERT(data4 == obj);

    obj = timeline->getClosestObject(4.1, ::arData::TimeLine::PAST);
    CPPUNIT_ASSERT(data4 == obj);
    obj = timeline->getClosestObject(4.1, ::arData::TimeLine::FUTURE);
    CPPUNIT_ASSERT(NULL == obj);

}

//------------------------------------------------------------------------------

void RawBufferTLTest::setObjectTest()
{
    ::arData::RawBufferTL::sptr timeline = ::arData::RawBufferTL::New();
    timeline->initPoolSize(3* sizeof(float));

    float values[3] = {1.0f, 5.2f, 7.5f};

    SPTR(::arData::timeline::RawBuffer) data1 = timeline->createBuffer(1);
    std::copy(values, values + 3, data1->getBuffer<float>());
    SPTR(::arData::timeline::RawBuffer) data2 = timeline->createBuffer(2);
    std::copy(values, values + 3, data2->getBuffer<float>());
    SPTR(::arData::timeline::RawBuffer) data3 = timeline->createBuffer(3);
    std::copy(values, values + 3, data3->getBuffer<float>());
    SPTR(::arData::timeline::RawBuffer) data4 = timeline->createBuffer(4);
    std::copy(values, values + 3, data4->getBuffer<float>());

    timeline->pushObject(data1);
    timeline->pushObject(data2);
    timeline->pushObject(data3);
    timeline->pushObject(data4);


    CSPTR(::arData::timeline::Object) obj;

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
    ::arData::RawBufferTL::sptr timeline = ::arData::RawBufferTL::New();
    timeline->initPoolSize(3* sizeof(float));

    float values[3] = {2.0f, 1.2f, 6.5f};

    SPTR(::arData::timeline::RawBuffer) data1 = timeline->createBuffer(1);
    std::copy(values, values + 3, data1->getBuffer<float>());
    SPTR(::arData::timeline::RawBuffer) data2 = timeline->createBuffer(2);
    std::copy(values, values + 3, data2->getBuffer<float>());
    SPTR(::arData::timeline::RawBuffer) data3 = timeline->createBuffer(3);
    std::copy(values, values + 3, data3->getBuffer<float>());
    SPTR(::arData::timeline::RawBuffer) data4 = timeline->createBuffer(4);
    std::copy(values, values + 3, data4->getBuffer<float>());

    timeline->pushObject(data1);
    timeline->pushObject(data2);
    timeline->pushObject(data3);
    timeline->pushObject(data4);

    CSPTR(::arData::timeline::Object) obj;

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
    ::arData::RawBufferTL::sptr timeline = ::arData::RawBufferTL::New();
    timeline->initPoolSize(3* sizeof(float));

    ::fwCore::HiResClock::HiResClockType time1 = ::fwCore::HiResClock::getTimeInMilliSec();
    ::fwCore::HiResClock::HiResClockType time2 = time1 + 42;

    float values1[3] = {1.0f, 5.2f, 7.5f};
    float values2[3] = {8.0f, 9.0f, 66.f};
    SPTR(::arData::timeline::RawBuffer) data1 = timeline->createBuffer(time1);
    std::copy(values1, values1 + 3, data1->getBuffer<float>());
    SPTR(::arData::timeline::RawBuffer) data2 = timeline->createBuffer(time2);
    std::copy(values2, values2 + 3, data2->getBuffer<float>());

    timeline->pushObject(data1);
    timeline->pushObject(data2);

    // deepCopy test
    ::arData::RawBufferTL::sptr deepTimeline = ::arData::RawBufferTL::New();
    deepTimeline->deepCopy(timeline);
    CSPTR(::arData::timeline::Object) deepDataPushed1 = deepTimeline->getObject(time1);
    CPPUNIT_ASSERT(deepDataPushed1);
    CPPUNIT_ASSERT(data1 != deepDataPushed1);
    CSPTR(::arData::timeline::RawBuffer) buff1 =
        std::dynamic_pointer_cast< const ::arData::timeline::RawBuffer >(deepDataPushed1);
    CPPUNIT_ASSERT(buff1);
    CPPUNIT_ASSERT_EQUAL(buff1, deepTimeline->getBuffer(time1));
    float* buffData1 = buff1->getBuffer<float>();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, buffData1[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5.2, buffData1[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(7.5, buffData1[2], 0.00001);

    CSPTR(::arData::timeline::Object) deepDataPushed2 = deepTimeline->getObject(time2);
    CPPUNIT_ASSERT(deepDataPushed2);
    CPPUNIT_ASSERT(data2 != deepDataPushed2);
    CSPTR(::arData::timeline::RawBuffer) buff2 =
        std::dynamic_pointer_cast< const ::arData::timeline::RawBuffer >(deepDataPushed2);
    CPPUNIT_ASSERT(buff2);
    float* buffData2 = buff2->getBuffer<float>();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(8.0, buffData2[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0, buffData2[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(66., buffData2[2], 0.00001);
}

} //namespace ut
} //namespace arData
