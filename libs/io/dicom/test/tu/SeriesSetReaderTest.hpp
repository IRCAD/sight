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

namespace sight::io::dicom
{

namespace ut
{

class SeriesSetReaderTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(SeriesSetReaderTest);
CPPUNIT_TEST(readJMSSeriesSetTest);
CPPUNIT_TEST(readCTSeriesSetTest);
CPPUNIT_TEST(readMRSeriesSetTest);
CPPUNIT_TEST(readOTSeriesSetTest);
CPPUNIT_TEST(readSEGSeriesSetTest);
CPPUNIT_TEST(readSFSeriesSetTest);
CPPUNIT_TEST(readSRSeriesSetTest);
CPPUNIT_TEST(read3DSRSeriesSetTest);
CPPUNIT_TEST(readDisabledSeriesSetTest);
CPPUNIT_TEST(readMRSeriesWithDicomDirDBTest);
CPPUNIT_TEST(readMultipleRescaleSeriesSetTest);
CPPUNIT_TEST(readCTWithSurviewSeriesSetTest);
CPPUNIT_TEST(readMRWithTemporalPositionSeriesSetTest);
CPPUNIT_TEST(readCTSeriesSetIssue01Test);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();

    /// Read JMS Series
    static void readJMSSeriesSetTest();

    /// Read CT Series (01-CT-DICOM_LIVER)
    static void readCTSeriesSetTest();

    /// Read MR Series (46-MR-BARRE-MONO2-12-shoulder) (Rotated)
    static void readMRSeriesSetTest();

    /// Read OT Series (42-OT-BARRE-MONO2-8-colon) (Secondary capture)
    static void readOTSeriesSetTest();

    /// Read SEG Series (71-CT-DICOM_SEG)
    static void readSEGSeriesSetTest();

    /// Read SF Series (71-CT-DICOM_SF)
    static void readSFSeriesSetTest();

    /// Read SR Series (71-CT-DICOM_SR)
    static void readSRSeriesSetTest();

    /// Read SR Series (71-CT-DICOM_3DSR)
    static void read3DSRSeriesSetTest();

    /// Read disabled Series (Try to read MR when only CT is enabled)
    static void readDisabledSeriesSetTest();

    /// Read MR Series with DICOMDIR (82-MR-SAGITTAL-KNEE-DICOMDIR)
    static void readMRSeriesWithDicomDirDBTest();

    /// Read multiple rescale Series (83-CT-MultipleRescale)
    static void readMultipleRescaleSeriesSetTest();

    /// Read CT with surview Series (84-CT-Surview)
    static void readCTWithSurviewSeriesSetTest();

    /// Read MR with temporal position identifier Series (85-MR-TemporalPosition)
    static void readMRWithTemporalPositionSeriesSetTest();

    /// Read CT image 01 for stability issue (86-CT-Skull)
    static void readCTSeriesSetIssue01Test();

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
    static void readCTSeriesSetIssue01();
};

} // namespace ut

} // namespace sight::io::dicom
