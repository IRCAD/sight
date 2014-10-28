/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwMath/VectorFunctions.hpp>

#include <fwCore/base.hpp>

#include "VectorFunctionsTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMath::ut::VectorFunctionsTest );

namespace fwMath
{
namespace ut
{

void VectorFunctionsTest::setUp()
{
    // Set up context before running a test.

}

//------------------------------------------------------------------------------

void VectorFunctionsTest::tearDown()
{
    // Clean up after the test run.

}

//------------------------------------------------------------------------------

void VectorFunctionsTest::checkDot()
{
    // Produit scalaire.
    const double V1_X = 0.1;
    const double V1_Y =rand()%30+0.1;
    const double V1_Z =rand()%20+0.4;

    const double V2_X = rand()%50+0.4;
    const double V2_Y =0.5;
    const double V2_Z =rand()%10+0.8;

    const fwVec3d V1 = {V1_X, V1_Y, V1_Z};
    const fwVec3d V2 = {V2_X, V2_Y, V2_Z};
    double result;
    result = ::fwMath::dot(V1, V2);

    double dotResult = V1_X*V2_X + V1_Y*V2_Y + V1_Z*V2_Z;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(dotResult,result, 0.00001);
}

//------------------------------------------------------------------------------

void VectorFunctionsTest::checkCross()
{
    // Produit vectoriel.
    const double DISTANCE = 10.25;

    // New version
    const double CROOS_X = -0.03;
    const double CROSS_Y = 0.06;
    const double CROSS_Z = -0.03;
    const fwVec3d V1 = {0.1, 0.2, 0.3};
    const fwVec3d V2 = {0.4, 0.5, 0.6};

    fwVec3d result;
    result = ::fwMath::cross(V1, V2);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(CROOS_X, result[0],  0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(CROSS_Y, result[1],  0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(CROSS_Z, result[2],  0.00001);
}

//------------------------------------------------------------------------------

void VectorFunctionsTest::checkNormalize()
{
    const double NORME = 0.87749;
    fwVec3d V = {0.4, 0.5, 0.6};
    double norme = ::fwMath::normalize(V);


    CPPUNIT_ASSERT_DOUBLES_EQUAL(NORME, norme, 0.00001);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.4/NORME, V[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5/NORME, V[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.6/NORME, V[2], 0.00001);


}

//------------------------------------------------------------------------------

void VectorFunctionsTest::checkNegate()
{
    fwVec3d V = {1.0, 2.0, 3.0};

    ::fwMath::negate(V);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-1.0), V[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-2.0), V[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-3.0), V[2], 0.00001);

}

//------------------------------------------------------------------------------

void VectorFunctionsTest::checkVecLength()
{
    fwVec3d V = {1.0, 2.0, 3.0};

    double length  = ::fwMath::vecLength(V);

    double expected = 3.741657387;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected,length, 0.00001);
}

} //namespace ut
} //namespace fwMath
