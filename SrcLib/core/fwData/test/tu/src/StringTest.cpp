/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include "fwData/String.hpp"
#include "StringTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( StringTest );

void StringTest::setUp()
{
	// Set up context before running a test.
}

void StringTest::tearDown()
{
	// Clean up after the test run.
}

void StringTest::methode1()
{
	const std::string VALUE	= "chaine_caractere" ;

	// process
	::fwData::String::NewSptr p1( VALUE );

	// check
	CPPUNIT_ASSERT_EQUAL(p1->value(),	VALUE);

}
