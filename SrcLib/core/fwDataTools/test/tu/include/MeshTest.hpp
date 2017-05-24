/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATATOOLS_UT_MESHTEST_HPP__
#define __FWDATATOOLS_UT_MESHTEST_HPP__

#include <fwData/Image.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwDataTools
{
namespace ut
{

/**
 * @brief Test many methods to create image.
 */
class MeshTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( MeshTest );
CPPUNIT_TEST( colorizePointsTest );
CPPUNIT_TEST( colorizeCellsTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void colorizePointsTest();
    void colorizeCellsTest();

};

} //namespace ut
} //namespace fwDataTools

#endif //__FWDATATOOLS_UT_MESHTEST_HPP__
