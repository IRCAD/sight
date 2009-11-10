/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwMath/PlaneFunctions.hpp>

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
    const double DISTANCE = 10.25;

	fwPlane plane;

	const double NORMAL_X = 0.2;
	const double NORMAL_Y = 0.6;
	const double NORMAL_Z = 0.8;
	const fwVec3d NORMAL = {NORMAL_X, NORMAL_Y, NORMAL_Z};

	::fwMath::setNormal(plane, NORMAL);
	fwVec3d normalRead = ::fwMath::getNormal(plane);

	CPPUNIT_ASSERT_EQUAL(normalRead[0], NORMAL_X);
	CPPUNIT_ASSERT_EQUAL(normalRead[1], NORMAL_Y);
	CPPUNIT_ASSERT_EQUAL(normalRead[2], NORMAL_Z);
}

void PlaneFunctionsTest::checkSetValues()
{

}
void PlaneFunctionsTest::checkIntersect( ) {

	const bool INTERSECT = true;

	fwVec3d  point;
	fwLine  line;
	const fwVec3d linePos = {0.0, 0.0, 4.0};
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
	CPPUNIT_ASSERT_EQUAL(point[0], 0.0);
	CPPUNIT_ASSERT_EQUAL(point[0], 0.0);
	CPPUNIT_ASSERT_EQUAL(point[0], 0.0);

	CPPUNIT_ASSERT_EQUAL(intersect, INTERSECT);
}

void PlaneFunctionsTest::checkIsInHalfSpace() {
	const bool IN_HALF_SPACE = true;

//	const fwVec3d point1 = {0.0, 0.0, -4.0};  //==> InHaklfSpace = false
	const fwVec3d point1 = {0.0, 0.0, 4.0};   //==> InHaklfSpace = true;

	const fwVec3d planPt1 = {0.0,0.0,0.0};
	const fwVec3d planPt2 = {2.0,0.0, 0.0};
	const fwVec3d planPt3 = {0.0, 2.0, 0.0};
	fwPlane plane;
	::fwMath::setValues(plane, planPt1, planPt2, planPt3);

	bool result = ::fwMath::isInHalfSpace(plane, point1);
	CPPUNIT_ASSERT_EQUAL(result, IN_HALF_SPACE);
}

void PlaneFunctionsTest::checkOffset() {

	const double DISTANCE = 0.3;
	fwPlane plane;
	const fwVec3d normal = {0.2, 0.6, 0.8};
	const double distance = 0.28;
	::fwMath::setNormal(plane, normal);
	::fwMath::setDistance(plane, distance);

	::fwMath::offset(plane, 0.02);
	double result = ::fwMath::getDistance(plane);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(result, DISTANCE, 0.00001);
}

void PlaneFunctionsTest::checkTransform() {

	const fwVec3d normal = {0.2, 0.6, 0.8};
	const double distance = 0.28;

	fwPlane plane;
	::fwMath::setNormal(plane, normal);
	::fwMath::setDistance(plane, distance);

//	const fwMatrix4x4 matrice = { 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1 };
	const fwMatrix4x4 matrice = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

	::fwMath::transform(plane, matrice);


//	CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[0], 0.2);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[1], 0.6);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[2], 0.8);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[3], 0.28);
}



