/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include <algorithm>

#include <arData/FrameTL.hpp>
#include <arData/timeline/Buffer.hpp>

#include <fwTest/Exception.hpp>

#include "FrameTLTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::arData::ut::FrameTLTest );

namespace arData
{
namespace ut
{

static ::fwTest::Exception fwTestException(""); // force link with fwTest

void FrameTLTest::setUp()
{
    // Set up context before running a test.

}
void FrameTLTest::tearDown()
{
    // Clean up after the test run.
}

void FrameTLTest::pushTest()
{
    ::arData::FrameTL::sptr timeline = ::arData::FrameTL::New();
    timeline->initPoolSize(10,20,::fwTools::Type::s_UINT8, 3);

    CPPUNIT_ASSERT_EQUAL(size_t(20),timeline->getHeight());
    CPPUNIT_ASSERT_EQUAL(size_t(10),timeline->getWidth());
    CPPUNIT_ASSERT_EQUAL(size_t(3),timeline->getNumberOfComponents());
    CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_UINT8,timeline->getType());

    ::fwCore::HiResClock::HiResClockType time1 = ::fwCore::HiResClock::getTimeInMilliSec();
    ::fwCore::HiResClock::HiResClockType time2 = time1 + 42;


    SPTR(::arData::FrameTL::BufferType) data1 = timeline->createBuffer(time1);
    ::fwTools::Type::UInt8Type* bufferData1   = data1->addElement(0);
    std::fill(bufferData1, bufferData1+(10*20*3), 1);
    SPTR(::arData::FrameTL::BufferType) data2 = timeline->createBuffer(time2);
    ::fwTools::Type::UInt8Type* bufferData2   = data2->addElement(0);
    std::fill(bufferData2, bufferData2+(10*20*3), 2);

    timeline->pushObject(data1);
    timeline->pushObject(data2);


    CSPTR(::arData::timeline::Object) dataPushed1 = timeline->getObject(time1);
    CPPUNIT_ASSERT(data1 == dataPushed1);

    CSPTR(::arData::timeline::Object) dataPushed2 = timeline->getObject(time2);
    CPPUNIT_ASSERT(data2 == dataPushed2);


    CSPTR(::arData::timeline::Object) dataPushed1Bis = timeline->getClosestObject(time1 + 1.5);
    CSPTR(::arData::FrameTL::BufferType) buff        =
        std::dynamic_pointer_cast< const ::arData::FrameTL::BufferType >(dataPushed1Bis);
    CPPUNIT_ASSERT(buff);
    CPPUNIT_ASSERT_EQUAL(buff, timeline->getClosestBuffer(time1 + 1.5));
    const ::fwTools::Type::UInt8Type* buffData = &buff->getElement(0);
    CPPUNIT_ASSERT_EQUAL(::fwTools::Type::UInt8Type(1), buffData[0]);
    CPPUNIT_ASSERT_EQUAL(::fwTools::Type::UInt8Type(1), buffData[300]);
    CPPUNIT_ASSERT_EQUAL(::fwTools::Type::UInt8Type(1), buffData[599]);

    CSPTR(::arData::timeline::Object) dataPushed2Bis = timeline->getNewerObject();
    CPPUNIT_ASSERT(data2 == dataPushed2Bis);

    ::fwCore::HiResClock::HiResClockType time2Pushed = timeline->getNewerTimestamp();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(time2, time2Pushed, 0.00001);

    timeline->clearTimeline();
    CSPTR(::arData::timeline::Object) nullObj = timeline->getNewerObject();
    CPPUNIT_ASSERT(nullObj == NULL);
}

//------------------------------------------------------------------------------


} //namespace ut
} //namespace arData
