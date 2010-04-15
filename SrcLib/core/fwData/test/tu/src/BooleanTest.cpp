/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Boolean.hpp>

#include "BooleanTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( BooleanTest );

void BooleanTest::setUp()
{
    // Set up context before running a test.

}
void BooleanTest::tearDown()
{
    // Clean up after the test run.
}

void BooleanTest::methode1()
{
    const bool TRUE  = true ;
    const bool FALSE = not TRUE ;

    ::fwData::Boolean::NewSptr p1( TRUE );
    ::fwData::Boolean::NewSptr p2( FALSE );

    // check
    CPPUNIT_ASSERT_EQUAL(TRUE , ::fwData::Boolean::New( TRUE )->value());
    CPPUNIT_ASSERT_EQUAL(FALSE, ::fwData::Boolean::New( FALSE )->value());

    CPPUNIT_ASSERT_EQUAL(TRUE , p1->value());
    CPPUNIT_ASSERT_EQUAL(FALSE, p2->value());

}
