/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwMath/PlaneFunctions.hpp>
// #include "SoLib/Sblinear.h"

#include "PlaneFunctionsTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( PlaneFunctionsTest );

void PlaneFunctionsTest::setUp()
{
	// Set up context before running a test.

}
void PlaneFunctionsTest::tearDown()
{
	// Clean up after the test run.

}

void PlaneFunctionsTest::checkDistance()
{
    const double DISTANCE = 10.25;

	fwPlane plane;
	::fwMath::setDistance(plane, DISTANCE);
	CPPUNIT_ASSERT_EQUAL(::fwMath::getDistance(plane), DISTANCE);
}
void PlaneFunctionsTest::checkNormal()
{
	const double PlanP1_X = 1.0;
	const double PlanP1_Y = 0.0;
	const double PlanP1_Z = 0.0;

	const double PlanP2_X = 0.0;
	const double PlanP2_Y = 0.0;
	const double PlanP2_Z = 1.0;

	const double PlanP3_X = 0.0;
	const double PlanP3_Y = 2.0;
	const double PlanP3_Z = 1.0;

	const fwVec3d planPt1 = {PlanP1_X, PlanP1_Y, PlanP1_Z};
	const fwVec3d planPt2 = {PlanP2_X, PlanP2_Y, PlanP2_Z};
	const fwVec3d planPt3 = {PlanP3_X, PlanP3_Y, PlanP3_Z};
	fwPlane plane;
	::fwMath::setValues(plane, planPt1, planPt2, planPt3);
	fwVec3d normal = ::fwMath::getNormal(plane);


//	SbPlane soPlane(SbVec3f(PlanP1_X, PlanP1_Y, PlanP1_Z), SbVec3f(PlanP2_X, PlanP2_Y, PlanP2_Z), SbVec3f(PlanP3_X, PlanP3_Y, PlanP3_Z));
//	SbVec3f sbNormal= soPlane.getNormal();
//
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(normal[0], (double)sbNormal[0], 0.00001);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(normal[1], (double)sbNormal[1], 0.00001);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(normal[2], (double)sbNormal[2], 0.00001);
}

void PlaneFunctionsTest::checkSetValues()
{

}
void PlaneFunctionsTest::checkIntersect( ) {

	const bool INTERSECT = true;

	fwVec3d  point;
	fwLine  line;
	const fwVec3d linePos = {1.0, 2.0, 4.0};
//	const fwVec3d lineDirection = {1.0, 0.0, 0.0};  // ==> pas d'intercestion
//	const fwVec3d lineDirection = {3.0, 0.0, 4.0};  // ==> intercestion
	const fwVec3d lineDirection = {0.0, 0.0, 4.0};  // ==> intersection en (0.0, 0.0, 0.0)

	line = std::make_pair < fwVec3d, fwVec3d >(linePos, lineDirection);

	const fwVec3d planPt1 = {0.0,0.0,0.0};
	const fwVec3d planPt2 = {2.0,0.0, 0.0};
	const fwVec3d planPt3 = {0.0, 2.0, 0.0};
	fwPlane plane;
	::fwMath::setValues(plane, planPt1, planPt2, planPt3);
	bool intersect =  ::fwMath::intersect( plane, line,  point);

	CPPUNIT_ASSERT_EQUAL(point[0], 1.0);
	CPPUNIT_ASSERT_EQUAL(point[1], 2.0);
	CPPUNIT_ASSERT_EQUAL(point[2], 0.0);

	CPPUNIT_ASSERT_EQUAL(intersect, INTERSECT);
}

