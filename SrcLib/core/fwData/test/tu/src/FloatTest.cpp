/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Float.hpp>
#include "FloatTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FloatTest );

void FloatTest::setUp()
{
    // Set up context before running a test.

}

void FloatTest::tearDown()
{
    // Clean up after the test run.
}

void FloatTest::methode1()
{
    const float VALUE   = 2.04f ;
    // process
    ::fwData::Float::NewSptr p1( VALUE );
    // check
    CPPUNIT_ASSERT_EQUAL(p1->value(),   VALUE);

}
