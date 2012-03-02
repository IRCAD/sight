/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Boolean.hpp>

#include "BooleanTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::BooleanTest );

namespace fwData
{
namespace ut
{

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
    const bool TRUE_VALUE  = true ;
    const bool FALSE_VALUE = ! TRUE_VALUE ;

    ::fwData::Boolean::NewSptr p1( TRUE_VALUE );
    ::fwData::Boolean::NewSptr p2( FALSE_VALUE );

    // check
    CPPUNIT_ASSERT_EQUAL(TRUE_VALUE , ::fwData::Boolean::New( TRUE_VALUE )->value());
    CPPUNIT_ASSERT_EQUAL(FALSE_VALUE, ::fwData::Boolean::New( FALSE_VALUE )->value());

    CPPUNIT_ASSERT_EQUAL(TRUE_VALUE , p1->value());
    CPPUNIT_ASSERT_EQUAL(FALSE_VALUE, p2->value());

}

} //namespace ut
} //namespace fwData
