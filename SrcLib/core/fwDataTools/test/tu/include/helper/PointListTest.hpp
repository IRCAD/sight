/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATATOOLS_UT_HELPER_POINTLISTTEST_HPP__
#define __FWDATATOOLS_UT_HELPER_POINTLISTTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwDataTools
{
namespace ut
{

class PointListTest : public CPPUNIT_NS::TestFixture
{
private:
    CPPUNIT_TEST_SUITE( PointListTest );
    CPPUNIT_TEST( computeDistance );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void computeDistance();
};
} //namespace ut
} //namespace fwDataTools

#endif //__FWDATATOOLS_UT_HELPER_POINTLISTTEST_HPP__
