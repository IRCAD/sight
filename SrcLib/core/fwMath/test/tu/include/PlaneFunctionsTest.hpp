/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMATH_TU_PLANEFUNCTIONSTEST_HPP__
#define __FWMATH_TU_PLANEFUNCTIONSTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwMath
{
namespace ut
{

class PlaneFunctionsTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( PlaneFunctionsTest );
    CPPUNIT_TEST( checkDistance );
    CPPUNIT_TEST( checkNormal );
//  CPPUNIT_TEST( checkIntersect );
    CPPUNIT_TEST( checkIsInHalfSpace_fwMath_SoLib );
    CPPUNIT_TEST( checkOffset );
    CPPUNIT_TEST( checkTransform );
    CPPUNIT_TEST( checkIntersect_fwMath_SoLib);
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    // fonctions de tests
    void checkDistance();
    void checkNormal();
    void checkIntersect();
    void checkIsInHalfSpace_fwMath_SoLib();
    void checkOffset();
    void checkTransform();
    void checkIntersect_fwMath_SoLib();
};

} //namespace ut
} //namespace fwMath

#endif //__FWMATH_TU_PLANEFUNCTIONSTEST_HPP__
