/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/GenericField.hpp>
#include <fwData/Patient.hpp>

#include "GenericFieldTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( GenericFieldTest );

void GenericFieldTest::setUp()
{
    // Set up context before running a test.

}

void GenericFieldTest::tearDown()
{
    // Clean up after the test run.
}


void GenericFieldTest::methode1()
{
    typedef float TestType;

    const TestType VALUE = 13.456789 ;

    ::fwData::GenericField<TestType>::NewSptr p1( VALUE ) ;

    // check
    CPPUNIT_ASSERT_EQUAL( p1->value(),   VALUE);
}

