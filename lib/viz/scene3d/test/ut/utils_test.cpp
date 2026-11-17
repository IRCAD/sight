/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include <data/image.hpp>
#include <data/mesh.hpp>

#include <utest_data/generator/image.hpp>

#include <viz/scene3d/mesh.hpp>
#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/r2vb_renderable.hpp>
#include <viz/scene3d/utils.hpp>

#include <doctest/doctest.h>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreColourValue.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgrePrerequisites.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreSceneManager.h>

#include <memory>
#include <random>
#include <string>

TEST_SUITE("sight::viz::scene3d::utils")
{
    TEST_CASE("from_ogre_color")
    {
        sight::data::color::sptr ref_color = std::make_shared<sight::data::color>();
        ref_color->set_rgba(1.F, 1.F, 1.F, 1.F);

        sight::data::color::sptr result_color = sight::viz::scene3d::utils::from_ogre_color(Ogre::ColourValue());

        CHECK(static_cast<int>(result_color->red()) == static_cast<int>(ref_color->red()));
        CHECK(static_cast<int>(result_color->green()) == static_cast<int>(ref_color->green()));
        CHECK(static_cast<int>(result_color->blue()) == static_cast<int>(ref_color->blue()));
        CHECK(static_cast<int>(result_color->alpha()) == static_cast<int>(ref_color->alpha()));
    }

    TEST_CASE("convert_ogre_matrix")
    {
        std::mt19937 rng {std::random_device {}()};
        std::uniform_real_distribution<float> dist(-10., 10.);

        // Convert from Sight to ogre and back to Sight.
        {
            sight::data::matrix4::sptr mat0 = std::make_shared<sight::data::matrix4>();
            for(double& coeff : *mat0)
            {
                coeff = dist(rng);
            }

            const Ogre::Matrix4 ogre_mat0 = sight::viz::scene3d::utils::to_ogre_matrix(mat0);
            for(std::uint8_t l = 0 ; l < 4 ; ++l)
            {
                for(std::uint8_t c = 0 ; c < 4 ; ++c)
                {
                    CHECK_EQ((*mat0)(l, c), double(ogre_mat0[l][c]));
                }
            }

            // Convert back to TM3D.
            sight::data::matrix4::sptr mat0_copy = std::make_shared<sight::data::matrix4>();
            sight::viz::scene3d::utils::from_ogre_matrix(ogre_mat0, mat0_copy);
            for(std::uint8_t l = 0 ; l < 4 ; ++l)
            {
                for(std::uint8_t c = 0 ; c < 4 ; ++c)
                {
                    CHECK_EQ((*mat0)(l, c), (*mat0_copy)(l, c));
                    CHECK_EQ(double(ogre_mat0[l][c]), (*mat0_copy)(l, c));
                }
            }
        }

        // Convert from ogre to Sight and back to ogre.
        {
            Ogre::Matrix4 ogre_mat1;
            for(std::uint8_t l = 0 ; l < 4 ; ++l)
            {
                for(std::uint8_t c = 0 ; c < 4 ; ++c)
                {
                    ogre_mat1[l][c] = dist(rng);
                }
            }

            sight::data::matrix4::sptr mat1_copy = std::make_shared<sight::data::matrix4>();
            sight::viz::scene3d::utils::from_ogre_matrix(ogre_mat1, mat1_copy);
            for(std::uint8_t l = 0 ; l < 4 ; ++l)
            {
                for(std::uint8_t c = 0 ; c < 4 ; ++c)
                {
                    CHECK_EQ(double(ogre_mat1[l][c]), (*mat1_copy)(l, c));
                }
            }

            const Ogre::Matrix4 ogre_mat1_copy = sight::viz::scene3d::utils::to_ogre_matrix(mat1_copy);
            for(std::uint8_t l = 0 ; l < 4 ; ++l)
            {
                for(std::uint8_t c = 0 ; c < 4 ; ++c)
                {
                    CHECK_EQ(ogre_mat1[l][c], ogre_mat1_copy[l][c]);
                    CHECK_EQ((*mat1_copy)(l, c), double(ogre_mat1_copy[l][c]));
                }
            }
        }
    }

    TEST_CASE("world_to_slice")
    {
        sight::data::image::sptr image = std::make_shared<sight::data::image>();
        Ogre::Vector3 world_inside_im  = {20., 10., 5.};
        Ogre::Vector3 world_outside_im = {50., -1., 5.};

        // Spacing is 0, throw exception.
        CHECK_THROWS_AS(sight::viz::scene3d::utils::world_to_slices(*image, world_inside_im), sight::core::exception);

        sight::utest_data::generator::image::generate_image(
            image,
            {40, 40, 40},
            {1., 1., 1.},
            {0., 0., 0.},
            {1, 0, 0, 0, 1, 0, 0, 0, 1},
            sight::core::type::UINT8,
            sight::data::image::pixel_format_t::gray_scale
        );

        Ogre::Vector3i slice_idx;
        CHECK_NOTHROW(slice_idx = sight::viz::scene3d::utils::world_to_slices(*image, world_inside_im));
        CHECK_EQ(20, slice_idx[0]);
        CHECK_EQ(10, slice_idx[1]);
        CHECK_EQ(5, slice_idx[2]);

        CHECK_THROWS_AS(sight::viz::scene3d::utils::world_to_slices(*image, world_outside_im), sight::core::exception);
    }
}

