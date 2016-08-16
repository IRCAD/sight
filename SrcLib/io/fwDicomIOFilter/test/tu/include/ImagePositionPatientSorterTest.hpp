/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOFILTER_UT_IMAGEPOSITIONPATIENTSORTERTEST_HPP__
#define __FWDICOMIOFILTER_UT_IMAGEPOSITIONPATIENTSORTERTEST_HPP__

#include <fwServices/macros.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwDicomIOFilter
{
namespace ut
{

/**
 * @brief Test ImagePositionPatientSorter class
 */
class ImagePositionPatientSorterTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ImagePositionPatientSorterTest );
CPPUNIT_TEST( simpleApplication );
CPPUNIT_TEST( applyFilterOnMultipleVolumeImage );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    /// Apply the patch and verify that the DicomSeries has been correctly modified
    void simpleApplication();

    /// Apply the filter on an unsupported multiple volume image
    void applyFilterOnMultipleVolumeImage();

};

} // namespace ut
} // namespace fwDicomIOFilter

#endif // __FWDICOMIOFILTER_UT_IMAGEPOSITIONPATIENTSORTERTEST_HPP__
