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

#include "viz/qt3d/core/GenericScene.hpp"

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

void generic_scene_test::initializeEmptyScene()
{
    test_application app;

    auto* scene     = new sight::viz::qt3d::core::GenericScene(false);
    auto* scene_qml = new sight::viz::qt3d::core::GenericScene(true);

    CPPUNIT_ASSERT(scene != nullptr);
    CPPUNIT_ASSERT(scene->getCamera() != nullptr);
    CPPUNIT_ASSERT(scene->getCameraController() != nullptr);
    CPPUNIT_ASSERT(scene->getFrameGraph() != nullptr);
    CPPUNIT_ASSERT(scene->getInputSettings() == nullptr);
    CPPUNIT_ASSERT(scene->getRenderSettings() == nullptr);

    CPPUNIT_ASSERT(scene_qml != nullptr);
    CPPUNIT_ASSERT(scene_qml->getCamera() != nullptr);
    CPPUNIT_ASSERT(scene_qml->getCameraController() != nullptr);
    CPPUNIT_ASSERT(scene_qml->getFrameGraph() != nullptr);
    CPPUNIT_ASSERT(scene_qml->getInputSettings() != nullptr);
    CPPUNIT_ASSERT(scene_qml->getRenderSettings() != nullptr);

    CPPUNIT_ASSERT_EQUAL(scene->getCameraController()->camera(), scene->getCamera());
    CPPUNIT_ASSERT_EQUAL(qobject_cast<Qt3DRender::QCamera*>(scene->getFrameGraph()->getCamera()), scene->getCamera());

    delete scene;
    delete scene_qml;
}

//------------------------------------------------------------------------------

} // namespace sight::viz::qt3d_test::ut
