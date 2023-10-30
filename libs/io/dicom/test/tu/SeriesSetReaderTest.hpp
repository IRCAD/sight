/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

class series_set_reader_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(series_set_reader_test);
CPPUNIT_TEST(read_jms_series_set_test);
CPPUNIT_TEST(read_ct_series_set_test);
CPPUNIT_TEST(read_mr_series_set_test);
CPPUNIT_TEST(read_ot_series_set_test);
CPPUNIT_TEST(read_seg_series_set_test);
CPPUNIT_TEST(read_sf_series_set_test);
CPPUNIT_TEST(read_sr_series_set_test);
CPPUNIT_TEST(read_3d_sr_series_set_test);
CPPUNIT_TEST(read_disabled_series_set_test);
CPPUNIT_TEST(read_mr_series_with_dicom_dir_db_test);
CPPUNIT_TEST(read_multiple_rescale_series_set_test);
CPPUNIT_TEST(read_ct_with_surview_series_set_test);
CPPUNIT_TEST(read_mr_with_temporal_position_series_set_test);
CPPUNIT_TEST(read_ct_series_set_issue01_test);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();

    /// Read JMS Series
    static void read_jms_series_set_test();

    /// Read CT Series (01-CT-DICOM_LIVER)
    static void read_ct_series_set_test();

    /// Read MR Series (46-MR-BARRE-MONO2-12-shoulder) (Rotated)
    static void read_mr_series_set_test();

    /// Read OT Series (42-OT-BARRE-MONO2-8-colon) (Secondary capture)
    static void read_ot_series_set_test();

    /// Read SEG Series (71-CT-DICOM_SEG)
    static void read_seg_series_set_test();

    /// Read SF Series (71-CT-DICOM_SF)
    static void read_sf_series_set_test();

    /// Read SR Series (71-CT-DICOM_SR)
    static void read_sr_series_set_test();

    /// Read SR Series (71-CT-DICOM_3DSR)
    static void read_3d_sr_series_set_test();

    /// Read disabled Series (Try to read MR when only CT is enabled)
    static void read_disabled_series_set_test();

    /// Read MR Series with DICOMDIR (82-MR-SAGITTAL-KNEE-DICOMDIR)
    static void read_mr_series_with_dicom_dir_db_test();

    /// Read multiple rescale Series (83-CT-MultipleRescale)
    static void read_multiple_rescale_series_set_test();

    /// Read CT with surview Series (84-CT-Surview)
    static void read_ct_with_surview_series_set_test();

    /// Read MR with temporal position identifier Series (85-MR-TemporalPosition)
    static void read_mr_with_temporal_position_series_set_test();

    /// Read CT image 01 for stability issue (86-CT-Skull)
    static void read_ct_series_set_issue01_test();

protected:

    /// Read and check JMS series
    static void read_jms_series();

    /// Read and check CT series (01-CT-DICOM_LIVER)
    static void read_ct_series();

    /// Read and check MR series (46-MR-BARRE-MONO2-12-shoulder)
    static void read_mr_series();

    /// Read and check OT series (42-OT-BARRE-MONO2-8-colon) (Secondary capture)
    static void read_ot_series();

    /// Read and check SEG series (71-CT-DICOM_SEG)
    static void read_seg_series();

    /// Read and check SF series (71-CT-DICOM_SF)
    static void read_sf_series();

    /// Read and check SF series (71-CT-DICOM_SR)
    static void read_sr_series();

    /// Read and check SF series (71-CT-DICOM_3DSR)
    static void read_3d_sr_series();

    /// Read disabled series (Try to read MR when only CT is enabled)
    static void read_disabled_series();

    /// Read MR Series with DICOMDIR (82-MR-SAGITTAL-KNEE-DICOMDIR)
    static void read_mr_series_with_dicom_dir();

    /// Read multiple rescale series (83-CT-MultipleRescale)
    static void read_multiple_rescale_series();

    /// Read CT with surview Series (84-CT-Surview)
    static void read_ct_with_surview_series();

    /// Read MR with temporal position identifier Series (85-MR-TemporalPosition)
    static void read_mr_with_temporal_position_series();

    /// Read CT image 01 for stability issue (86-CT-Skull)
    static void read_ct_series_set_issue01();
};

} // namespace ut

} // namespace sight::io::dicom
