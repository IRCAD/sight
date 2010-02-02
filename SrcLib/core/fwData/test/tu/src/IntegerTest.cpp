/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Integer.hpp>

#include "IntegerTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(IntegerTest );

void IntegerTest::setUp()
{
	// Set up context before running a test.
}

void IntegerTest::tearDown()
{
	// Clean up after the test run.
}

void IntegerTest::methode1()
{
	const int VALUE	= 13 ;

	// process
	::fwData::Integer::NewSptr p1(VALUE );

	// check
	CPPUNIT_ASSERT_EQUAL(p1->value(),	VALUE);

}
