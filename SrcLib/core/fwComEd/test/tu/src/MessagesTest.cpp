/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Object.hpp>
#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Image.hpp>
#include <fwData/PatientDB.hpp>

#include <fwComEd/FloatMsg.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/PatientDBMsg.hpp>

#include <fwServices/ObjectMsg.hpp>

#include "MessagesTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( MessagesTest );

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
	::fwComEd::FloatMsg::NewSptr floatMsg;
	floatMsg->addEvent(::fwComEd::FloatMsg::VALUE_IS_MODIFIED);

	// check Event
	CPPUNIT_ASSERT(floatMsg->hasEvent(::fwComEd::FloatMsg::VALUE_IS_MODIFIED));
}

//------------------------------------------------------------------------------

void MessagesTest::testImageMsg()
{
	const std::string MYMODIF1 = ::fwComEd::ImageMsg::BUFFER;
	const std::string MYMODIF2 = ::fwComEd::ImageMsg::WINDOWING;

	::fwData::Integer::sptr intField(new ::fwData::Integer(23));
	::fwData::Image::NewSptr image;

	// create message
	::fwComEd::ImageMsg::NewSptr imageMsg;
	imageMsg->addEvent(MYMODIF1);
	imageMsg->addEvent(MYMODIF2, intField);

	// check Event
	CPPUNIT_ASSERT(imageMsg->hasEvent(MYMODIF1));
	CPPUNIT_ASSERT(imageMsg->hasEvent(MYMODIF2));

	// check data info
	::fwData::Object::sptr obj = ::fwData::Object::constCast(imageMsg->getDataInfo(MYMODIF2));
	::fwData::Integer::sptr intField2 = ::fwData::Integer::dynamicCast(obj);
	CPPUNIT_ASSERT(intField2);
	CPPUNIT_ASSERT_EQUAL(intField, intField2);
}

//------------------------------------------------------------------------------

void MessagesTest::testPatientDBMsg()
{
	const std::string EVENT = ::fwComEd::PatientDBMsg::NEW_IMAGE_SELECTED;

	::fwData::PatientDB::NewSptr patientDB;
	::fwData::Image::sptr image = ::fwData::Image::New();

	// create message
	::fwComEd::PatientDBMsg::NewSptr patientDBMsg;
	patientDBMsg->addEvent(EVENT, image);

	//check event
	CPPUNIT_ASSERT(patientDBMsg->hasEvent(EVENT));

	// check data info
	::fwData::Object::sptr obj = ::fwData::Object::constCast(patientDBMsg->getDataInfo(EVENT));
	::fwData::Image::sptr img = ::fwData::Image::dynamicCast(obj);
	CPPUNIT_ASSERT(img);
	CPPUNIT_ASSERT_EQUAL(image, img);
}

//------------------------------------------------------------------------------
