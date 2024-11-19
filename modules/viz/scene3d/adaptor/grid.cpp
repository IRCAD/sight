/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include "modules/viz/scene3d/adaptor/grid.hpp"

#include "modules/viz/scene3d/adaptor/transform.hpp"

#include <core/com/slots.hxx>

#include <data/tools/color.hpp>

#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/ogre.hpp>

#include <Ogre.h>
#include <OgreAxisAlignedBox.h>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t UPDATE_LENGTH_SLOT = "updateSize";

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
            this->get_id() + "_transform"
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

    Ogre::SceneManager* scene_mgr = this->get_scene_manager();

    m_line = scene_mgr->createManualObject(this->get_id() + "_grid");
    // Set the line as dynamic, so we can update it later on, when the length changes
    m_line->setDynamic(true);
    m_line->setQueryFlags(Ogre::SceneManager::STATICGEOMETRY_TYPE_MASK);

    // Set the material
    m_material = std::make_shared<data::material>();

    m_material_adaptor = this->register_service<module::viz::scene3d::adaptor::material>(
        "sight::module::viz::scene3d::adaptor::material"
    );
    m_material_adaptor->set_inout(m_material, module::viz::scene3d::adaptor::material::MATERIAL_INOUT, true);
    m_material_adaptor->configure(
        this->get_id() + m_material_adaptor->get_id(),
        this->get_id() + m_material_adaptor->get_id(),
        this->render_service(),
        m_layer_id,
        "ambient"
    );
    m_material_adaptor->start();

    m_material_adaptor->get_material_fw()->set_has_vertex_color(true);
    m_material_adaptor->update();

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
    this->unregister_services();
    m_material = nullptr;
    if(m_line != nullptr)
    {
        m_line->detachFromParent();
        this->get_scene_manager()->destroyManualObject(m_line);
        m_line = nullptr;
    }

    adaptor::deinit();
}

//-----------------------------------------------------------------------------

void grid::attach_node(Ogre::MovableObject* _object)
{
    Ogre::SceneNode* root_scene_node = this->get_scene_manager()->getRootSceneNode();
    Ogre::SceneNode* trans_node      = this->get_or_create_transform_node(root_scene_node);
    SIGHT_ASSERT("Transform node shouldn't be null", trans_node);

    trans_node->setVisible(visible());
    trans_node->attachObject(_object);
}

//-----------------------------------------------------------------------------

void grid::draw_grid(bool _existing_line)
{
    if(!_existing_line)
    {
        m_line->begin(
            m_material_adaptor->get_material_name(),
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
    Ogre::SceneNode* root_scene_node = this->get_scene_manager()->getRootSceneNode();
    Ogre::SceneNode* trans_node      = this->get_or_create_transform_node(root_scene_node);
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
