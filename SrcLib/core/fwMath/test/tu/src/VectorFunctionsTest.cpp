/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwMath/VectorFunctions.hpp>

#include "VectorFunctionsTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( VectorFunctionsTest );

void VectorFunctionsTest::setUp()
{
	// Set up context before running a test.

}
void VectorFunctionsTest::tearDown()
{
	// Clean up after the test run.
}

void VectorFunctionsTest::checkDot()
{
	// Produit scalaire.
    const double NORME_CARRE = 0.32;

	const fwVec3d V1 = {0.1, 0.2, 0.3};
	const fwVec3d V2 = {0.4, 0.5, 0.6};

	double result;
	result = ::fwMath::dot(V1, V2);

	CPPUNIT_ASSERT_EQUAL(result, NORME_CARRE);
}
void VectorFunctionsTest::checkCross()
{
	// Produit vectoriel.
    const double DISTANCE = 10.25;


	const double CROOS_X = -0.03;
	const double CROSS_Y = 0.06;
	const double CROSS_Z = -0.03;
	const fwVec3d V1 = {0.1, 0.2, 0.3};
	const fwVec3d V2 = {0.4, 0.5, 0.6};

	fwVec3d result;
	result = ::fwMath::cross(V1, V2);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(result[0], CROOS_X , 0.00001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(result[1], CROSS_Y , 0.00001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(result[2], CROSS_Z , 0.00001);
}
void VectorFunctionsTest::checkNormalize()
{
	const double NORME = 0.87749;
	fwVec3d V = {0.4, 0.5, 0.6};
	double norme = ::fwMath::normalize(V);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(norme,  NORME, 0.00001);
}

