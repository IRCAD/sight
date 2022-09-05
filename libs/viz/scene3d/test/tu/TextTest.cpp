/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "TextTest.hpp"

#include <utest/Filter.hpp>

#include <viz/scene3d/helper/Font.hpp>
#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/Text.hpp>
#include <viz/scene3d/Utils.hpp>
#include <viz/scene3d/WindowManager.hpp>

#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/Overlay/OgreOverlayManager.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::scene3d::ut::TextTest);

namespace sight::viz::scene3d::ut
{

//------------------------------------------------------------------------------

void TextTest::setUp()
{
}

//------------------------------------------------------------------------------

void TextTest::tearDown()
{
}

//------------------------------------------------------------------------------

void TextTest::factoryTest()
{
    // This is needed for the TextureManager to be instanced, no better way has be found.
    auto* ogreRoot = Utils::getOgreRoot();

    sight::viz::scene3d::WindowManager::sptr mgr = sight::viz::scene3d::WindowManager::get();
    Ogre::RenderWindow* ogreRenderWindow         = mgr->get("test");

    // Load the material manually because the Font will need it
    Ogre::MaterialManager::getSingleton().load("Text", viz::scene3d::RESOURCE_GROUP);

    Ogre::SceneManager* sceneManager = ogreRoot->createSceneManager("DefaultSceneManager", "test");
    const auto& factoryName          = viz::scene3d::factory::Text::FACTORY_TYPE_NAME;
    const auto& textName1            = "HelloWorld";

    Ogre::SceneNode* rootNode = sceneManager->getRootSceneNode();

    auto& overlayManager = Ogre::OverlayManager::getSingleton();

    auto* overlayTextPanel =
        static_cast<Ogre::OverlayContainer*>(overlayManager.createOverlayElement("Panel", "_GUI"));

    auto* const camera = sceneManager->createCamera("TestCamera");
    ogreRenderWindow->addViewport(camera, 0);

    viz::scene3d::Text* textObj1 = viz::scene3d::Text::New(
        "testTest",
        sceneManager,
        overlayTextPanel,
        "DejaVuSans.ttf",
        32,
        96,
        camera
    );
    CPPUNIT_ASSERT(textObj1 != nullptr); // See if it has the right type.

    Ogre::MovableObject* movableText1 = textObj1;
    CPPUNIT_ASSERT(movableText1 != nullptr); // Check if the object was created.

    rootNode->attachObject(textObj1);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), rootNode->getAttachedObjects().size());
    CPPUNIT_ASSERT_EQUAL(movableText1, rootNode->getAttachedObject(0));

    rootNode->detachObject(movableText1);
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), rootNode->getAttachedObjects().size());

    sceneManager->destroyMovableObject(textObj1);
    CPPUNIT_ASSERT_EQUAL(false, sceneManager->hasMovableObject(textName1, factoryName));

    CPPUNIT_ASSERT(sceneManager->getMovableObjects(factoryName).empty());

    ogreRenderWindow->removeViewport(0);
    ogreRoot->destroySceneManager(sceneManager);
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d::ut
