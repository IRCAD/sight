/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMATH_UT_PLANEFUNCTIONSTEST_HPP__
#define __FWMATH_UT_PLANEFUNCTIONSTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwMath
{
namespace ut
{

class PlaneFunctionsTest : public CPPUNIT_NS::TestFixture
{
private:
    CPPUNIT_TEST_SUITE( PlaneFunctionsTest );
    CPPUNIT_TEST( checkGetPlane );
    CPPUNIT_TEST( checkSetValues );
    CPPUNIT_TEST( checkDistance );
    CPPUNIT_TEST( checkNormal );
//    CPPUNIT_TEST( checkIntersect );
    CPPUNIT_TEST( checkIsInHalfSpace );
    CPPUNIT_TEST( checkOffset );
    CPPUNIT_TEST( checkTransform );
    CPPUNIT_TEST( checkOperator);
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    // fonctions de tests
    void checkSetValues();
    void checkGetPlane();
    void checkDistance();
    void checkNormal();
    void checkIntersect();
    void checkIsInHalfSpace();
    void checkOffset();
    void checkTransform();
    void checkOperator();
};

} //namespace ut
} //namespace fwMath

#endif //__FWMATH_UT_PLANEFUNCTIONSTEST_HPP__
