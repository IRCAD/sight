/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMATH_TU_MATRIXFUNCTIONSTEST_HPP__
#define __FWMATH_TU_MATRIXFUNCTIONSTEST_HPP__

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
    // interface
    void setUp();
    void tearDown();
    // fonctions de tests
    void checkMultVecMatrix();
    void checkRotationMatrix();
};

} //namespace ut
} //namespace fwMath

#endif //__FWMATH_TU_MATRIXFUNCTIONSTEST_HPP__
