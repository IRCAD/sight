/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/light.hpp"

#include <core/com/slots.hxx>

#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/registry/macros.hpp>
#include <viz/scene3d/render.hpp>
#include <viz/scene3d/utils.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreMath.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t SET_X_OFFSET_SLOT = "setXOffset";
static const core::com::slots::key_t SET_Y_OFFSET_SLOT = "setYOffset";

static const service::base::key_t DIFFUSE_COLOR_INOUT  = "diffuseColor";
static const service::base::key_t SPECULAR_COLOR_INOUT = "specularColor";

SIGHT_REGISTER_SCENE3D_LIGHT(
    sight::module::viz::scene3d::adaptor::light,
    sight::viz::scene3d::light_adaptor::REGISTRY_KEY
)

//------------------------------------------------------------------------------

light::light() noexcept
{
    new_slot(SET_X_OFFSET_SLOT, &light::set_theta_offset, this);
    new_slot(SET_Y_OFFSET_SLOT, &light::set_phi_offset, this);
}

//------------------------------------------------------------------------------

void light::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    this->set_transform_id(
        config.get<std::string>(
            sight::viz::scene3d::transformable::TRANSFORM_CONFIG,
            gen_id("transform")
        )
    );

    static const std::string s_NAME_CONFIG         = CONFIG + "name";
    static const std::string s_SWITCHED_ON_CONFIG  = CONFIG + "switchedOn";
    static const std::string s_THETA_OFFSET_CONFIG = CONFIG + "thetaOffset";
    static const std::string s_PHI_OFFSET_CONFIG   = CONFIG + "phiOffset";

    m_light_name   = config.get<std::string>(s_NAME_CONFIG);
    m_switched_on  = config.get<bool>(s_SWITCHED_ON_CONFIG, m_switched_on);
    m_theta_offset = config.get<float>(s_THETA_OFFSET_CONFIG, m_theta_offset);
    m_phi_offset   = config.get<float>(s_PHI_OFFSET_CONFIG, m_phi_offset);
}

//------------------------------------------------------------------------------

void light::starting()
{
    adaptor::init();

    this->render_service()->make_current();

    Ogre::SceneManager* const scene_mgr = this->get_scene_manager();
    m_light = scene_mgr->createLight(gen_id("" + m_light_name));

    // Sets the default light direction to the camera's view direction,
    m_light->setType(m_light_type);
    m_light->setVisible(m_switched_on);

    Ogre::SceneNode* root_scene_node = this->get_scene_manager()->getRootSceneNode();
    Ogre::SceneNode* trans_node      = this->get_or_create_transform_node(root_scene_node);
    m_light_node = trans_node->createChildSceneNode(gen_id("light"));
    m_light_node->attachObject(m_light);

    if(m_theta_offset != 0.F || m_phi_offset != 0.F)
    {
        this->set_theta_offset(m_theta_offset);
        this->set_phi_offset(m_phi_offset);
    }

    if(m_light_name != sight::viz::scene3d::layer::DEFAULT_LIGHT_NAME)
    {
        // Creates the visual feedback
        m_material = std::make_unique<sight::viz::scene3d::material::standard>(gen_id("material"));
        m_material->set_layout(data::mesh::attribute::point_normals | data::mesh::attribute::point_colors);
        m_material->set_shading(sight::data::material::shading_t::phong, this->layer()->num_lights());

        // Size, these value allow to display light with good enough ratio.
        const float origin_radius   = m_length * 0.1F;
        const float cylinder_length = m_length - m_length / 10;
        const float cylinder_radius = m_length / 80;
        const float cone_length     = m_length - cylinder_length;
        const float cone_radius     = cylinder_radius * 2;
        const unsigned sample       = 64;

        // Creates the commun sphere position
        m_light_position = scene_mgr->createManualObject(gen_id("origin"));
        sight::viz::scene3d::helper::manual_object::create_sphere(
            m_light_position,
            m_material->name(),
            Ogre::ColourValue(0.98F, 0.96F, 0.62F, 1.0F),
            origin_radius,
            sample
        );
        m_light_position->setVisible(m_visual_feedback);
        m_light_node->attachObject(m_light_position);

        // Create the directional light feedback
        m_directional_feedback.first  = scene_mgr->createManualObject(gen_id("line"));
        m_directional_feedback.second = scene_mgr->createManualObject(gen_id("cone"));

        sight::viz::scene3d::helper::manual_object::create_cylinder(
            m_directional_feedback.first,
            m_material->name(),
            Ogre::ColourValue(0.F, 0.F, 1.F, 1.0F),
            cylinder_radius,
            cylinder_length,
            sample
        );
        Ogre::SceneNode* line_node = m_light_node->createChildSceneNode(gen_id("lineNode"));
        line_node->attachObject(m_directional_feedback.first);
        line_node->yaw(Ogre::Degree(-90));

        sight::viz::scene3d::helper::manual_object::create_cone(
            m_directional_feedback.second,
            m_material->name(),
            Ogre::ColourValue(0.F, 0.F, 1.F, 1.0F),
            cone_radius,
            cone_length,
            sample
        );
        Ogre::SceneNode* cone_node = m_light_node->createChildSceneNode(gen_id("coneNode"));

        cone_node->attachObject(m_directional_feedback.second);
        cone_node->translate(0.F, 0.F, cylinder_length);
        cone_node->yaw(Ogre::Degree(-90));

        m_directional_feedback.first->setVisible(m_visual_feedback && m_light_type == Ogre::Light::LT_DIRECTIONAL);
        m_directional_feedback.second->setVisible(m_visual_feedback && m_light_type == Ogre::Light::LT_DIRECTIONAL);
    }

    updating();
}

