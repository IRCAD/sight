/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Image.hpp>

#include <fwServices/ObjectMsg.hpp>

#include "ObjectMsgTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ObjectMsgTest );

//------------------------------------------------------------------------------

void ObjectMsgTest::setUp()
{
        // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ObjectMsgTest::tearDown()
{
        // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ObjectMsgTest::testObjectMsg()
{
        const std::string EVENT1 = "EVENT1";
        const std::string EVENT2 = "EVENT2";

        ::fwData::Image::sptr image = ::fwData::Image::New();

        // create message with events
        ::fwServices::ObjectMsg::NewSptr objMsg;
        objMsg->addEvent(EVENT1);
        objMsg->addEvent(EVENT2, image);


        // check events
        CPPUNIT_ASSERT(objMsg->hasEvent(EVENT1));
        CPPUNIT_ASSERT(objMsg->hasEvent(EVENT2));

        // check event object
        ::fwData::Object::sptr obj = ::fwData::Object::constCast( objMsg->getDataInfo( EVENT2 ) );
        ::fwData::Image::sptr img = ::fwData::Image::dynamicCast( obj );

        CPPUNIT_ASSERT_EQUAL(image, img);
}

//------------------------------------------------------------------------------
