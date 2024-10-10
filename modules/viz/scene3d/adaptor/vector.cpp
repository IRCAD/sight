/************************************************************************
 *
 * Copyright (C) 2019-2024 IRCAD France
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

//-----------------------------------------------------------------------------

service::connections_t vector::auto_connections() const
{
    return {
        {m_length, data::object::MODIFIED_SIG, adaptor::slots::LAZY_UPDATE},
        {m_color, data::object::MODIFIED_SIG, adaptor::slots::LAZY_UPDATE}
    };
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
}

//-----------------------------------------------------------------------------

void vector::starting()
{
    adaptor::init();

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

    this->apply_visibility();

    this->request_render();
}

//-----------------------------------------------------------------------------

void vector::updating()
{
    this->render_service()->make_current();

    this->delete_vector();
    this->create_vector();

    this->set_visible(visible());

    this->update_done();
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

    adaptor::deinit();
}

//-----------------------------------------------------------------------------

void vector::create_vector()
{
    const auto length = static_cast<float>(*m_length);
    // Size, these value allow to display a vector with good enough ratio.
    const float cylinder_length = length - length / 10;
    const float cylinder_radius = length / 80;
    const float cone_length     = length - cylinder_length;
    const float cone_radius     = cylinder_radius * 2;
    const unsigned sample       = 64;

    // Color
    const auto color = *m_color;
    Ogre::ColourValue ogre_color(color[0], color[1], color[2], color[3]);

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

} // namespace sight::module::viz::scene3d::adaptor.
