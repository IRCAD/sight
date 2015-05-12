/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMATH_UT_MATRIXFUNCTIONSTEST_HPP__
#define __FWMATH_UT_MATRIXFUNCTIONSTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwMath
{
namespace ut
{

class MatrixFunctionsTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( MatrixFunctionsTest );
CPPUNIT_TEST( checkMultVecMatrix);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();

    void checkMultVecMatrix();
    void checkRotationMatrix();
};

} //namespace ut
} //namespace fwMath

#endif //__FWMATH_UT_MATRIXFUNCTIONSTEST_HPP__
