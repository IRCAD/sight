/************************************************************************
 *
 * Copyright (C) 2022-2025 IRCAD France
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

#include "viz/scene3d/helper/scene.hpp"
#include "viz/scene3d/transformable.hpp"

#include <doctest/doctest.h>

#include <OGRE/OgreSceneNode.h>

#include <cstdint>

// Create a object of type transformable to test it.
class test_transformable : public sight::viz::scene3d::transformable
{
public:

    test_transformable()  = default;
    ~test_transformable() = default;
};

//------------------------------------------------------------------------------

#define ASSERT_NOT_NULL(expr) if((expr) == nullptr){throw std::runtime_error(#expr " is null.");}

TEST_SUITE("sight::viz::scene3d::helper::transformable")
{
    TEST_CASE("transform_node")
    {
        auto* ogre_root                   = sight::viz::scene3d::utils::get_ogre_root();
        Ogre::SceneManager* scene_manager = ogre_root->createSceneManager("DefaultSceneManager", "test");
        Ogre::SceneNode* root_node        = scene_manager->getRootSceneNode();
        ASSERT_NOT_NULL(root_node);

        auto* my_fake_transformable = new test_transformable();
        my_fake_transformable->set_transform_id("TransformTestId");

        const auto id = my_fake_transformable->get_transform_id();
        CHECK_EQ(std::string("TransformTestId"), id);

        auto* const null_transform_node = my_fake_transformable->get_transform_node();
        CHECK(nullptr == null_transform_node);

        auto* const transform_node = my_fake_transformable->get_or_create_transform_node(root_node);
        ASSERT_NOT_NULL(transform_node);

        auto* const transform_node2 = my_fake_transformable->get_transform_node();
        ASSERT_NOT_NULL(transform_node2);

        auto* const child_node = transform_node2->createChildSceneNode("_childNode");
        ASSERT_NOT_NULL(child_node);

        CHECK_NOTHROW(transform_node2->removeAndDestroyChild("_childNode"));

        delete my_fake_transformable;
        ogre_root->destroySceneManager(scene_manager);
    }
}
