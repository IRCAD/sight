/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMDSEMANTICPATCH_UT_IMAGEV09ALATOV10TEST_HPP__
#define __FWMDSEMANTICPATCH_UT_IMAGEV09ALATOV10TEST_HPP__

#include <fwAtoms/Sequence.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwMDSemanticPatch
{
namespace ut
{
/**
 * @brief Test patch to convert 'm_imageLandmarksId' field  of '::fwData::Image' from '::fwData::PointList' to
 *        `::fwData::Landmarks`.
 */
class ImageV09ALAToV10Test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ImageV09ALAToV10Test );
CPPUNIT_TEST( applyPatchTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void applyPatchTest();

    void addPoint(::fwAtoms::Sequence::sptr seq, const std::array<double, 3>& pt, const std::string& label);
};

} //namespace ut
} //namespace fwMDSemanticPatch

#endif //__FWMDSEMANTICPATCH_UT_IMAGEV09ALATOV10TEST_HPP__
