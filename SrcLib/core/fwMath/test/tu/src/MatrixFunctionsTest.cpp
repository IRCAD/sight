/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMath/MatrixFunctions.hpp"
#include "MatrixFunctionsTest.hpp"


#include "fwMath/IntrasecTypes.hpp"
#include "fwMath/VectorFunctions.hpp"
#include "fwMath/LineFunctions.hpp"
#include "fwMath/export.hpp"
#include "fwMath/MatrixFunctions.hpp"

//#include "SoLib/Sblinear.h"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( MatrixFunctionsTest );

void MatrixFunctionsTest::setUp()
{
    // Set up context before running a test.


}
void MatrixFunctionsTest::tearDown()
{
    // Clean up after the test run.
}

void MatrixFunctionsTest::checkMultVecMatrix()
{
    // Produit scalaire.
    const double X = rand()%1 + 0.9;
    const double Y = rand()%10 + 0.8;
    const double Z = rand()%70 + 0.2;

    const double c1 = rand()%40+0.8;
    const double c2 = rand()%1+0.1;
    const double c3 = rand()%29+0.5;
    const double c4 = rand()%50+0.6;
    const double c5 = rand()%20+0.5;


    const fwVec3d source = {X, Y, Z};
    fwVec3d result;
    const fwMatrix4x4 matrice = {1,c1,1,c2, 1,2,c3,c4, 1,1,5,c2, c3,1,c1,1};
    ::fwMath::multVecMatrix(matrice, source, result);

//  SbVec3f src(X, Y, Z);
//  SbVec3f sbResult;
//  SbMatrix sbmatrix(1,c1,1,c2, 1,2,c3,c4, 1,1,5,c2, c3,1,c1,1);
//  sbmatrix.multVecMatrix(src, sbResult);
//
//  CPPUNIT_ASSERT_DOUBLES_EQUAL(result[0], sbResult[0], 0.00001);
//  CPPUNIT_ASSERT_DOUBLES_EQUAL(result[1], sbResult[1], 0.00001);
//  CPPUNIT_ASSERT_DOUBLES_EQUAL(result[2], sbResult[2], 0.00001);
}

void MatrixFunctionsTest::checkRotationMatrix()
{
//    const double X = 7.8;
//    const double Y = 0.6;
//    const double Z = 2.6;
//
//    const fwVec3d vec = {X, Y, Z};
//
//    fwMatrix4x4 result = ::fwMath::getRotationMatrix(vec);
//
//
//  SbVec3f src(X, Y, Z);
//  SbRotation sbRotation;
//  SbMatrix sbResult = sbRotation.getRotationMatrix(src);
//
//  CPPUNIT_ASSERT_DOUBLES_EQUAL(result[0][0], sbResult[0][0], 0.00001);
//  CPPUNIT_ASSERT_DOUBLES_EQUAL(result[0][1], sbResult[0][1], 0.00001);
//  CPPUNIT_ASSERT_DOUBLES_EQUAL(result[0][2], sbResult[0][2], 0.00001);
//  CPPUNIT_ASSERT_DOUBLES_EQUAL(result[0][3], sbResult[0][3], 0.00001);
//
//  CPPUNIT_ASSERT_DOUBLES_EQUAL(result[1][0], sbResult[1][0], 0.00001);
//  CPPUNIT_ASSERT_DOUBLES_EQUAL(result[1][1], sbResult[1][1], 0.00001);
//  CPPUNIT_ASSERT_DOUBLES_EQUAL(result[1][2], sbResult[1][2], 0.00001);
//  CPPUNIT_ASSERT_DOUBLES_EQUAL(result[1][3], sbResult[1][3], 0.00001);
//
//  CPPUNIT_ASSERT_DOUBLES_EQUAL(result[2][0], sbResult[2][0], 0.00001);
//  CPPUNIT_ASSERT_DOUBLES_EQUAL(result[2][1], sbResult[2][1], 0.00001);
//  CPPUNIT_ASSERT_DOUBLES_EQUAL(result[2][2], sbResult[2][2], 0.00001);
//  CPPUNIT_ASSERT_DOUBLES_EQUAL(result[2][3], sbResult[2][3], 0.00001);
//
//  CPPUNIT_ASSERT_DOUBLES_EQUAL(result[3][0], sbResult[3][0], 0.00001);
//  CPPUNIT_ASSERT_DOUBLES_EQUAL(result[3][1], sbResult[3][1], 0.00001);
//  CPPUNIT_ASSERT_DOUBLES_EQUAL(result[3][2], sbResult[3][2], 0.00001);
//  CPPUNIT_ASSERT_DOUBLES_EQUAL(result[3][3], sbResult[3][3], 0.00001);
}
