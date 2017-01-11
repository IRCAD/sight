/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_UT_DICOMANONYMIZERTEST_HPP__
#define __FWGDCMIO_UT_DICOMANONYMIZERTEST_HPP__

#include <fwServices/macros.hpp>

#include <gdcmTag.h>
#include <gdcmDataSet.h>

#include <cppunit/extensions/HelperMacros.h>

namespace fwGdcmIO
{
namespace ut
{

class DicomAnonymizerTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( DicomAnonymizerTest );
CPPUNIT_TEST( anonymizeImageSeriesTest );
CPPUNIT_TEST( anonymizeDICOMTest );
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    /// Test anonymization of ImageSeries
    void anonymizeImageSeriesTest();

    /// Test anonymisation of DICOM folder
    void anonymizeDICOMTest();

private:
    void testDICOMFolder(const ::boost::filesystem::path& srcPath);
    void testAnonymizedFile(const std::string& filename);

    std::set< std::string > m_uidContainer;

};

} // namespace ut
} // namespace fwGdcmIO

#endif // __FWGDCMIO_UT_DICOMANONYMIZERTEST_HPP__
