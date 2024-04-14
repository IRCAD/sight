/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include <data/camera.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <openvslam/camera/perspective.h>

namespace sight::navigation::openvslam::ut
{

/**
 * @brief Intrinsic camera calibration conversion tests.
 */
class helper_test : public CPPUNIT_NS::TestFixture
{
public:

    CPPUNIT_TEST_SUITE(helper_test);
    CPPUNIT_TEST(to_sight);
    CPPUNIT_TEST(from_sight);
    CPPUNIT_TEST(create_config);
    CPPUNIT_TEST(write_read_config);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    static void to_sight();
    static void from_sight();
    static void create_config();
    static void write_read_config();

    /// Comparision of camera, set _sightExpected to true if the expected values are sight camera, false otherwise.
    static void compare_cam(
        const data::camera& _sight_cam,
        const ::openvslam::camera::perspective& _ovs_cam,
        bool _sight_expected = true
    );
};

} // namespace sight::navigation::openvslam::ut
