/************************************************************************
 *
 * Copyright (C) 2015-2023 IRCAD France
 * Copyright (C) 2015-2021 IHU Strasbourg
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

#include "scene_test.hpp"

#include "viz/scene3d/helper/Scene.hpp"

#include <OGRE/OgreSceneNode.h>

#include <cstdint>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::scene3d::helper::ut::scene_test);

namespace sight::viz::scene3d::helper::ut
{

//------------------------------------------------------------------------------

void scene_test::setUp()
{
}

//------------------------------------------------------------------------------

void scene_test::tearDown()
{
}

//------------------------------------------------------------------------------

#define ASSERT_NOT_NULL(expr) if((expr) == nullptr){throw std::runtime_error(#expr " is null.");}

//------------------------------------------------------------------------------

void scene_test::getNodeById()
{
    auto* ogreRoot                   = Utils::getOgreRoot();
    Ogre::SceneManager* sceneManager = ogreRoot->createSceneManager("DefaultSceneManager", "test");

    Ogre::SceneNode* rootNode = sceneManager->getRootSceneNode();
    ASSERT_NOT_NULL(rootNode);

    auto* node1 = rootNode->createChildSceneNode("Node1");
    auto* node2 = rootNode->createChildSceneNode("Node2");

    auto* node1_1 = node1->createChildSceneNode("Node1_1");
    auto* node1_2 = node1->createChildSceneNode("Node1_2");
    auto* node1_3 = node1->createChildSceneNode("Node1_3");

    auto* node2_1 = node2->createChildSceneNode("Node2_1");
    auto* node2_2 = node2->createChildSceneNode("Node2_2");

    auto* node1_1_1 = node1_1->createChildSceneNode("Node1_1_1");
    auto* node1_2_1 = node1_2->createChildSceneNode("Node1_2_1");
    auto* node1_2_2 = node1_2->createChildSceneNode("Node1_2_2");
    auto* node2_2_1 = node2_2->createChildSceneNode("Node2_2_1");

    CPPUNIT_ASSERT_EQUAL(node1, viz::scene3d::helper::Scene::getNodeById("Node1", rootNode));
    CPPUNIT_ASSERT_EQUAL(node2, viz::scene3d::helper::Scene::getNodeById("Node2", rootNode));

    CPPUNIT_ASSERT_EQUAL(node1_1, viz::scene3d::helper::Scene::getNodeById("Node1_1", rootNode));
    CPPUNIT_ASSERT_EQUAL(node1_2, viz::scene3d::helper::Scene::getNodeById("Node1_2", rootNode));
    CPPUNIT_ASSERT_EQUAL(node1_3, viz::scene3d::helper::Scene::getNodeById("Node1_3", rootNode));

    CPPUNIT_ASSERT_EQUAL(node1_1, viz::scene3d::helper::Scene::getNodeById("Node1_1", node1));
    CPPUNIT_ASSERT_EQUAL(node1_2, viz::scene3d::helper::Scene::getNodeById("Node1_2", node1));
    CPPUNIT_ASSERT_EQUAL(node1_3, viz::scene3d::helper::Scene::getNodeById("Node1_3", node1));

    CPPUNIT_ASSERT_EQUAL(node2_1, viz::scene3d::helper::Scene::getNodeById("Node2_1", rootNode));
    CPPUNIT_ASSERT_EQUAL(node2_2, viz::scene3d::helper::Scene::getNodeById("Node2_2", rootNode));

    CPPUNIT_ASSERT_EQUAL(node1_1_1, viz::scene3d::helper::Scene::getNodeById("Node1_1_1", rootNode));
    CPPUNIT_ASSERT_EQUAL(node1_2_1, viz::scene3d::helper::Scene::getNodeById("Node1_2_1", node1_2));
    CPPUNIT_ASSERT_EQUAL(node1_2_2, viz::scene3d::helper::Scene::getNodeById("Node1_2_2", rootNode));
    CPPUNIT_ASSERT_EQUAL(node2_2_1, viz::scene3d::helper::Scene::getNodeById("Node2_2_1", rootNode));

    Ogre::SceneNode* nullNode = nullptr;
    CPPUNIT_ASSERT_EQUAL(nullNode, viz::scene3d::helper::Scene::getNodeById("Node1_3_1", rootNode));
    CPPUNIT_ASSERT_EQUAL(nullNode, viz::scene3d::helper::Scene::getNodeById("Node2_1_1", rootNode));
    // cspell: ignore AHDFVHDFD
    CPPUNIT_ASSERT_EQUAL(nullNode, viz::scene3d::helper::Scene::getNodeById("AHDFVHDFD", rootNode));
    CPPUNIT_ASSERT_EQUAL(nullNode, viz::scene3d::helper::Scene::getNodeById("Node2_1", node1));

    ogreRoot->destroySceneManager(sceneManager);
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d::helper::ut
