/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Object.hpp>
#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Image.hpp>

#include <fwComEd/FloatMsg.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/helper/MsgHelper.hpp>

#include <fwServices/ObjectMsg.hpp>

#include "MessagesTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwComEd::ut::MessagesTest );

namespace fwComEd
{
namespace ut
{

//------------------------------------------------------------------------------

void MessagesTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void MessagesTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void MessagesTest::testFloatMsg()
{
    // create message
    ::fwComEd::FloatMsg::sptr floatMsg = ::fwComEd::FloatMsg::New();
    floatMsg->addEvent(::fwComEd::FloatMsg::VALUE_IS_MODIFIED);

    // check Event
    CPPUNIT_ASSERT(floatMsg->hasEvent(::fwComEd::FloatMsg::VALUE_IS_MODIFIED));
}

//------------------------------------------------------------------------------

void MessagesTest::testImageMsg()
{
    const std::string MYMODIF1 = ::fwComEd::ImageMsg::BUFFER;
    const std::string MYMODIF2 = ::fwComEd::ImageMsg::SLICE_INDEX;

    ::fwData::Integer::sptr intField = ::fwData::Integer::New(23);
    ::fwData::Image::sptr image      = ::fwData::Image::New();

    // create message
    ::fwComEd::ImageMsg::sptr imageMsg = ::fwComEd::ImageMsg::New();
    imageMsg->addEvent(MYMODIF1);
    imageMsg->addEvent(MYMODIF2, intField);

    // check Event
    CPPUNIT_ASSERT(imageMsg->hasEvent(MYMODIF1));
    CPPUNIT_ASSERT(imageMsg->hasEvent(MYMODIF2));

    // check data info
    ::fwData::Object::sptr obj        = ::fwData::Object::constCast(imageMsg->getDataInfo(MYMODIF2));
    ::fwData::Integer::sptr intField2 = ::fwData::Integer::dynamicCast(obj);
    CPPUNIT_ASSERT(intField2);
    CPPUNIT_ASSERT_EQUAL(intField, intField2);
}

//------------------------------------------------------------------------------

void MessagesTest::messageCreationTest()
{
    ::fwData::Image::sptr image       = ::fwData::Image::New();
    ::fwServices::ObjectMsg::sptr msg = ::fwComEd::helper::MsgHelper::createAssociatedMsg(image);
    CPPUNIT_ASSERT(msg);
    CPPUNIT_ASSERT(::fwComEd::ImageMsg::dynamicCast(msg));
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwComEd
