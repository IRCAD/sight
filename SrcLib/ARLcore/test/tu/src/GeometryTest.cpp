/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "GeometryTest.hpp"
#include <arlcore/UnitTestArlCore.h>


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::ARLcore::ut::GeometryTest );

namespace ARLcore
{
namespace ut
{

void GeometryTest::setUp()
{
    // Set up context before running a test.

}
void GeometryTest::tearDown()
{
    // Clean up after the test run.
}




void GeometryTest::ReconstructionPolyscopicTest()
{
    CPPUNIT_ASSERT_EQUAL( true, arlCore::testReconstructionPolyscopic( 6, 10e-10 ) );
}



void GeometryTest::SphereCenterEstimationTest()
{
    CPPUNIT_ASSERT_EQUAL( true, arlCore::testSphereCenterEstimation( 5000, 1e-4 ) );
}



void GeometryTest::RigidTransfoTest()
{
    CPPUNIT_ASSERT_EQUAL( true, arlCore::testRigidTransfo( 1000000, 1e-5 ) );
}



void GeometryTest::EpipolarMatchingTest()
{
    CPPUNIT_ASSERT_EQUAL( true, arlCore::testRegistration3D3DwithoutMatching( 10000, 1e-5 ) );
}




} //namespace ut
} //namespace ARLcore
