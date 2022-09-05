/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#pragma once

#include <service/macros.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::io::dicom::ut
{

class SeriesDBReaderTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(SeriesDBReaderTest);
CPPUNIT_TEST(readJMSSeriesDBTest);
CPPUNIT_TEST(readCTSeriesDBTest);
CPPUNIT_TEST(readMRSeriesDBTest);
CPPUNIT_TEST(readOTSeriesDBTest);
CPPUNIT_TEST(readSEGSeriesDBTest);
CPPUNIT_TEST(readSFSeriesDBTest);
CPPUNIT_TEST(readSRSeriesDBTest);
CPPUNIT_TEST(read3DSRSeriesDBTest);
CPPUNIT_TEST(readDisabledSeriesDBTest);
CPPUNIT_TEST(readMRSeriesWithDicomDirDBTest);
CPPUNIT_TEST(readMultipleRescaleSeriesDBTest);
CPPUNIT_TEST(readCTWithSurviewSeriesDBTest);
CPPUNIT_TEST(readMRWithTemporalPositionSeriesDBTest);
CPPUNIT_TEST(readCTSeriesDBIssue01Test);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    /// Read JMS Series
    static void readJMSSeriesDBTest();

    /// Read CT Series (01-CT-DICOM_LIVER)
    static void readCTSeriesDBTest();

    /// Read MR Series (46-MR-BARRE-MONO2-12-shoulder) (Rotated)
    static void readMRSeriesDBTest();

    /// Read OT Series (42-OT-BARRE-MONO2-8-colon) (Secondary capture)
    static void readOTSeriesDBTest();

    /// Read SEG Series (71-CT-DICOM_SEG)
    static void readSEGSeriesDBTest();

    /// Read SF Series (71-CT-DICOM_SF)
    static void readSFSeriesDBTest();

    /// Read SR Series (71-CT-DICOM_SR)
    static void readSRSeriesDBTest();

    /// Read SR Series (71-CT-DICOM_3DSR)
    static void read3DSRSeriesDBTest();

    /// Read disabled Series (Try to read MR when only CT is enabled)
    static void readDisabledSeriesDBTest();

    /// Read MR Series with DICOMDIR (82-MR-SAGITTAL-KNEE-DICOMDIR)
    static void readMRSeriesWithDicomDirDBTest();

    /// Read multiple rescale Series (83-CT-MultipleRescale)
    static void readMultipleRescaleSeriesDBTest();

    /// Read CT with surview Series (84-CT-Surview)
    static void readCTWithSurviewSeriesDBTest();

    /// Read MR with temporal position identifier Series (85-MR-TemporalPosition)
    static void readMRWithTemporalPositionSeriesDBTest();

    /// Read CT image 01 for stability issue (86-CT-Skull)
    static void readCTSeriesDBIssue01Test();

protected:

    /// Read and check JMS series
    static void readJMSSeries();

    /// Read and check CT series (01-CT-DICOM_LIVER)
    static void readCTSeries();

    /// Read and check MR series (46-MR-BARRE-MONO2-12-shoulder)
    static void readMRSeries();

    /// Read and check OT series (42-OT-BARRE-MONO2-8-colon) (Secondary capture)
    static void readOTSeries();

    /// Read and check SEG series (71-CT-DICOM_SEG)
    static void readSEGSeries();

    /// Read and check SF series (71-CT-DICOM_SF)
    static void readSFSeries();

    /// Read and check SF series (71-CT-DICOM_SR)
    static void readSRSeries();

    /// Read and check SF series (71-CT-DICOM_3DSR)
    static void read3DSRSeries();

    /// Read disabled series (Try to read MR when only CT is enabled)
    static void readDisabledSeries();

    /// Read MR Series with DICOMDIR (82-MR-SAGITTAL-KNEE-DICOMDIR)
    static void readMRSeriesWithDicomDir();

    /// Read multiple rescale series (83-CT-MultipleRescale)
    static void readMultipleRescaleSeries();

    /// Read CT with surview Series (84-CT-Surview)
    static void readCTWithSurviewSeries();

    /// Read MR with temporal position identifier Series (85-MR-TemporalPosition)
    static void readMRWithTemporalPositionSeries();

    /// Read CT image 01 for stability issue (86-CT-Skull)
    static void readCTSeriesDBIssue01();
};

} // namespace sight::io::dicom::ut
