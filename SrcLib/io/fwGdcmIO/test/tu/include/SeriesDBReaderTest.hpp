/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWGDCMIO_UT_SERIESDBREADERTEST_HPP__
#define __FWGDCMIO_UT_SERIESDBREADERTEST_HPP__

#include <fwServices/macros.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwGdcmIO
{
namespace ut
{

class SeriesDBReaderTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( SeriesDBReaderTest );
CPPUNIT_TEST( readACHSeriesDBTest );
CPPUNIT_TEST( readCTSeriesDBTest );
CPPUNIT_TEST( readMRSeriesDBTest );
CPPUNIT_TEST( readOTSeriesDBTest );
CPPUNIT_TEST( readSEGSeriesDBTest );
CPPUNIT_TEST( readSFSeriesDBTest );
CPPUNIT_TEST( readSRSeriesDBTest );
CPPUNIT_TEST( read3DSRSeriesDBTest );
CPPUNIT_TEST( readDisabledSeriesDBTest );
CPPUNIT_TEST( readMRSeriesWithDicomDirDBTest );
CPPUNIT_TEST( readMultipleRescaleSeriesDBTest );
CPPUNIT_TEST( readCTWithSurviewSeriesDBTest );
CPPUNIT_TEST( readMRWithTemporalPositionSeriesDBTest );
CPPUNIT_TEST( readCTSeriesDBIssue01Test );
CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();

    /// Read ACH Series
    void readACHSeriesDBTest();

    /// Read CT Series (01-CT-DICOM_LIVER)
    void readCTSeriesDBTest();

    /// Read MR Series (46-MR-BARRE-MONO2-12-shoulder) (Rotated)
    void readMRSeriesDBTest();

    /// Read OT Series (42-OT-BARRE-MONO2-8-colon) (Secondary capture)
    void readOTSeriesDBTest();

    /// Read SEG Series (71-CT-DICOM_SEG)
    void readSEGSeriesDBTest();

    /// Read SF Series (71-CT-DICOM_SF)
    void readSFSeriesDBTest();

    /// Read SR Series (71-CT-DICOM_SR)
    void readSRSeriesDBTest();

    /// Read SR Series (71-CT-DICOM_3DSR)
    void read3DSRSeriesDBTest();

    /// Read disabled Series (Try to read MR when only CT is enabled)
    void readDisabledSeriesDBTest();

    /// Read MR Series with DICOMDIR (82-MR-SAGITTAL-KNEE-DICOMDIR)
    void readMRSeriesWithDicomDirDBTest();

    /// Read multiple rescale Series (83-CT-MultipleRescale)
    void readMultipleRescaleSeriesDBTest();

    /// Read CT with surview Series (84-CT-Surview)
    void readCTWithSurviewSeriesDBTest();

    /// Read MR with temporal position identifier Series (85-MR-TemporalPosition)
    void readMRWithTemporalPositionSeriesDBTest();

    /// Read CT image 01 for stability issue (86-CT-Skull)
    void readCTSeriesDBIssue01Test();

protected:
    /// Read and check ACH series
    void readACHSeries();

    /// Read and check CT series (01-CT-DICOM_LIVER)
    void readCTSeries();

    /// Read and check MR series (46-MR-BARRE-MONO2-12-shoulder)
    void readMRSeries();

    /// Read and check OT series (42-OT-BARRE-MONO2-8-colon) (Secondary capture)
    void readOTSeries();

    /// Read and check SEG series (71-CT-DICOM_SEG)
    void readSEGSeries();

    /// Read and check SF series (71-CT-DICOM_SF)
    void readSFSeries();

    /// Read and check SF series (71-CT-DICOM_SR)
    void readSRSeries();

    /// Read and check SF series (71-CT-DICOM_3DSR)
    void read3DSRSeries();

    /// Read disabled series (Try to read MR when only CT is enabled)
    void readDisabledSeries();

    /// Read MR Series with DICOMDIR (82-MR-SAGITTAL-KNEE-DICOMDIR)
    void readMRSeriesWithDicomDir();

    /// Read multiple rescale series (83-CT-MultipleRescale)
    void readMultipleRescaleSeries();

    /// Read CT with surview Series (84-CT-Surview)
    void readCTWithSurviewSeries();

    /// Read MR with temporal position identifier Series (85-MR-TemporalPosition)
    void readMRWithTemporalPositionSeries();

    /// Read CT image 01 for stability issue (86-CT-Skull)
    void readCTSeriesDBIssue01();
};



} // namespace ut
} // namespace fwGdcmIO

#endif // __FWGDCMIO_UT_SERIESDBREADERTEST_HPP__

