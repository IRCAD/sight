/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "RegistrationTest.hpp"
#include <arlcore/UnitTestArlCore.h>


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::ARLcore::ut::RegistrationTest );

namespace ARLcore
{
namespace ut
{

void RegistrationTest::setUp()
{
    // Set up context before running a test.

}
void RegistrationTest::tearDown()
{
    // Clean up after the test run.
}



void RegistrationTest::HomographyRegistrationTest()
{
    CPPUNIT_ASSERT_EQUAL( true, arlCore::testHomographyRegistration( 10000, 1e-2 ) );
}



void RegistrationTest::ProjectiveRegistrationTest()
{
    CPPUNIT_ASSERT_EQUAL( true, arlCore::testProjectiveRegistration( 10, 1e-2 ) );
}



void RegistrationTest::Registration3D3DTest()
{
    CPPUNIT_ASSERT_EQUAL( true, arlCore::testRegistration3D3D( 15000, 1e-10 ) );
}



void RegistrationTest::ICPRegistrationTest()
{
    CPPUNIT_ASSERT_EQUAL( true, arlCore::testICPRegistration( 3, 1e-11 ) );
}



void RegistrationTest::threeD3DMatchingTest()
{
    CPPUNIT_ASSERT_EQUAL( true, arlCore::testRegistration3D3D( 15000, 1e-10 ) );
}



void RegistrationTest::registration3D3DwithoutMatchingTest()
{
    CPPUNIT_ASSERT_EQUAL( true, arlCore::testRegistration3D3DwithoutMatching( 10000, 1e-5 ) );
}




} //namespace ut
} //namespace ARLcore
