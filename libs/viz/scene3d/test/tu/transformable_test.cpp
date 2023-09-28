/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "transformable_test.hpp"

#include "viz/scene3d/helper/Scene.hpp"
#include "viz/scene3d/transformable.hpp"

#include <OGRE/OgreSceneNode.h>

#include <cstdint>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::scene3d::helper::ut::transformableTest);

namespace sight::viz::scene3d::helper::ut
{

// Create a object of type transformable to test it.
class Testtransformable : public transformable
{
public:

    Testtransformable()  = default;
    ~Testtransformable() = default;
};

//------------------------------------------------------------------------------

void transformableTest::setUp()
{
}

//------------------------------------------------------------------------------

void transformableTest::tearDown()
{
}

//------------------------------------------------------------------------------

#define ASSERT_NOT_NULL(expr) if((expr) == nullptr){throw std::runtime_error(#expr " is null.");}

//------------------------------------------------------------------------------

void transformableTest::transformNodeTest()
{
    auto* ogreRoot                   = Utils::getOgreRoot();
    Ogre::SceneManager* sceneManager = ogreRoot->createSceneManager("DefaultSceneManager", "test");

    Ogre::SceneNode* rootNode = sceneManager->getRootSceneNode();
    ASSERT_NOT_NULL(rootNode);

    auto* myFakeTransformable = new Testtransformable();

    myFakeTransformable->setTransformId("TransformTestId");

    const auto id = myFakeTransformable->getTransformId();

    CPPUNIT_ASSERT_EQUAL(std::string("TransformTestId"), id);

    auto* const nullTransformNode = myFakeTransformable->getTransformNode();
    CPPUNIT_ASSERT(nullptr == nullTransformNode);

    auto* const transformNode = myFakeTransformable->getOrCreateTransformNode(rootNode);
    ASSERT_NOT_NULL(transformNode);

    auto* const transformNode2 = myFakeTransformable->getTransformNode();

    ASSERT_NOT_NULL(transformNode2);

    auto* const childNode = transformNode2->createChildSceneNode("_childNode");

    ASSERT_NOT_NULL(childNode);

    CPPUNIT_ASSERT_NO_THROW(transformNode2->removeAndDestroyChild("_childNode"));

    ogreRoot->destroySceneManager(sceneManager);
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d::helper::ut
