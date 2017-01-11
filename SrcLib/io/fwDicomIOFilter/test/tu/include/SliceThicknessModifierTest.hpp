/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOFILTER_UT_SLICETHICKNESSMODIFIERTEST_HPP__
#define __FWDICOMIOFILTER_UT_SLICETHICKNESSMODIFIERTEST_HPP__

#include <fwServices/macros.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwDicomIOFilter
{
namespace ut
{

/**
 * @brief Test SliceThicknessModifier class
 */
class SliceThicknessModifierTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( SliceThicknessModifierTest );
CPPUNIT_TEST( simpleApplication );
CPPUNIT_TEST( applyFilterOn2DImage );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    /// Apply the filter and verify that the DicomSeries has been correctly modified
    void simpleApplication();

    /// Apply the filter on an unsupported 2D image
    void applyFilterOn2DImage();

};

} // namespace ut
} // namespace fwDicomIOFilter

#endif // __FWDICOMIOFILTER_UT_SLICETHICKNESSMODIFIERTEST_HPP__
