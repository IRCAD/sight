/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWMATH_TU_VECTORFUNCTIONSTEST_H
#define FWMATH_TU_VECTORFUNCTIONSTEST_H

#include <cppunit/extensions/HelperMacros.h>


class VectorFunctionsTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( VectorFunctionsTest );
	CPPUNIT_TEST( checkNormalize);
	CPPUNIT_TEST( checkDot );
	CPPUNIT_TEST( checkDot_fwMath_Solib );
	CPPUNIT_TEST( checkCross );
	CPPUNIT_TEST( checkCross_fwMath_SoLib );
	CPPUNIT_TEST_SUITE_END();

protected:

public:
	// interface
	void setUp();
	void tearDown();
	// fonctions de tests
	void checkDot();
	void checkDot_fwMath_Solib();
	void checkCross();
	void checkCross_fwMath_SoLib();
	void checkNormalize();
};

#endif
