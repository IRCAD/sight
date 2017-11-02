/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_UT_DICOMSERIESWRITERTEST_HPP__
#define __FWGDCMIO_UT_DICOMSERIESWRITERTEST_HPP__

#include <fwCore/base.hpp>

#include <fwMedData/DicomSeries.hpp>

#include <fwServices/macros.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwGdcmIO
{
namespace ut
{

typedef std::set<std::string> ExcludeSetType;
class DicomSeriesWriterTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( DicomSeriesWriterTest );
CPPUNIT_TEST( writeReadTest );
CPPUNIT_TEST( writeReadAnonymiseTest );
CPPUNIT_TEST( writeReadDirArchiveTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void writeReadTest();
    void writeReadAnonymiseTest();
    void writeReadDirArchiveTest();

    // HACK: force link with fwMedDataCamp. Needed when calling ::fwDataCamp::visitor::CompareObjects::compare.
    int m_medDataCampVersion;

private:

    /**
     * @brief Read Dicom series at given path and compare them to m_srcDicomSeries.
     *
     * @note Do not compare local_dicom_paths attributes
     *
     * @param p path to read Dicom series to be compared
     * @param anonymized if true, further attributes related to anonymisation are not compared.
     */
    void checkDicomSeries(
        const ::boost::filesystem::path& p, bool anonymized = false);

    ::fwMedData::DicomSeries::sptr m_srcDicomSeries;

};

} // namespace ut
} // namespace fwGdcmIO

#endif // __FWGDCMIO_UT_DICOMSERIESWRITERTEST_HPP__
