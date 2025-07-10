/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

#include "module/viz/scene3d/adaptor/grid.hpp"

#include "module/viz/scene3d/adaptor/transform.hpp"

#include <core/com/slots.hxx>

#include <data/tools/color.hpp>

#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/ogre.hpp>

#include <Ogre.h>
#include <OgreAxisAlignedBox.h>
#include <OgreCamera.h>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t UPDATE_LENGTH_SLOT = "updateSize";

//-----------------------------------------------------------------------------

/// Synchronizes the grid layer camera with the adaptor layer camera
struct grid::camera_listener : public Ogre::Camera::Listener
{
    Ogre::Camera* m_camera;

    //------------------------------------------------------------------------------

    explicit camera_listener(Ogre::Camera* _camera) :
        m_camera(_camera)
    {
    }

    //------------------------------------------------------------------------------

    void cameraPreRenderScene(Ogre::Camera* _grid_camera) final
    {
        _grid_camera->getParentSceneNode()->setPosition(m_camera->getParentSceneNode()->getPosition());
        _grid_camera->getParentSceneNode()->setOrientation(m_camera->getParentSceneNode()->getOrientation());
    }
};

//-----------------------------------------------------------------------------

grid::grid() noexcept
{
    new_slot(UPDATE_LENGTH_SLOT, &grid::update_size, this);
}

//-----------------------------------------------------------------------------

void grid::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    // parsing transform or create an "empty" one
    this->set_transform_id(
        config.get<std::string>(
            sight::viz::scene3d::transformable::TRANSFORM_CONFIG,
            gen_id("transform")
        )
    );

    m_size = config.get<float>(CONFIG + "size", m_size);

    const std::string color = config.get(CONFIG + "color", "#FFFFFF");
    std::array<std::uint8_t, 4> rgba {};
    data::tools::color::hexa_string_to_rgba(color, rgba);
    m_color.r = static_cast<float>(rgba[0]) / 255.F;
    m_color.g = static_cast<float>(rgba[1]) / 255.F;
    m_color.b = static_cast<float>(rgba[2]) / 255.F;
    m_color.a = static_cast<float>(rgba[3]) / 255.F;

    m_dash_length = config.get(CONFIG + "dashLength", m_dash_length);
    m_step        = config.get(CONFIG + "step", m_step);
    m_elevation   = config.get(CONFIG + "elevation", m_elevation);
}

//-----------------------------------------------------------------------------

void grid::starting()
{
    adaptor::init();

    this->render_service()->make_current();

    auto grid_layer = render_service()->layer(sight::viz::scene3d::render::render::layer::GRID);
    auto* scene_mgr = grid_layer->get_scene_manager();

    m_line = scene_mgr->createManualObject(gen_id("grid"));
    // Set the line as dynamic, so we can update it later on, when the length changes
    m_line->setDynamic(true);

    // Set the material
    const auto mtl_name = gen_id("material");
    m_material = std::make_unique<sight::viz::scene3d::material::standard>(mtl_name);
    m_material->set_layout(data::mesh::attribute::point_colors);
    m_material->set_shading(data::material::shading_t::ambient, this->layer()->num_lights());

    auto* camera = layer()->get_default_camera();
    m_camera_listener = new camera_listener(camera);
    auto* grid_camera = grid_layer->get_default_camera();
    grid_camera->addListener(m_camera_listener);

    // Draw the line
    this->draw_grid(false);

    this->attach_node(m_line);

    this->set_visible(visible());
}

//-----------------------------------------------------------------------------

void grid::updating()
{
    if(visible())
    {
        this->render_service()->make_current();
        // Draw
        this->draw_grid(true);
    }

    this->update_done();
    this->request_render();
}

//-----------------------------------------------------------------------------

void grid::stopping()
{
    this->render_service()->make_current();

    if(m_camera_listener != nullptr)
    {
        auto grid_layer   = render_service()->layer(sight::viz::scene3d::render::render::layer::GRID);
        auto* grid_camera = grid_layer->get_default_camera();
        grid_camera->removeListener(m_camera_listener);
        delete m_camera_listener;
        m_camera_listener = nullptr;
    }

    m_material.reset();
    if(m_line != nullptr)
    {
        m_line->detachFromParent();
        auto* scene_mgr =
            render_service()->layer(sight::viz::scene3d::render::render::layer::GRID)->get_scene_manager();
        scene_mgr->destroyManualObject(m_line);
        m_line = nullptr;
    }

    adaptor::deinit();
}

