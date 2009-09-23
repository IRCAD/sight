/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/Object.hpp>

#include "FieldTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FieldTest );

void FieldTest::setUp()
{
	// Set up context before running a test.
}

void FieldTest::tearDown()
{
	// Clean up after the test run.
}

void FieldTest::methode1()
{
	std::string LABEL	= "label" ;

	// process
	::fwTools::Field::NewSptr p1(LABEL);

	// check
	CPPUNIT_ASSERT_EQUAL(p1->label(),	LABEL);

}
