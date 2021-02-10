/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
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

#include <data/Camera.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <openvslam/camera/perspective.h>

namespace sight::modules::navigation::openvslam::detail
{

namespace ut
{

/**
 * @brief Intrinsic camera calibration conversion tests.
 */
class HelperTest : public CPPUNIT_NS::TestFixture
{
public:

    CPPUNIT_TEST_SUITE( HelperTest );
    CPPUNIT_TEST(toSight);
    CPPUNIT_TEST(fromSight);
    CPPUNIT_TEST(createConfig);
    CPPUNIT_TEST(writeReadConfig);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void toSight();
    void fromSight();
    void createConfig();
    void writeReadConfig();

    /// Comparision of camera, set _sightExpected to true if the expected values are sight camera, false otherwise.
    void compareCam(const data::Camera::csptr _sightCam, const ::openvslam::camera::perspective& _ovsCam,
                    bool _sightExpected = true);

};

}

} // namespace sight::io::opencv
