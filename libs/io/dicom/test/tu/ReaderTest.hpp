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

#include <cppunit/extensions/HelperMacros.h>

namespace sight::io::dicom::ut
{

class ReaderTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(ReaderTest);
CPPUNIT_TEST(readJMSSeriesSetTest);
CPPUNIT_TEST(readCTSeriesSetTest);
CPPUNIT_TEST(readMRSeriesSetTest);
CPPUNIT_TEST(readOTSeriesSetTest);
CPPUNIT_TEST(readMultipleRescaleSeriesSetTest);
CPPUNIT_TEST(readCTWithSurviewSeriesSetTest);
CPPUNIT_TEST(readMRWithTemporalPositionSeriesSetTest);
CPPUNIT_TEST(readCTSeriesSetIssue01Test);
CPPUNIT_TEST(readEnhancedUSVolumeTest);
CPPUNIT_TEST(readUltrasoundImageTest);
CPPUNIT_TEST(readUltrasoundMultiframeImageTest);
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

    /// Read multiple rescale Series (83-CT-MultipleRescale)
    static void readMultipleRescaleSeriesSetTest();

    /// Read CT with surview Series (84-CT-Surview)
    static void readCTWithSurviewSeriesSetTest();

    /// Read MR with temporal position identifier Series (85-MR-TemporalPosition)
    static void readMRWithTemporalPositionSeriesSetTest();

    /// Read CT image 01 for stability issue (86-CT-Skull)
    static void readCTSeriesSetIssue01Test();

    /// Read Enhanced US Volume Storage
    static void readEnhancedUSVolumeTest();

    /// Read Ultrasound Image Storage
    static void readUltrasoundImageTest();

    /// Read Ultrasound Multi-frame Image Storage
    static void readUltrasoundMultiframeImageTest();
};

} // namespace sight::io::dicom::ut
