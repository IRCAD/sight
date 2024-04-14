/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

namespace sight::geometry::vision::ut
{

class helper_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(helper_test);
CPPUNIT_TEST(reprojection_identity);
CPPUNIT_TEST(reprojection_real_dataset_test1);
CPPUNIT_TEST(reprojection_real_dataset_test2);
CPPUNIT_TEST(tool_calibration_basic);
CPPUNIT_TEST(tool_calibration);
CPPUNIT_TEST(chessboard_detection_test);
CPPUNIT_TEST(chessboard_detection_scale_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void reprojection_real_dataset_test1();
    static void reprojection_real_dataset_test2();
    static void reprojection_identity();
    static void tool_calibration_basic();
    static void tool_calibration();
    static void chessboard_detection_test();
    static void chessboard_detection_scale_test();
};

} // namespace sight::geometry::vision::ut