// Collision tests

namespace
{

struct ogre_context
{
    ogre_context() :
        root(sight::viz::scene3d::utils::get_ogre_root())
    {
        render_window = root->createRenderWindow("collision_tools_test_window", 640, 480, false);
    }

    Ogre::Root* root {nullptr};
    Ogre::RenderWindow* render_window {nullptr};
};

} // namespace
//------------------------------------------------------------------------------

static ogre_context& get_ogre_context()
{
    static ogre_context context;
    return context;
}

namespace
{

class collision_fixture
{
public:

    collision_fixture()
    {
        auto& context = get_ogre_context();
        m_scene_manager = context.root->createSceneManager("DefaultSceneManager", "collision_tools_test_scene");
        // NOLINTNEXTLINE(cppcoreguidelines-prefer-member-initializer)
        m_camera = m_scene_manager->createCamera(sight::viz::scene3d::DEFAULT_CAMERA_NAME);
        context.render_window->addViewport(m_camera);
        auto* const camera_node = m_scene_manager->getRootSceneNode()->createChildSceneNode();
        camera_node->attachObject(m_camera);
        camera_node->setDirection(Ogre::Vector3::UNIT_Z);
        m_camera->setProjectionType(Ogre::PT_PERSPECTIVE);
        m_camera->setAspectRatio(1.0F);
        m_camera->setNearClipDistance(0.1F);
        m_camera->setFarClipDistance(100.0F);
    }

    ~collision_fixture()
    {
        auto& context = get_ogre_context();
        context.render_window->removeAllViewports();
        context.root->destroySceneManager(m_scene_manager);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] std::optional<sight::viz::scene3d::pick_result_t> pick() const
    {
        m_scene_manager->_updateSceneGraph(m_camera);
        auto& context = get_ogre_context();
        const int x   = static_cast<int>(context.render_window->getWidth() / 2);
        const int y   = static_cast<int>(context.render_window->getHeight() / 2);
        return sight::viz::scene3d::utils::pick_object(
            x,
            y,
            static_cast<Ogre::uint32>(-1),
            *m_scene_manager
        );
    }

    //------------------------------------------------------------------------------

    Ogre::SceneManager& scene_manager()
    {
        return *m_scene_manager;
    }

private:

    Ogre::SceneManager* m_scene_manager {nullptr};
    Ogre::Camera* m_camera {nullptr};
};

} // namespace

//------------------------------------------------------------------------------

static Ogre::Entity* create_manual_entity(
    Ogre::SceneManager& _scene_manager,
    Ogre::RenderOperation::OperationType _operation,
    std::string _name
)
{
    constexpr const char* s_material_name = "collision_tools_test_material";
    if(!Ogre::MaterialManager::getSingleton().resourceExists(s_material_name, sight::viz::scene3d::RESOURCE_GROUP))
    {
        const auto material = Ogre::MaterialManager::getSingleton().create(
            s_material_name,
            sight::viz::scene3d::RESOURCE_GROUP
        );
        material->getTechnique(0)->getPass(0)->setCullingMode(Ogre::CULL_NONE);
    }

    auto* const manual = _scene_manager.createManualObject(_name + "_manual");
    manual->begin(s_material_name, _operation, sight::viz::scene3d::RESOURCE_GROUP);

    if(_operation == Ogre::RenderOperation::OT_POINT_LIST)
    {
        manual->position(0.0F, 0.0F, 5.0F);
    }
    else if(_operation == Ogre::RenderOperation::OT_TRIANGLE_LIST)
    {
        manual->position(-1.0F, -1.0F, 5.0F);
        manual->position(1.0F, -1.0F, 5.0F);
        manual->position(0.0F, 1.0F, 5.0F);
    }
    else
    {
        // OT_LINE_LIST is interpreted as two triangles by collision_tools, as used by the existing quad path.
        manual->position(-1.0F, -1.0F, 5.0F);
        manual->position(1.0F, -1.0F, 5.0F);
        manual->position(1.0F, 1.0F, 5.0F);
        manual->position(-1.0F, 1.0F, 5.0F);
    }

    manual->end();
    const Ogre::MeshPtr mesh = manual->convertToMesh(_name + "_mesh", sight::viz::scene3d::RESOURCE_GROUP);
    _scene_manager.destroyManualObject(manual);

    auto* const entity = _scene_manager.createEntity(mesh);
    _scene_manager.getRootSceneNode()->createChildSceneNode()->attachObject(entity);
    entity->setQueryFlags(static_cast<Ogre::uint32>(-1));
    return entity;
}