//-----------------------------------------------------------------------------

void grid::attach_node(Ogre::MovableObject* _object)
{
    auto* scene_mgr =
        render_service()->layer(sight::viz::scene3d::render::render::layer::GRID)->get_scene_manager();
    Ogre::SceneNode* root_scene_node = scene_mgr->getRootSceneNode();
    Ogre::SceneNode* trans_node      = this->get_or_create_transform_node(root_scene_node);
    SIGHT_ASSERT("Transform node shouldn't be null", trans_node);

    if(const auto orientation = m_orientation.lock(); orientation)
    {
        trans_node->yaw(Ogre::Degree(Ogre::Real(orientation->value()[0])));
        trans_node->pitch(Ogre::Degree(Ogre::Real(orientation->value()[1])));
        trans_node->roll(Ogre::Degree(Ogre::Real(orientation->value()[2])));
    }

    if(const auto position = m_position.lock(); position)
    {
        const auto pos = Ogre::Vector3(
            Ogre::Real(position->value()[0]),
            Ogre::Real(position->value()[1]),
            Ogre::Real(position->value()[2])
        );
        trans_node->setPosition(pos);
    }

    trans_node->setVisible(visible());
    trans_node->attachObject(_object);
}

//-----------------------------------------------------------------------------

void grid::draw_grid(bool _existing_line)
{
    if(!_existing_line)
    {
        m_line->begin(
            m_material->name(),
            Ogre::RenderOperation::OT_LINE_LIST,
            sight::viz::scene3d::RESOURCE_GROUP
        );
    }
    else
    {
        m_line->beginUpdate(0);
    }

    const auto half_size = m_size * 0.5F;
    const auto step_size = m_size / static_cast<float>(m_step);

    float x = -half_size;
    for(std::size_t i = 0 ; i <= m_step ; ++i)
    {
        sight::viz::scene3d::helper::manual_object::draw_dashed_line(
            m_line,
            Ogre::Vector3(x, m_elevation, -half_size),
            Ogre::Vector3(x, m_elevation, half_size),
            m_dash_length,
            m_dash_length,
            m_color
        );
        x += step_size;
    }

    float y = -half_size;
    for(std::size_t i = 0 ; i <= m_step ; ++i)
    {
        sight::viz::scene3d::helper::manual_object::draw_dashed_line(
            m_line,
            Ogre::Vector3(-half_size, m_elevation, y),
            Ogre::Vector3(half_size, m_elevation, y),
            m_dash_length,
            m_dash_length,
            m_color
        );
        y += step_size;
    }

    m_line->end();
}

//-----------------------------------------------------------------------------

void grid::set_visible(bool /*_visible*/)
{
    auto* scene_mgr =
        render_service()->layer(sight::viz::scene3d::render::render::layer::GRID)->get_scene_manager();
    Ogre::SceneNode* root_scene_node = scene_mgr->getRootSceneNode();
    Ogre::SceneNode* trans_node      = this->get_or_create_transform_node(root_scene_node);

    if(const auto orientation = m_orientation.lock(); orientation)
    {
        trans_node->yaw(Ogre::Degree(Ogre::Real(orientation->value()[0])));
        trans_node->pitch(Ogre::Degree(Ogre::Real(orientation->value()[1])));
        trans_node->roll(Ogre::Degree(Ogre::Real(orientation->value()[2])));
    }

    if(const auto position = m_position.lock(); position)
    {
        const auto pos = Ogre::Vector3(
            Ogre::Real(position->value()[0]),
            Ogre::Real(position->value()[1]),
            Ogre::Real(position->value()[2])
        );
        trans_node->setPosition(pos);
    }

    trans_node->setVisible(visible());
    this->updating();
}

//-----------------------------------------------------------------------------

void grid::update_size(float _size)
{
    m_size = _size;
    this->updating();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
