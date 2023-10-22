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

#include "viz/scene3d/helper/scene.hpp"
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
    auto* ogre_root                   = utils::getOgreRoot();
    Ogre::SceneManager* scene_manager = ogre_root->createSceneManager("DefaultSceneManager", "test");

    Ogre::SceneNode* root_node = scene_manager->getRootSceneNode();
    ASSERT_NOT_NULL(root_node);

    auto* my_fake_transformable = new Testtransformable();

    my_fake_transformable->setTransformId("TransformTestId");

    const auto id = my_fake_transformable->getTransformId();

    CPPUNIT_ASSERT_EQUAL(std::string("TransformTestId"), id);

    auto* const null_transform_node = my_fake_transformable->getTransformNode();
    CPPUNIT_ASSERT(nullptr == null_transform_node);

    auto* const transform_node = my_fake_transformable->getOrCreateTransformNode(root_node);
    ASSERT_NOT_NULL(transform_node);

    auto* const transform_node2 = my_fake_transformable->getTransformNode();

    ASSERT_NOT_NULL(transform_node2);

    auto* const child_node = transform_node2->createChildSceneNode("_childNode");

    ASSERT_NOT_NULL(child_node);

    CPPUNIT_ASSERT_NO_THROW(transform_node2->removeAndDestroyChild("_childNode"));

    ogre_root->destroySceneManager(scene_manager);
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d::helper::ut