//------------------------------------------------------------------------------

static std::shared_ptr<sight::data::mesh> create_data_mesh(sight::data::mesh::cell_type_t _cell_type)
{
    auto mesh = std::make_shared<sight::data::mesh>();
    mesh->reserve(4, 1, _cell_type);
    const auto lock = mesh->dump_lock();

    mesh->push_point(-1.0F, -1.0F, 5.0F);
    mesh->push_point(1.0F, -1.0F, 5.0F);
    mesh->push_point(1.0F, 1.0F, 5.0F);
    mesh->push_point(-1.0F, 1.0F, _cell_type == sight::data::mesh::cell_type_t::quad ? 5.0F : 6.0F);
    mesh->push_cell(0, 1, 2, 3);
    return mesh;
}

//------------------------------------------------------------------------------

static void attach_r2vb_mesh(
    Ogre::SceneManager& _scene_manager,
    sight::data::mesh::cell_type_t _cell_type,
    std::string _name
)
{
    constexpr const char* s_material_name = "collision_tools_test_material";
    if(!Ogre::MaterialManager::getSingleton().resourceExists(s_material_name, sight::viz::scene3d::RESOURCE_GROUP))
    {
        const auto material = Ogre::MaterialManager::getSingleton().create(
            s_material_name,
            sight::viz::scene3d::RESOURCE_GROUP
        );
        material->getTechnique(0)->getPass(0)->setCullingMode(Ogre::CULL_NONE);
    }

    const auto data_mesh = create_data_mesh(_cell_type);
    sight::viz::scene3d::mesh render_mesh(_name);
    render_mesh.update_mesh(data_mesh);
    render_mesh.update_vertices(data_mesh);
    const auto [created, renderables] = render_mesh.update_r2vb(
        data_mesh,
        _scene_manager,
        s_material_name
    );
    REQUIRE(created);
    REQUIRE(renderables.size() == 1);

    renderables.front()->setQueryFlags(static_cast<Ogre::uint32>(-1));
    _scene_manager.getRootSceneNode()->createChildSceneNode()->attachObject(renderables.front());
}

TEST_SUITE("sight::viz::scene3d::utils::pick_object")
{
    TEST_CASE_FIXTURE(collision_fixture, "point_list")
    {
        create_manual_entity(scene_manager(), Ogre::RenderOperation::OT_POINT_LIST, "point");

        const auto result = pick();
        REQUIRE(result.has_value());
        CHECK(doctest::Approx(result->distance) == 4.9F);
    }

    TEST_CASE_FIXTURE(collision_fixture, "line_list_quad")
    {
        create_manual_entity(scene_manager(), Ogre::RenderOperation::OT_LINE_LIST, "line");
        const auto result = pick();
        REQUIRE(result.has_value());
        CHECK(doctest::Approx(result->distance) == 4.9F);
    }

    TEST_CASE_FIXTURE(collision_fixture, "triangle_list")
    {
        create_manual_entity(scene_manager(), Ogre::RenderOperation::OT_TRIANGLE_LIST, "triangle");

        const auto result = pick();
        REQUIRE(result.has_value());
        CHECK(doctest::Approx(result->distance) == 4.9F);
    }

    TEST_CASE_FIXTURE(collision_fixture, "line_list_adjacency_quad")
    {
        attach_r2vb_mesh(scene_manager(), sight::data::mesh::cell_type_t::quad, "quad");
        CHECK(pick().has_value());
        scene_manager().clearScene();
    }

    TEST_CASE_FIXTURE(collision_fixture, "line_list_adjacency_tetra")
    {
        attach_r2vb_mesh(scene_manager(), sight::data::mesh::cell_type_t::tetra, "tetra");
        CHECK(pick().has_value());
        scene_manager().clearScene();
    }
}
