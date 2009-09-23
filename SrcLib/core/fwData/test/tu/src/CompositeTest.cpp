/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <exception>
#include <vector>
#include <ostream>
#include <map>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "fwData/Composite.hpp"
#include "CompositeTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( CompositeTest );

void CompositeTest::setUp()
{
	// Set up context before running a test.

}
void CompositeTest::tearDown()
{
	// Clean up after the test run.
}

void CompositeTest::methode1()
{
	//-----------test values
	const std::string STR = "toto";
	::fwData::Object::sptr obj = ::fwData::Object::New();

	::fwData::Composite::NewSptr compo;

	compo->getRefMap()[STR] = obj;

	CPPUNIT_ASSERT(compo->getRefMap().find(STR) != compo->getRefMap().end());
	CPPUNIT_ASSERT_EQUAL(compo->getRefMap()[STR], obj);
}