void PlaneFunctionsTest::checkIntersect_fwMath_SoLib( ) {

	SLM_TRACE_FUNC();

//	for(int i= 0; i<100; i++) {
	const double LineP1_X = 1.0;
	const double LineP1_Y = rand()%100 +0.5;
	const double LineP1_Z = 1.0;

	const double LineP2_X = rand()%100 +0.1;
	const double LineP2_Y = 0.0;
	const double LineP2_Z = rand()%100 +0.6;

	const double PlanP1_X = 1.0;
	const double PlanP1_Y = rand()%100 +0.5;
	const double PlanP1_Z = 98.0;

	const double PlanP2_X = 0.0;
	const double PlanP2_Y = 7.0;
	const double PlanP2_Z = rand()%100 + 0.1;

	const double PlanP3_X = rand() % 200 +0.7;
	const double PlanP3_Y = 23.0;
	const double PlanP3_Z = rand()%50 + 0.8;

	// New version
	fwVec3d  point;
	fwLine  line;
	const fwVec3d linePos = {LineP1_X, LineP1_Y, LineP1_Z};
//	const fwVec3d lineDirection = {1.0, 0.0, 0.0};  // ==> pas d'intercestion
//	const fwVec3d lineDirection = {3.0, 0.0, 4.0};  // ==> intercestion
	fwVec3d lineDirection = {LineP2_X, LineP2_Y, LineP2_Z};  // ==> intersection en (0.0, 0.0, 0.0)
	line = std::make_pair < fwVec3d, fwVec3d >(linePos, lineDirection);
	const fwVec3d planPt1 = {PlanP1_X, PlanP1_Y, PlanP1_Z};
	const fwVec3d planPt2 = {PlanP2_X, PlanP2_Y, PlanP2_Z};
	const fwVec3d planPt3 = {PlanP3_X, PlanP3_Y, PlanP3_Z};
	fwPlane plane;
	::fwMath::setValues(plane, planPt1, planPt2, planPt3);
	int intersect =  ::fwMath::intersect( plane, line,  point);

	// Old version
//	SbLine aLine(SbVec3f(LineP1_X, LineP1_Y, LineP1_Z), SbVec3f(LineP2_X, LineP2_Y, LineP2_Z));
//	SbVec3f aIntersection;
//	SbPlane soPlane(SbVec3f(PlanP1_X, PlanP1_Y, PlanP1_Z), SbVec3f(PlanP2_X, PlanP2_Y, PlanP2_Z), SbVec3f(PlanP3_X, PlanP3_Y, PlanP3_Z));
//	SbBool soIntersect = soPlane.intersect(aLine,aIntersection);

//		OSLM_TRACE( "X :" <<  point[0] << ", " << aIntersection[0]);
//		OSLM_TRACE( "Y :" <<  point[1] << ", " << aIntersection[1]);
//		OSLM_TRACE( "Z :" <<  point[2] << ", " << aIntersection[2]);

//	CPPUNIT_ASSERT_EQUAL(intersect, soIntersect);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(point[0], (double)aIntersection[0], 0.00001);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(point[1], (double)aIntersection[1], 0.00001);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(point[2], (double)aIntersection[2], 0.00001);

//}

//	CPPUNIT_ASSERT_EQUAL(intersect, INTERSECT);
}

void PlaneFunctionsTest::checkIsInHalfSpace_fwMath_SoLib() {


	const double P1_X = 1.0;
	const double P1_Y = 0.0;
	const double P1_Z = 1.0;

	const double PlanP1_X = 1.0;
	const double PlanP1_Y = 0.0;
	const double PlanP1_Z = 0.0;

	const double PlanP2_X = 0.0;
	const double PlanP2_Y = 0.0;
	const double PlanP2_Z = 1.0;

	const double PlanP3_X = 0.0;
	const double PlanP3_Y = 2.0;
	const double PlanP3_Z = 1.0;

	const fwVec3d point1 = {P1_X, P1_Y, P1_Z};


	const fwVec3d planPt1 = {PlanP1_X, PlanP1_Y, PlanP1_Z};
	const fwVec3d planPt2 = {PlanP2_X, PlanP2_Y, PlanP2_Z};
	const fwVec3d planPt3 = {PlanP3_X, PlanP3_Y, PlanP3_Z};
	fwPlane plane;
	::fwMath::setValues(plane, planPt1, planPt2, planPt3);

	bool result = ::fwMath::isInHalfSpace(plane, point1);


//	SbPlane sbPlane(SbVec3f(PlanP1_X, PlanP1_Y, PlanP1_Z), SbVec3f(PlanP2_X, PlanP2_Y, PlanP2_Z), SbVec3f(PlanP3_X, PlanP3_Y, PlanP3_Z));
//	SbVec3f sbPoint1(P1_X, P1_Y, P1_Z);
//
//	SbBool sbResult = sbPlane.isInHalfSpace(sbPoint1);
//	CPPUNIT_ASSERT_EQUAL(result, (bool)sbResult);

}