//-----------------------------------------------------------------------------

service::connections_t light::auto_connections() const
{
    service::connections_t connections = adaptor::auto_connections();
    connections.push(DIFFUSE_COLOR_INOUT, data::color::MODIFIED_SIG, adaptor::slots::LAZY_UPDATE);
    connections.push(SPECULAR_COLOR_INOUT, data::color::MODIFIED_SIG, adaptor::slots::LAZY_UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void light::updating()
{
    const auto light_diffuse_color  = m_diffuse.lock();
    const auto light_specular_color = m_specular.lock();

    this->render_service()->make_current();

    const Ogre::ColourValue diffuse_color(light_diffuse_color->red(),
                                          light_diffuse_color->green(),
                                          light_diffuse_color->blue(),
                                          light_diffuse_color->alpha());

    const Ogre::ColourValue specular_color(light_specular_color->red(),
                                           light_specular_color->green(),
                                           light_specular_color->blue(),
                                           light_specular_color->alpha());

    m_light->setDiffuseColour(diffuse_color);
    m_light->setSpecularColour(specular_color);
    m_light->setType(m_light_type);

    this->update_done();
    this->request_render();
}

//------------------------------------------------------------------------------

void light::stopping()
{
    this->render_service()->make_current();

    this->unregister_services();

    Ogre::SceneManager* const scene_mgr = this->get_scene_manager();
    if(m_light_name != sight::viz::scene3d::layer::DEFAULT_LIGHT_NAME)
    {
        m_light_node->removeAndDestroyAllChildren();

        scene_mgr->destroyManualObject(m_light_position);
        scene_mgr->destroyManualObject(m_directional_feedback.first);
        scene_mgr->destroyManualObject(m_directional_feedback.second);

        m_light_position              = nullptr;
        m_directional_feedback.first  = nullptr;
        m_directional_feedback.second = nullptr;
        m_material.reset();
    }

    scene_mgr->destroyLight(m_light);
    scene_mgr->destroySceneNode(m_light_node);

    m_light      = nullptr;
    m_light_node = nullptr;

    adaptor::deinit();
}

//------------------------------------------------------------------------------

void light::set_diffuse_color(Ogre::ColourValue _diffuse_color)
{
    const auto light_diffuse_color = m_diffuse.lock();

    light_diffuse_color->set_rgba(_diffuse_color.r, _diffuse_color.g, _diffuse_color.b, _diffuse_color.a);

    m_light->setDiffuseColour(_diffuse_color);
    this->request_render();
}

//------------------------------------------------------------------------------

void light::set_specular_color(Ogre::ColourValue _specular_color)
{
    const auto light_specular_color = m_specular.lock();

    light_specular_color->set_rgba(_specular_color.r, _specular_color.g, _specular_color.b, _specular_color.a);

    m_light->setSpecularColour(_specular_color);
    this->request_render();
}

//------------------------------------------------------------------------------

void light::switch_on(bool _on)
{
    m_switched_on = _on;

    if(m_light != nullptr)
    {
        m_light->setVisible(m_switched_on);
        this->request_render();
    }
}

//------------------------------------------------------------------------------

void light::set_theta_offset(float _theta_offset)
{
    this->render_service()->make_current();

    const float theta_delta = _theta_offset - m_theta_offset;
    m_theta_offset = _theta_offset;

    Ogre::Radian theta_offset_rad_delta(Ogre::Degree(static_cast<Ogre::Real>(theta_delta)));
    Ogre::Vector3 y_axis = m_light_node->getOrientation().yAxis();

    m_light_node->rotate(y_axis, theta_offset_rad_delta, Ogre::Node::TS_WORLD);
    this->request_render();
}

//------------------------------------------------------------------------------

void light::set_phi_offset(float _phi_offset)
{
    this->render_service()->make_current();

    const float phi_delta = _phi_offset - m_phi_offset;
    m_phi_offset = _phi_offset;

    Ogre::Radian phi_offset_rad_delta(Ogre::Degree(static_cast<Ogre::Real>(phi_delta)));
    Ogre::Vector3 x_axis = m_light_node->getOrientation().xAxis();

    m_light_node->rotate(x_axis, phi_offset_rad_delta, Ogre::Node::TS_WORLD);
    this->request_render();
}

//------------------------------------------------------------------------------

void light::enable_visual_feedback(bool _enable)
{
    m_visual_feedback = _enable;
    if(m_light_position != nullptr)
    {
        m_light_position->setVisible(m_visual_feedback);
        m_directional_feedback.first->setVisible(m_visual_feedback && m_light_type == Ogre::Light::LT_DIRECTIONAL);
        m_directional_feedback.second->setVisible(m_visual_feedback && m_light_type == Ogre::Light::LT_DIRECTIONAL);
    }
}

//------------------------------------------------------------------------------

void light::set_type(Ogre::Light::LightTypes _type)
{
    m_light_type = _type;
    if(m_directional_feedback.first != nullptr)
    {
        m_directional_feedback.first->setVisible(m_visual_feedback && m_light_type == Ogre::Light::LT_DIRECTIONAL);
        m_directional_feedback.second->setVisible(m_visual_feedback && m_light_type == Ogre::Light::LT_DIRECTIONAL);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
