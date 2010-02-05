/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwMath/VectorFunctions.hpp>
// #include "SoLib/Sblinear.h"

#include <fwCore/base.hpp>

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

void VectorFunctionsTest::checkDot_fwMath_Solib()
{

	const double V1_X = 0.1;
	const double V1_Y =rand()%30+0.1;
	const double V1_Z =rand()%20+0.4;

	const double V2_X = rand()%50+0.4;
	const double V2_Y =0.5;
	const double V2_Z =rand()%10+0.8;

	// Produit scalaire.
	const fwVec3d V1 = {V1_X, V1_Y, V1_Z};
	const fwVec3d V2 = {V2_X, V2_Y, V2_Z};
	double result;
	result = ::fwMath::dot(V1, V2);

	// Old version
//	SbVec3f soV1(V1_X, V1_Y, V1_Z);
//	SbVec3f soV2(V2_X, V2_Y, V2_Z);
//	float soResult = soV1.dot(soV2);
//
//	OSLM_TRACE( "V1_X :" <<  V1[0] << ", " << soV1[0]);
//	OSLM_TRACE( "V1_Y :" <<  V1[1] << ", " << soV1[1]);
//	OSLM_TRACE( "V1_Z :" <<  V1[2] << ", " << soV1[2]);
//	OSLM_TRACE( "V2_X :" <<  V2[0] << ", " << soV2[0]);
//	OSLM_TRACE( "V2_Y :" <<  V2[1] << ", " << soV2[1]);
//	OSLM_TRACE( "V2_Z :" <<  V2[2] << ", " << soV2[2]);
//	OSLM_TRACE( "DOT RESULT :" <<  result << ", " << soResult);


//	CPPUNIT_ASSERT_DOUBLES_EQUAL(result, double(soResult), 0.00001);
}
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

	CPPUNIT_ASSERT_DOUBLES_EQUAL(result[0], CROOS_X , 0.00001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(result[1], CROSS_Y , 0.00001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(result[2], CROSS_Z , 0.00001);
}

void VectorFunctionsTest::checkCross_fwMath_SoLib()
{
	SLM_TRACE_FUNC();
	// Produit vectoriel.

	const double V1_X = 0.1;
	const double V1_Y =rand()%30+0.1;
	const double V1_Z =rand()%10+0.4;

	const double V2_X = rand()%50+0.4;
	const double V2_Y =0.5;
	const double V2_Z =rand()%20+0.8;

    // New version
	const fwVec3d V1 = {V1_X, V1_Y, V1_Z};
	const fwVec3d V2 = {V2_X, V2_Y, V2_Z};

	fwVec3d result;
	result = ::fwMath::cross(V1, V2);

	// Old version
//	SbVec3f soV1(V1_X, V1_Y, V1_Z);
//	SbVec3f soV2(V2_X, V2_Y, V2_Z);
//	SbVec3f soResult = soV1.cross(soV2);
//
//	OSLM_TRACE( "V1_X :" <<  V1[0] << ", " << soV1[0]);
//	OSLM_TRACE( "V1_Y :" <<  V1[1] << ", " << soV1[1]);
//	OSLM_TRACE( "V1_Z :" <<  V1[2] << ", " << soV1[2]);
//	OSLM_TRACE( "V2_X :" <<  V2[0] << ", " << soV2[0]);
//	OSLM_TRACE( "V2_Y :" <<  V2[1] << ", " << soV2[1]);
//	OSLM_TRACE( "V2_Z :" <<  V2[2] << ", " << soV2[2]);
//	OSLM_TRACE( "CROSS RESULT_X :" <<  result[0] << ", " << soResult[0]);
//	OSLM_TRACE( "CROSS RESULT_Y :" <<  result[1] << ", " << soResult[1]);
//	OSLM_TRACE( "CROSS RESULT_Z :" <<  result[2] << ", " << soResult[2]);
//
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(result[0], (double)soResult[0] , 0.00001);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(result[1], (double)soResult[1] , 0.00001);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(result[2], (double)soResult[2] , 0.00001);

}

void VectorFunctionsTest::checkNormalize()
{
	const double NORME = 0.87749;
	fwVec3d V = {0.4, 0.5, 0.6};
	double norme = ::fwMath::normalize(V);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(norme,  NORME, 0.00001);
}

