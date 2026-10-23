/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "camera_rotation_fixture.hpp"

namespace sight::module::viz::scene3d::ut
{

namespace
{

struct trackball_camera_fixture : camera_rotation_fixture
{
    trackball_camera_fixture() :
        camera_rotation_fixture("trackball_camera")
    {
    }
};

} // namespace

TEST_SUITE("sight::module::viz::scene3d::adaptor::trackball_camera")
{
    TEST_CASE_FIXTURE(trackball_camera_fixture, "rotation_reference_axes")
    {
        check_rotation_modes();
    }

    TEST_CASE_FIXTURE(trackball_camera_fixture, "restart_refreshes_view_up")
    {
        check_restart();
    }

    TEST_CASE_FIXTURE(trackball_camera_fixture, "view_up_is_read_at_startup")
    {
        set_view_up(Ogre::Vector3::UNIT_Z);
        start(false);
        set_view_up(Ogre::Vector3::UNIT_X);
        camera_node()->setOrientation(tilted_orientation());
        check_horizontal_rotation(Ogre::Vector3::UNIT_Z);
        m_service->stop().get();
        m_service->start().get();
        camera_node()->setOrientation(tilted_orientation());
        check_horizontal_rotation(Ogre::Vector3::UNIT_X);
    }
}

} // namespace sight::module::viz::scene3d::ut
