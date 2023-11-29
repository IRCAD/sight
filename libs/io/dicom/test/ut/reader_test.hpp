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

#include <cppunit/extensions/HelperMacros.h>

namespace sight::io::dicom::ut
{

class reader_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(reader_test);
CPPUNIT_TEST(read_jms_series_set_test);
CPPUNIT_TEST(read_ct_series_set_test);
CPPUNIT_TEST(read_mr_series_set_test);
CPPUNIT_TEST(read_ot_series_set_test);
CPPUNIT_TEST(read_multiple_rescale_series_set_test);
CPPUNIT_TEST(read_ct_with_surview_series_set_test);
CPPUNIT_TEST(read_mr_with_temporal_position_series_set_test);
CPPUNIT_TEST(read_ct_series_set_issue01_test);
CPPUNIT_TEST(read_enhanced_us_volume_test);
CPPUNIT_TEST(read_ultrasound_image_test);
CPPUNIT_TEST(read_ultrasound_multiframe_image_test);
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

    /// Read multiple rescale Series (83-CT-MultipleRescale)
    static void read_multiple_rescale_series_set_test();

    /// Read CT with surview Series (84-CT-Surview)
    static void read_ct_with_surview_series_set_test();

    /// Read MR with temporal position identifier Series (85-MR-TemporalPosition)
    static void read_mr_with_temporal_position_series_set_test();

    /// Read CT image 01 for stability issue (86-CT-Skull)
    static void read_ct_series_set_issue01_test();

    /// Read Enhanced US Volume Storage
    static void read_enhanced_us_volume_test();

    /// Read Ultrasound image Storage
    static void read_ultrasound_image_test();

    /// Read Ultrasound Multi-frame image Storage
    static void read_ultrasound_multiframe_image_test();
};

} // namespace sight::io::dicom::ut
