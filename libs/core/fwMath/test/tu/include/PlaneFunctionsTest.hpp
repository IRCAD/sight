/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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
