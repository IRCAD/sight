/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMATH_TU_COMPARETEST_HPP__
#define __FWMATH_TU_COMPARETEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwMath
{
namespace ut
{

class CompareTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( CompareTest );
    CPPUNIT_TEST( checkEquality);
    CPPUNIT_TEST( checkContainerEquality);
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    // fonctions de tests
    void checkEquality();
    void checkContainerEquality();
};

} //namespace ut
} //namespace fwMath

#endif //__FWMATH_TU_COMPARETEST_HPP__
