/************************************************************************
 *
 * Copyright (C) 2018-2026 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "data/camera.hpp"

#include "viz/scene3d/utils.hpp"

#include <core/runtime/runtime.hpp>

#include <utest/filter.hpp>

#include <viz/scene3d/helper/camera.hpp>
#include <viz/scene3d/window_manager.hpp>

#include <doctest/doctest.h>

#include <OgreLogManager.h>
#include <OgreMatrix4.h>
#include <OgreRenderWindow.h>
#include <OgreViewport.h>

namespace
{

struct initializer
{
    initializer()
    {
        sight::core::runtime::init();
        sight::core::runtime::load_module("sight::module::viz::scene3d::test");
    }
};

//------------------------------------------------------------------------------

void compare_matrix(const Ogre::Matrix4& _m1, const Ogre::Matrix4& _m2)
{
    for(unsigned int i = 0 ; i < 4 ; ++i)
    {
        for(unsigned int j = 0 ; j < 4 ; ++j)
        {
            CHECK(doctest::Approx(_m1[i][j]).epsilon(0.00000000000001) == _m2[i][j]);
        }
    }
}

//------------------------------------------------------------------------------

void compare_point(const Ogre::Vector4& _p1, const Ogre::Vector3& _p2)
{
    CHECK(doctest::Approx(_p1[0]).epsilon(0.0001) == _p2[0]);
    CHECK(doctest::Approx(_p1[1]).epsilon(0.0001) == _p2[1]);
    CHECK(doctest::Approx(_p1[2]).epsilon(0.0001) == _p2[2]);
}

} // namespace

//------------------------------------------------------------------------------

TEST_SUITE("sight::viz::scene3d::camera")
{
    TEST_CASE("compute_projection_matrix")
    {
        const double cx          = 988.898;
        const double cy          = 553.769;
        const double fx          = 1590.59;
        const double fy          = 1628.71;
        const std::size_t width  = 1920;
        const std::size_t height = 1080;
        const float n            = 0.1F;
        const float f            = 100;

        // Original matrix
        Ogre::Matrix4 expected(
            1.6568645238876F, 0.F, -0.0295966863632202F, 0.F,
            0.F, 3.01612949371338F, 0.0245949625968933F, 0.F,
            0.F, 0.F, -1.00200200080872F, -0.200200200080872F,
            0.F, 0.F, -1.F, 0.F
        );

        // Original camera
        auto camera = std::make_shared<sight::data::camera>();
        camera->set_cx(cx);
        camera->set_cy(cy);
        camera->set_fx(fx);
        camera->set_fy(fy);
        camera->set_width(width);
        camera->set_height(height);

        Ogre::Matrix4 actual = sight::viz::scene3d::helper::camera::compute_projection_matrix(
            *camera,
            static_cast<float>(camera->get_width()),
            static_cast<float>(camera->get_height()),
            n,
            f
        );
        compare_matrix(expected, actual);
    }

    TEST_CASE_FIXTURE(initializer, "convert_pixel_to_world_space")
    {
        auto* const root          = sight::viz::scene3d::utils::get_ogre_root();
        auto* const scene_manager = root->createSceneManager("DefaultSceneManager", "TestSceneManager");

        sight::viz::scene3d::window_manager::sptr mgr = sight::viz::scene3d::window_manager::get();
        Ogre::RenderWindow* render_window             = mgr->get("test");
        auto* const camera                            = scene_manager->createCamera("TestCamera");
        render_window->addViewport(camera, 0);

        const auto width  = static_cast<float>(render_window->getWidth());
        const auto height = static_cast<float>(render_window->getHeight());

        camera->setNearClipDistance(1);
        camera->setFarClipDistance(10);
        camera->setAutoAspectRatio(true);
        camera->setOrthoWindowWidth(1920);
        camera->setOrthoWindowHeight(1080);
        camera->setProjectionType(Ogre::ProjectionType::PT_PERSPECTIVE);

        {
            // Manually project a point
            const Ogre::Vector4 standard_point(1.F, 2.F, 3.F, 1.F);
            const Ogre::Vector4 clipped_point = camera->getProjectionMatrix() * camera->getViewMatrix()
                                                * standard_point;
            const Ogre::Vector3 ndc_point = clipped_point.xyz() / clipped_point.w;

            // /!\ in openGl, y coordinate begin from the upper left corner, we need to set him from the lower left
            // corner.
            const Ogre::Real f_x = (ndc_point.x + 1.F) * 0.5F;
            const Ogre::Real f_y = 1.F - (ndc_point.y + 1.F) * 0.5F;
            const Ogre::Real f_z = (ndc_point.z + 1.F) * 0.5F;
            const Ogre::Vector3 viewport_point(f_x, f_y, f_z);

            // Unproject the projected point
            const Ogre::Vector3 point             = viewport_point * Ogre::Vector3(width, height, 1);
            const Ogre::Vector3 unprojected_point =
                sight::viz::scene3d::helper::camera::convert_screen_space_to_view_space(*camera, point);

            compare_point(standard_point, unprojected_point);
        }

        camera->setProjectionType(Ogre::ProjectionType::PT_ORTHOGRAPHIC);

        {
            // Manually project a point
            const Ogre::Vector4 standard_point(1.F, 2.F, 3.F, 1.F);
            const Ogre::Vector4 clipped_point = camera->getProjectionMatrix() * camera->getViewMatrix()
                                                * standard_point;
            const Ogre::Vector3 ndc_point = clipped_point.xyz() / clipped_point.w;

            // /!\ in openGl, y coordinate begin from the upper left corner, we need to set him from the lower left
            // corner.
            const Ogre::Real f_x = (ndc_point.x + 1.F) * 0.5F;
            const Ogre::Real f_y = 1.F - (ndc_point.y + 1.F) * 0.5F;
            const Ogre::Real f_z = (ndc_point.z + 1.F) * 0.5F;
            const Ogre::Vector3 viewport_point(f_x, f_y, f_z);

            // Unproject the projected point
            const Ogre::Vector3 point             = viewport_point * Ogre::Vector3(width, height, 1);
            const Ogre::Vector3 unprojected_point =
                sight::viz::scene3d::helper::camera::convert_screen_space_to_view_space(*camera, point);

            compare_point(standard_point, unprojected_point);
        }

        render_window->removeViewport(0);
        root->destroySceneManager(scene_manager);
    }

    TEST_CASE_FIXTURE(initializer, "convert_world_space_to_screen_space")
    {
        auto* const root          = sight::viz::scene3d::utils::get_ogre_root();
        auto* const scene_manager = root->createSceneManager("DefaultSceneManager", "TestSceneManager");

        sight::viz::scene3d::window_manager::sptr mgr = sight::viz::scene3d::window_manager::get();
        Ogre::RenderWindow* render_window             = mgr->get("test");
        auto* const camera                            = scene_manager->createCamera("TestCamera");
        render_window->addViewport(camera, 0);

        camera->setNearClipDistance(1);
        camera->setFarClipDistance(10);
        camera->setAutoAspectRatio(true);
        camera->setOrthoWindowWidth(1920);
        camera->setOrthoWindowHeight(1080);

        auto* camera_node = scene_manager->createSceneNode();
        const Ogre::Quaternion rotate_x(Ogre::Degree(65), Ogre::Vector3(1, 0, 0));
        const Ogre::Quaternion rotate_y(Ogre::Degree(-176), Ogre::Vector3(0, 1, 0));
        camera_node->setOrientation(rotate_x * rotate_y);
        camera_node->setPosition(-12.F, 5.F, 234.F);

        camera->setProjectionType(Ogre::ProjectionType::PT_PERSPECTIVE);

        {
            const Ogre::Vector3 standard_point(-4.F, 4.F, 3.F);
            const Ogre::Vector2 projected_point =
                sight::viz::scene3d::helper::camera::convert_world_space_to_screen_space(
                    *camera,
                    standard_point
                );

            const Ogre::Vector2 point(341.4213F, 421.8951F);
            CHECK(doctest::Approx(projected_point[0]).epsilon(0.0001) == point[0]);
            CHECK(doctest::Approx(projected_point[1]).epsilon(0.0001) == point[1]);
        }

        camera->setProjectionType(Ogre::ProjectionType::PT_ORTHOGRAPHIC);

        {
            const Ogre::Vector3 standard_point(-4.F, 87.F, 3.F);
            const Ogre::Vector2 projected_point =
                sight::viz::scene3d::helper::camera::convert_world_space_to_screen_space(
                    *camera,
                    standard_point
                );

            const Ogre::Vector2 point(99.4444F, 83.8888F);
            CHECK(doctest::Approx(projected_point[0]).epsilon(0.0001) == point[0]);
            CHECK(doctest::Approx(projected_point[1]).epsilon(0.0001) == point[1]);
        }

        render_window->removeViewport(0);
        root->destroySceneManager(scene_manager);
    }
} // TEST_SUITE
