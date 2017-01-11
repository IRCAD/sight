/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARLCORE_UT_MATRIXTEST_HPP__
#define __ARLCORE_UT_MATRIXTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace ARLcore
{
namespace ut
{

class MatrixTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( MatrixTest );
CPPUNIT_TEST( conversionVectorMatrixTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void conversionVectorMatrixTest();

};

} //namespace ut
} //namespace ARLCORE
#endif //__ARLCORE_UT_MATRIXTEST_HPP__

