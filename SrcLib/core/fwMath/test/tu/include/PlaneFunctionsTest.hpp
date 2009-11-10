/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWMATH_TU_PLANEFUNCTIONSTEST_H
#define FWMATH_TU_PLANEFUNCTIONSTEST_H

#include <cppunit/extensions/HelperMacros.h>

class PlaneFunctionsTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( PlaneFunctionsTest );
	CPPUNIT_TEST( checkDistance );
	CPPUNIT_TEST( checkNormal );
	CPPUNIT_TEST( checkSetValues );
	CPPUNIT_TEST( checkIntersect );
	CPPUNIT_TEST( checkIsInHalfSpace );
	CPPUNIT_TEST( checkOffset );
	CPPUNIT_TEST( checkTransform );
	CPPUNIT_TEST_SUITE_END();

protected:

public:
	// interface
	void setUp();
	void tearDown();
	// fonctions de tests
	void checkDistance();
	void checkNormal();
	void checkSetValues();
	void checkIntersect();
	void checkIsInHalfSpace();
	void checkOffset();
	void checkTransform();
};

#endif
