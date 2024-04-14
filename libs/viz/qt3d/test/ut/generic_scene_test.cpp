/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "generic_scene_test.hpp"

#include "test_application.hpp"

#include "viz/qt3d/core/generic_scene.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::qt3d_test::ut::generic_scene_test);

namespace sight::viz::qt3d_test::ut
{

//------------------------------------------------------------------------------

generic_scene_test::generic_scene_test()
= default;

//------------------------------------------------------------------------------

generic_scene_test::~generic_scene_test()
= default;

//------------------------------------------------------------------------------

void generic_scene_test::setUp()
{
}

//------------------------------------------------------------------------------

void generic_scene_test::tearDown()
{
}

//------------------------------------------------------------------------------

void generic_scene_test::initialize_empty_scene()
{
    test_application app;

    auto* scene     = new sight::viz::qt3d::core::generic_scene(false);
    auto* scene_qml = new sight::viz::qt3d::core::generic_scene(true);

    CPPUNIT_ASSERT(scene != nullptr);
    CPPUNIT_ASSERT(scene->get_camera() != nullptr);
    CPPUNIT_ASSERT(scene->get_camera_controller() != nullptr);
    CPPUNIT_ASSERT(scene->get_frame_graph() != nullptr);
    CPPUNIT_ASSERT(scene->get_input_settings() == nullptr);
    CPPUNIT_ASSERT(scene->get_render_settings() == nullptr);

    CPPUNIT_ASSERT(scene_qml != nullptr);
    CPPUNIT_ASSERT(scene_qml->get_camera() != nullptr);
    CPPUNIT_ASSERT(scene_qml->get_camera_controller() != nullptr);
    CPPUNIT_ASSERT(scene_qml->get_frame_graph() != nullptr);
    CPPUNIT_ASSERT(scene_qml->get_input_settings() != nullptr);
    CPPUNIT_ASSERT(scene_qml->get_render_settings() != nullptr);

    CPPUNIT_ASSERT_EQUAL(scene->get_camera_controller()->camera(), scene->get_camera());
    CPPUNIT_ASSERT_EQUAL(
        qobject_cast<Qt3DRender::QCamera*>(scene->get_frame_graph()->get_camera()),
        scene->get_camera()
    );

    delete scene;
    delete scene_qml;
}

//------------------------------------------------------------------------------

} // namespace sight::viz::qt3d_test::ut
