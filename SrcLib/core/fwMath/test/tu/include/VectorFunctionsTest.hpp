/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMATH_UT_VECTORFUNCTIONSTEST_HPP__
#define __FWMATH_UT_VECTORFUNCTIONSTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>


namespace fwMath
{
namespace ut
{

class VectorFunctionsTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( VectorFunctionsTest );
CPPUNIT_TEST( checkNormalize);
CPPUNIT_TEST( checkDot );
CPPUNIT_TEST( checkCross );
CPPUNIT_TEST(checkNegate);
CPPUNIT_TEST(checkVecLength);
CPPUNIT_TEST_SUITE_END();

protected:

public:
    // interface
    void setUp();
    void tearDown();

    void checkDot();
    void checkCross();
    void checkNormalize();
    void checkNegate();
    void checkVecLength();

};

} //namespace ut
} //namespace fwMath

#endif //__FWMATH_UT_VECTORFUNCTIONSTEST_HPP__
