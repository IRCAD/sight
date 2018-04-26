/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "TextTest.hpp"

#include <fwRenderOgre/Text.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <OGRE/OgreLogManager.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreSceneNode.h>

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
    // Don't output the log to the terminal and delete the file when the test is done.
    ::Ogre::LogManager* logMgr = new ::Ogre::LogManager();
    logMgr->createLog("OgreTest.log", true, false, true);

    m_ogreRoot = Utils::getOgreRoot();
    ::Ogre::MaterialManager::getSingleton().initialise();
}

//------------------------------------------------------------------------------

void TextTest::tearDown()
{
    m_ogreRoot = nullptr;
    Utils::destroyOgreRoot();
}

//------------------------------------------------------------------------------

void TextTest::factoryTest()
{
    ::Ogre::SceneManager* sceneManager = m_ogreRoot->createSceneManager("DefaultSceneManager", "test");
    const auto& factoryName = ::fwRenderOgre::factory::Text::FACTORY_TYPE_NAME;
    const auto& textName1   = "COUCOU";

    ::Ogre::SceneNode* rootNode = sceneManager->getRootSceneNode();

    ::Ogre::MovableObject* movableText1 = sceneManager->createMovableObject("COUCOU", factoryName);
    CPPUNIT_ASSERT(movableText1 != nullptr); // Check if the object was created.

    ::fwRenderOgre::Text* textObj1 = dynamic_cast< ::fwRenderOgre::Text* >(movableText1);
    CPPUNIT_ASSERT(textObj1 != nullptr); // See if it has the right type.

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
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwRenderOgre
