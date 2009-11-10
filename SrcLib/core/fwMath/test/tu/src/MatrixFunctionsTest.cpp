/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMath/MatrixFunctions.hpp"
#include "MatrixFunctionsTest.hpp"


//#include "fwMath/IntrasecTypes.hpp"
//#include "fwMath/VectorFunctions.hpp"
//#include "fwMath/LineFunctions.hpp"
//#include "fwMath/export.hpp"
//#include "fwMath/MatrixFunctions.hpp"



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

//void MatrixFunctionsTest::checkMultVecMatrix()
//{
//	// Produit scalaire.
//    const double X = 2.6;
//    const double Y = 2.6;
//    const double Z = 2.6;
//
//    const fwVec3d source = {0.2, 0.6, 0.8};
//	fwVec3d result;
//	const fwMatrix4x4 matrice = { 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1 };
////	::fwMath::multVecMatrix(matrice, source, result);
//	fwMatrix4x4 matriceRot;
//	matriceRot = ::fwMath::getRotationMatrix(source);
//	 CPPUNIT_ASSERT_DOUBLES_EQUAL(result[0], X, 0.00001);
//	 CPPUNIT_ASSERT_DOUBLES_EQUAL(result[1], Y, 0.00001);
//	 CPPUNIT_ASSERT_DOUBLES_EQUAL(result[2], Z, 0.00001);
//}
