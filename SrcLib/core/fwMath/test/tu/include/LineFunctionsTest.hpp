/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMATH_UT_LINEFUNCTIONSTEST_HPP__
#define __FWMATH_UT_LINEFUNCTIONSTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwMath
{
namespace ut
{

class LineFunctionsTest : public CPPUNIT_NS::TestFixture
{
private:
    CPPUNIT_TEST_SUITE( LineFunctionsTest );
    CPPUNIT_TEST( checkGetClosestPoint);
    CPPUNIT_TEST( checkGetClosestPoints);
    CPPUNIT_TEST( checkIntersect1);
    CPPUNIT_TEST( checkIntersect2);
    CPPUNIT_TEST( checkIntersect3);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();

    void checkGetClosestPoint();
    void checkGetClosestPoints();
    void checkIntersect1();
    void checkIntersect2();
    void checkIntersect3();
};

} //namespace ut
} //namespace fwMath

#endif // __FWMATH_UT_LINEFUNCTIONSTEST_HPP__
