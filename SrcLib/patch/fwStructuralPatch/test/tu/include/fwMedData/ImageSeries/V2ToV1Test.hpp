/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <cppunit/extensions/HelperMacros.h>

namespace fwStructuralPatch
{
namespace ut
{
namespace fwMedData
{
namespace ImageSeries
{

/**
 * @brief Test patch to convert ::fwMedData::ImageSeries from version 2 to version 1.
 */
class V2ToV1Test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( V2ToV1Test );
CPPUNIT_TEST( applyPatchTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void applyPatchTest();
};

} //namespace ImageSeries
} //namespace fwMedData
} //namespace ut
} //namespace fwStructuralPatch