void PlaneFunctionsTest::checkOffset() {


	const double OFFSET = 0.3;

	const double PlanP1_X = 1.0;
	const double PlanP1_Y = 0.0;
	const double PlanP1_Z = 0.0;

	const double PlanP2_X = 0.0;
	const double PlanP2_Y = 0.0;
	const double PlanP2_Z = 1.0;

	const double PlanP3_X = 0.0;
	const double PlanP3_Y = 2.0;
	const double PlanP3_Z = 1.0;

	const fwVec3d planPt1 = {PlanP1_X, PlanP1_Y, PlanP1_Z};
	const fwVec3d planPt2 = {PlanP2_X, PlanP2_Y, PlanP2_Z};
	const fwVec3d planPt3 = {PlanP3_X, PlanP3_Y, PlanP3_Z};
	fwPlane plane;
	::fwMath::setValues(plane, planPt1, planPt2, planPt3);

	::fwMath::offset(plane,OFFSET );
	double result = ::fwMath::getDistance(plane);

//	SbPlane sbPlane(SbVec3f(PlanP1_X, PlanP1_Y, PlanP1_Z), SbVec3f(PlanP2_X, PlanP2_Y, PlanP2_Z), SbVec3f(PlanP3_X, PlanP3_Y, PlanP3_Z));
//	sbPlane.offset(OFFSET);
//	float sbResult = sbPlane.getDistanceFromOrigin();
//
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(result, sbResult, 0.00001);
}

void PlaneFunctionsTest::checkTransform() {

	const fwVec3d normal = {0.2, 0.6, 0.8};
	const double distance = 0.28;

    const double c1 = rand()%40+0.8;
    const double c2 = rand()%1+0.1;
    const double c3 = rand()%29+0.5;
    const double c4 = rand()%50+0.6;
    const double c5 = rand()%20+0.5;

	const double PlanP1_X = 1.0;
	const double PlanP1_Y = rand()%100 +0.5;
	const double PlanP1_Z = 98.0;

	const double PlanP2_X = 0.0;
	const double PlanP2_Y = 7.0;
	const double PlanP2_Z = rand()%100 + 0.1;

	const double PlanP3_X = rand() % 200 +0.7;
	const double PlanP3_Y = 23.0;
	const double PlanP3_Z = rand()%50 + 0.8;


	const fwVec3d planPt1 = {PlanP1_X, PlanP1_Y, PlanP1_Z};
	const fwVec3d planPt2 = {PlanP2_X, PlanP2_Y, PlanP2_Z};
	const fwVec3d planPt3 = {PlanP3_X, PlanP3_Y, PlanP3_Z};
	fwPlane plane;
	::fwMath::setValues(plane, planPt1, planPt2, planPt3);
	const fwMatrix4x4 matrice = {1,c1,0,c2, c3,1,c1,0, c4,0,1,c5, 0,c1,c2,c4 };
	::fwMath::transform(plane, matrice);

//	SbPlane soPlane(SbVec3f(PlanP1_X, PlanP1_Y, PlanP1_Z), SbVec3f(PlanP2_X, PlanP2_Y, PlanP2_Z), SbVec3f(PlanP3_X, PlanP3_Y, PlanP3_Z));
//	SbMatrix aMatrix (1,c1,0,c2, c3,1,c1,0, c4,0,1,c5, 0,c1,c2,c4);
//	soPlane.transform(aMatrix);
//
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[0], (soPlane.getNormal())[0], 0.00001);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[1], (soPlane.getNormal())[1], 0.00001);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[2], (soPlane.getNormal())[2], 0.00001);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[3], soPlane.getDistanceFromOrigin(), 0.00001);
}



