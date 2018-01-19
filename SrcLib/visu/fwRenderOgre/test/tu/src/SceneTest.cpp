/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "SceneTest.hpp"

#include "fwRenderOgre/helper/Scene.hpp"

#include <OGRE/OgreSceneNode.h>

#include <cstdint>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwRenderOgre::helper::ut::SceneTest );

namespace fwRenderOgre
{
namespace helper
{
namespace ut
{

//------------------------------------------------------------------------------

SceneTest::SceneTest()
{

}

//------------------------------------------------------------------------------

SceneTest::~SceneTest()
{
}

//------------------------------------------------------------------------------

void SceneTest::setUp()
{
    m_ogreRoot = Utils::getOgreRoot();
}

//------------------------------------------------------------------------------

void SceneTest::tearDown()
{
    m_ogreRoot = nullptr;
    Utils::destroyOgreRoot();
}

//------------------------------------------------------------------------------

void SceneTest::getNodeById()
{
    ::Ogre::SceneManager* sceneManager = m_ogreRoot->createSceneManager("DefaultSceneManager", "test");

    ::Ogre::SceneNode* rootNode = sceneManager->getRootSceneNode();
    CPPUNIT_ASSERT(nullptr != rootNode);

    auto node1 = rootNode->createChildSceneNode("Node1");
    auto node2 = rootNode->createChildSceneNode("Node2");

    auto node1_1 = node1->createChildSceneNode("Node1_1");
    auto node1_2 = node1->createChildSceneNode("Node1_2");
    auto node1_3 = node1->createChildSceneNode("Node1_3");

    auto node2_1 = node2->createChildSceneNode("Node2_1");
    auto node2_2 = node2->createChildSceneNode("Node2_2");

    auto node1_1_1 = node1_1->createChildSceneNode("Node1_1_1");
    auto node1_2_1 = node1_2->createChildSceneNode("Node1_2_1");
    auto node1_2_2 = node1_2->createChildSceneNode("Node1_2_2");
    auto node2_2_1 = node2_2->createChildSceneNode("Node2_2_1");

    CPPUNIT_ASSERT_EQUAL(node1, ::fwRenderOgre::helper::Scene::getNodeById("Node1", rootNode));
    CPPUNIT_ASSERT_EQUAL(node2, ::fwRenderOgre::helper::Scene::getNodeById("Node2", rootNode));

    CPPUNIT_ASSERT_EQUAL(node1_1, ::fwRenderOgre::helper::Scene::getNodeById("Node1_1", rootNode));
    CPPUNIT_ASSERT_EQUAL(node1_2, ::fwRenderOgre::helper::Scene::getNodeById("Node1_2", rootNode));
    CPPUNIT_ASSERT_EQUAL(node1_3, ::fwRenderOgre::helper::Scene::getNodeById("Node1_3", rootNode));

    CPPUNIT_ASSERT_EQUAL(node1_1, ::fwRenderOgre::helper::Scene::getNodeById("Node1_1", node1));
    CPPUNIT_ASSERT_EQUAL(node1_2, ::fwRenderOgre::helper::Scene::getNodeById("Node1_2", node1));
    CPPUNIT_ASSERT_EQUAL(node1_3, ::fwRenderOgre::helper::Scene::getNodeById("Node1_3", node1));

    CPPUNIT_ASSERT_EQUAL(node2_1, ::fwRenderOgre::helper::Scene::getNodeById("Node2_1", rootNode));
    CPPUNIT_ASSERT_EQUAL(node2_2, ::fwRenderOgre::helper::Scene::getNodeById("Node2_2", rootNode));

    CPPUNIT_ASSERT_EQUAL(node1_1_1, ::fwRenderOgre::helper::Scene::getNodeById("Node1_1_1", rootNode));
    CPPUNIT_ASSERT_EQUAL(node1_2_1, ::fwRenderOgre::helper::Scene::getNodeById("Node1_2_1", node1_2));
    CPPUNIT_ASSERT_EQUAL(node1_2_2, ::fwRenderOgre::helper::Scene::getNodeById("Node1_2_2", rootNode));
    CPPUNIT_ASSERT_EQUAL(node2_2_1, ::fwRenderOgre::helper::Scene::getNodeById("Node2_2_1", rootNode));

    ::Ogre::SceneNode* nullNode = nullptr;
    CPPUNIT_ASSERT_EQUAL(nullNode, ::fwRenderOgre::helper::Scene::getNodeById("Node1_3_1", rootNode));
    CPPUNIT_ASSERT_EQUAL(nullNode, ::fwRenderOgre::helper::Scene::getNodeById("Node2_1_1", rootNode));
    CPPUNIT_ASSERT_EQUAL(nullNode, ::fwRenderOgre::helper::Scene::getNodeById("AHDFVHDFD", rootNode));
    CPPUNIT_ASSERT_EQUAL(nullNode, ::fwRenderOgre::helper::Scene::getNodeById("Node2_1", node1));
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace helper
} //namespace fwRenderOgre
