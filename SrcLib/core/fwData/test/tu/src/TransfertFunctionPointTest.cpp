/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <exception>
#include <vector>
#include <ostream>
#include <map>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "fwData/TransfertFunctionPoint.hpp"
#include "fwData/Color.hpp"
#include "TransfertFunctionPointTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( TransfertFunctionPointTest );

void TransfertFunctionPointTest::setUp()
{
	// Set up context before running a test.

}
void TransfertFunctionPointTest::tearDown()
{
	// Clean up after the test run.
}

void TransfertFunctionPointTest::methode1()
{
	//-----------test values
	const ::fwData::Color::sptr COLOR(new ::fwData::Color(0.5f, 0.4f ,0.6f, 0.1f));
	const ::boost::int32_t INDEX = 32;

	::fwData::TransfertFunctionPoint::NewSptr tfp;

	tfp->setValue(INDEX);
	tfp->setColor(COLOR);


	CPPUNIT_ASSERT_EQUAL(tfp->getValue(), INDEX);
	CPPUNIT_ASSERT_EQUAL(tfp->getColor()->getRGBA()[0], COLOR->getRGBA()[0]);
	CPPUNIT_ASSERT_EQUAL(tfp->getColor()->getRGBA()[1], COLOR->getRGBA()[1]);
}



