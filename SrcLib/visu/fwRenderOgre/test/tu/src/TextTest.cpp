/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include <fwRenderOgre/helper/Font.hpp>
#include <fwRenderOgre/Text.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <OGRE/OgreLogManager.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/Overlay/OgreOverlayManager.h>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwRenderOgre::ut::TextTest );

namespace fwRenderOgre
{

namespace ut
{

//------------------------------------------------------------------------------

TextTest::TextTest()
{

}

//------------------------------------------------------------------------------

TextTest::~TextTest()
{

}

//------------------------------------------------------------------------------

void TextTest::setUp()
{
    m_ogreRoot = Utils::getOgreRoot();

    // Don't output the log to the terminal and delete the file when the test is done.
    ::Ogre::LogManager* logMgr = ::Ogre::LogManager::getSingletonPtr();
    logMgr->createLog("OgreTest.log", true, false, true);
}

//------------------------------------------------------------------------------

void TextTest::tearDown()
{
    if(m_ogreRoot)
    {
        m_ogreRoot = nullptr;
        Utils::destroyOgreRoot();
    }
}

//------------------------------------------------------------------------------

void TextTest::factoryTest()
{
    //This is needed for the TextureManager to be instanced, no better way has be found.
    auto ogreRenderWindow = m_ogreRoot->createRenderWindow("Dummy-RenderWindow",
                                                           static_cast<unsigned int>(1),
                                                           static_cast<unsigned int>(1),
                                                           false,
                                                           nullptr);
    ogreRenderWindow->setVisible(false);
    ogreRenderWindow->setAutoUpdated(false);
    ::Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    // Load the material manually because the Font will need it
    ::Ogre::MaterialManager::getSingleton().load("Text", ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    ::Ogre::SceneManager* sceneManager = m_ogreRoot->createSceneManager("DefaultSceneManager", "test");
    const auto& factoryName = ::fwRenderOgre::factory::Text::FACTORY_TYPE_NAME;
    const auto& textName1   = "COUCOU";

    ::Ogre::SceneNode* rootNode = sceneManager->getRootSceneNode();

    auto& overlayManager = ::Ogre::OverlayManager::getSingleton();

    auto overlayTextPanel =
        static_cast< ::Ogre::OverlayContainer* >(overlayManager.createOverlayElement("Panel", "_GUI"));

    ::Ogre::FontPtr courrierFont = ::fwRenderOgre::helper::Font::getFont("DejaVuSans.ttf", 32);

    ::fwRenderOgre::Text* textObj1 = ::fwRenderOgre::Text::New("testTest", sceneManager, overlayTextPanel,
                                                               courrierFont);
    CPPUNIT_ASSERT(textObj1 != nullptr); // See if it has the right type.

    ::Ogre::MovableObject* movableText1 = textObj1;
    CPPUNIT_ASSERT(movableText1 != nullptr); // Check if the object was created.

    rootNode->attachObject(textObj1);
    CPPUNIT_ASSERT_EQUAL(size_t(1), rootNode->getAttachedObjects().size());
    CPPUNIT_ASSERT_EQUAL(movableText1, rootNode->getAttachedObject(0));

    rootNode->detachObject(movableText1);
    CPPUNIT_ASSERT_EQUAL(size_t(0), rootNode->getAttachedObjects().size());

    sceneManager->destroyMovableObject(textObj1);
    CPPUNIT_ASSERT_EQUAL(false, sceneManager->hasMovableObject(textName1, factoryName));

    auto movableObjIterator = sceneManager->getMovableObjectIterator(factoryName);
    CPPUNIT_ASSERT(movableObjIterator.end() == movableObjIterator.current());

    m_ogreRoot->destroySceneManager(sceneManager);

    // ogreRenderWindow->destroy() leads to a double delete crash when deleting ogre root node
    // Use the "recommended" way to delete the RenderWindow as it also detach it from root node
    ogreRenderWindow = nullptr;
    m_ogreRoot->getRenderSystem()->destroyRenderWindow("Dummy-RenderWindow");
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwRenderOgre
