/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDCMTKIO_UT_SERIESDBREADERTEST_HPP__
#define __FWDCMTKIO_UT_SERIESDBREADERTEST_HPP__

#include <fwServices/macros.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwDcmtkIO
{
namespace ut
{

class SeriesDBReaderTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( SeriesDBReaderTest );
CPPUNIT_TEST( readACHSeriesDBTest );
CPPUNIT_TEST( readACHLazySeriesDBTest );
CPPUNIT_TEST( readCTSeriesDBTest );
CPPUNIT_TEST( readCTLazySeriesDBTest );
CPPUNIT_TEST( readMRSeriesDBTest );
CPPUNIT_TEST( readMRLazySeriesDBTest );
CPPUNIT_TEST( readOTSeriesDBTest );
CPPUNIT_TEST( readOTLazySeriesDBTest );
CPPUNIT_TEST( readUnsupportedSeriesDBTest );
CPPUNIT_TEST( readUnsupportedLazySeriesDBTest );
CPPUNIT_TEST( readDisabledSeriesDBTest );
CPPUNIT_TEST( readDisabledLazySeriesDBTest );
CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();

    /// Read ACH Series
    void readACHSeriesDBTest();
    void readACHLazySeriesDBTest();

    /// Read CT Series (01-CT-DICOM_LIVER)
    void readCTSeriesDBTest();
    void readCTLazySeriesDBTest();

    /// Read MR Series (46-MR-BARRE-MONO2-12-shoulder)
    void readMRSeriesDBTest();
    void readMRLazySeriesDBTest();

    /// Read OT Series (42-OT-BARRE-MONO2-8-colon) (Secondary capture)
    void readOTSeriesDBTest();
    void readOTLazySeriesDBTest();

    /// Read unsupported Series (71-CT-DICOM_SEG)
    void readUnsupportedSeriesDBTest();
    void readUnsupportedLazySeriesDBTest();

    /// Read disabled Series (Try to read MR when only CT is enabled)
    void readDisabledSeriesDBTest();
    void readDisabledLazySeriesDBTest();

protected:
    /// Read and check ACH series
    void readACHSeries();

    /// Read and check CT series (01-CT-DICOM_LIVER)
    void readCTSeries();

    /// Read and check MR series (46-MR-BARRE-MONO2-12-shoulder)
    void readMRSeries();

    /// Read and check OT series (42-OT-BARRE-MONO2-8-colon) (Secondary capture)
    void readOTSeries();

    /// Read unsupported series (71-CT-DICOM_SEG)
    void readUnsupportedSeries();

    /// Read disabled series (Try to read MR when only CT is enabled)
    void readDisabledSeries();


};

} // namespace ut
} // namespace fwDcmtkIO

#endif // __FWDCMTKIO_UT_SERIESDBREADERTEST_HPP__

