/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARSTRUCTURALPATCH_UT_ARDATA_CAMERA_V1TOV2TEST_HPP__
#define __ARSTRUCTURALPATCH_UT_ARDATA_CAMERA_V1TOV2TEST_HPP__

#include <cppunit/extensions/HelperMacros.h>


namespace arStructuralPatch
{
namespace ut
{
namespace arData
{
namespace Camera
{

/**
 * @brief Test patch to convert fwData::Reconstruction from version 1 to version 2.
 */
class V1ToV2Test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( V1ToV2Test );
CPPUNIT_TEST( applyPatchTestFile );
CPPUNIT_TEST( applyPatchTestStream );
CPPUNIT_TEST( applyPatchTestDevice );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void applyPatchTestFile();
    void applyPatchTestStream();
    void applyPatchTestDevice();
};

} //namespace Camera
} //namespace arData
} //namespace ut
} //namespace arStructuralPatch

#endif //__ARSTRUCTURALPATCH_UT_ARDATA_CAMERA_V1TOV2TEST_HPP__
