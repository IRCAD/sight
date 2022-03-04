/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include "GenericSceneTest.hpp"

#include "TestApplication.hpp"

#include "viz/qt3d/core/GenericScene.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::qt3dTest::ut::GenericSceneTest);

namespace sight::viz::qt3dTest
{

namespace ut
{

//------------------------------------------------------------------------------

GenericSceneTest::GenericSceneTest()
{
}

//------------------------------------------------------------------------------

GenericSceneTest::~GenericSceneTest()
{
}

//------------------------------------------------------------------------------

void GenericSceneTest::setUp()
{
}

//------------------------------------------------------------------------------

void GenericSceneTest::tearDown()
{
}

//------------------------------------------------------------------------------

void GenericSceneTest::initializeEmptyScene()
{
    TestApplication app;

    auto scene    = new sight::viz::qt3d::core::GenericScene(false);
    auto sceneQml = new sight::viz::qt3d::core::GenericScene(true);

    CPPUNIT_ASSERT(scene != nullptr);
    CPPUNIT_ASSERT(scene->getCamera() != nullptr);
    CPPUNIT_ASSERT(scene->getCameraController() != nullptr);
    CPPUNIT_ASSERT(scene->getFrameGraph() != nullptr);
    CPPUNIT_ASSERT(scene->getInputSettings() == nullptr);
    CPPUNIT_ASSERT(scene->getRenderSettings() == nullptr);

    CPPUNIT_ASSERT(sceneQml != nullptr);
    CPPUNIT_ASSERT(sceneQml->getCamera() != nullptr);
    CPPUNIT_ASSERT(sceneQml->getCameraController() != nullptr);
    CPPUNIT_ASSERT(sceneQml->getFrameGraph() != nullptr);
    CPPUNIT_ASSERT(sceneQml->getInputSettings() != nullptr);
    CPPUNIT_ASSERT(sceneQml->getRenderSettings() != nullptr);

    CPPUNIT_ASSERT_EQUAL(scene->getCameraController()->camera(), scene->getCamera());
    CPPUNIT_ASSERT_EQUAL(qobject_cast<Qt3DRender::QCamera*>(scene->getFrameGraph()->getCamera()), scene->getCamera());

    delete scene;
    delete sceneQml;
}

//------------------------------------------------------------------------------

} // namespace ut.

} // namespace sight::viz::qt3dTest.
