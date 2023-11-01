/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/vector.hpp"

#include "modules/viz/scene3d/adaptor/transform.hpp"

#include <core/com/slots.hxx>

#include <data/tools/color.hpp>

#include <service/macros.hpp>

#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/helper/scene.hpp>
#include <viz/scene3d/render.hpp>

#include <OgreNode.h>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t UPDATE_LENGTH_SLOT = "update_length";

//-----------------------------------------------------------------------------

vector::vector() noexcept
{
    new_slot(UPDATE_LENGTH_SLOT, &vector::update_length, this);
}

//-----------------------------------------------------------------------------

void vector::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    const std::string transform_id = config.get<std::string>(
        module::viz::scene3d::adaptor::transform::TRANSFORM_CONFIG,
        this->get_id() + "_transform"
    );

    this->set_transform_id(transform_id);
    m_length = config.get<float>(CONFIG + "length", m_length);
    m_color  = config.get<std::string>(CONFIG + "color", m_color);
    SIGHT_ASSERT(
        "Color string should start with '#' and followed by 6 or 8 "
        "hexadecimal digits. Given color: " << m_color,
        m_color[0] == '#'
        && (m_color.length() == 7 || m_color.length() == 9)
    );
}

//-----------------------------------------------------------------------------

void vector::starting()
{
    this->initialize();

    this->render_service()->make_current();

    Ogre::SceneNode* root_scene_node = this->get_scene_manager()->getRootSceneNode();
    Ogre::SceneNode* transform_node  = this->get_or_create_transform_node(root_scene_node);
    m_scene_node = transform_node->createChildSceneNode(this->get_id() + "_mainNode");

    // set the material
    m_material = std::make_shared<data::material>();

    m_material_adaptor = this->register_service<module::viz::scene3d::adaptor::material>(
        "sight::module::viz::scene3d::adaptor::material"
    );
    m_material_adaptor->set_inout(m_material, module::viz::scene3d::adaptor::material::MATERIAL_INOUT, true);
    m_material_adaptor->configure(
        this->get_id() + m_material_adaptor->get_id(),
        this->get_id() + m_material_adaptor->get_id(),
        this->render_service(),
        m_layer_id
    );
    m_material_adaptor->start();

    m_material_adaptor->get_material_fw()->set_has_vertex_color(true);
    m_material_adaptor->update();

    this->create_vector();

    this->update_visibility(m_visible);

    this->request_render();
}

//-----------------------------------------------------------------------------

void vector::updating()
{
    this->render_service()->make_current();

    this->delete_vector();
    this->create_vector();

    this->set_visible(m_visible);

    this->request_render();
}

//-----------------------------------------------------------------------------

void vector::stopping()
{
    this->render_service()->make_current();

    this->delete_vector();

    Ogre::SceneNode* transform_node = this->get_transform_node();
    if(transform_node != nullptr)
    {
        transform_node->removeAndDestroyChild(this->get_id() + "_mainNode");
    }

    this->unregister_services();
    m_material.reset();
}

//-----------------------------------------------------------------------------

void vector::create_vector()
{
    // Size, these value allow to display a vector with good enough ratio.
    const float cylinder_length = m_length - m_length / 10;
    const float cylinder_radius = m_length / 80;
    const float cone_length     = m_length - cylinder_length;
    const float cone_radius     = cylinder_radius * 2;
    const unsigned sample       = 64;

    // Color
    std::array<std::uint8_t, 4> color {};
    data::tools::color::hexa_string_to_rgba(m_color, color);
    Ogre::ColourValue ogre_color(float(color[0]) / 255.F, float(color[1]) / 255.F, float(color[2]) / 255.F);

    // Draw
    Ogre::SceneManager* scene_mgr = this->get_scene_manager();
    m_line = scene_mgr->createManualObject(this->get_id() + "_line");
    m_cone = scene_mgr->createManualObject(this->get_id() + "_cone");

    // Line
    sight::viz::scene3d::helper::manual_object::create_cylinder(
        m_line,
        m_material_adaptor->get_material_name(),
        ogre_color,
        cylinder_radius,
        cylinder_length,
        sample
    );
    Ogre::SceneNode* line_node = m_scene_node->createChildSceneNode(this->get_id() + "_lineNode");
    line_node->attachObject(m_line);
    // Rotate around y axis to create the cylinder on z Axis (consistent with line adaptor)
    line_node->yaw(Ogre::Degree(-90));

    // Cone
    sight::viz::scene3d::helper::manual_object::create_cone(
        m_cone,
        m_material_adaptor->get_material_name(),
        ogre_color,
        cone_radius,
        cone_length,
        sample
    );
    Ogre::SceneNode* cone_node = m_scene_node->createChildSceneNode(this->get_id() + "_coneNode");

    cone_node->attachObject(m_cone);
    cone_node->translate(0.F, 0.F, cylinder_length);
    cone_node->yaw(Ogre::Degree(-90));
}

//-----------------------------------------------------------------------------

void vector::delete_vector()
{
    if(m_scene_node != nullptr)
    {
        m_scene_node->removeAndDestroyChild(this->get_id() + "_lineNode");
        m_scene_node->removeAndDestroyChild(this->get_id() + "_coneNode");
    }

    Ogre::SceneManager* scene_mgr = this->get_scene_manager();

    scene_mgr->destroyManualObject(m_line);
    scene_mgr->destroyManualObject(m_cone);
}

//-----------------------------------------------------------------------------

void vector::set_visible(bool _visible)
{
    if(m_scene_node != nullptr)
    {
        m_scene_node->setVisible(_visible);
    }

    this->request_render();
}

//-----------------------------------------------------------------------------

void vector::update_length(float _length)
{
    m_length = _length;

    this->update();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
