/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "modules/viz/scene3d/adaptor/material.hpp"

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

static const service::base::key_t s_DIFFUSE_COLOR_INOUT  = "diffuseColor";
static const service::base::key_t s_SPECULAR_COLOR_INOUT = "specularColor";

SIGHT_REGISTER_SCENE3D_LIGHT(
    sight::module::viz::scene3d::adaptor::light,
    sight::viz::scene3d::light_adaptor::REGISTRY_KEY
)

//------------------------------------------------------------------------------

light::light() noexcept
{
    new_slot(SET_X_OFFSET_SLOT, &light::setThetaOffset, this);
    new_slot(SET_Y_OFFSET_SLOT, &light::setPhiOffset, this);
}

//------------------------------------------------------------------------------

void light::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

    this->setTransformId(
        config.get<std::string>(
            sight::viz::scene3d::transformable::s_TRANSFORM_CONFIG,
            this->get_id() + "_transform"
        )
    );

    static const std::string s_NAME_CONFIG         = s_CONFIG + "name";
    static const std::string s_SWITCHED_ON_CONFIG  = s_CONFIG + "switchedOn";
    static const std::string s_THETA_OFFSET_CONFIG = s_CONFIG + "thetaOffset";
    static const std::string s_PHI_OFFSET_CONFIG   = s_CONFIG + "phiOffset";

    m_lightName   = config.get<std::string>(s_NAME_CONFIG);
    m_switchedOn  = config.get<bool>(s_SWITCHED_ON_CONFIG, m_switchedOn);
    m_thetaOffset = config.get<float>(s_THETA_OFFSET_CONFIG, m_thetaOffset);
    m_phiOffset   = config.get<float>(s_PHI_OFFSET_CONFIG, m_phiOffset);
}

//------------------------------------------------------------------------------

void light::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    Ogre::SceneManager* const scene_mgr = this->getSceneManager();
    m_light = scene_mgr->createLight(this->get_id() + "_" + m_lightName);

    // Sets the default light direction to the camera's view direction,
    m_light->setType(m_lightType);
    m_light->setVisible(m_switchedOn);

    Ogre::SceneNode* root_scene_node = this->getSceneManager()->getRootSceneNode();
    Ogre::SceneNode* trans_node      = this->getOrCreateTransformNode(root_scene_node);
    m_lightNode = trans_node->createChildSceneNode(this->get_id() + "_light");
    m_lightNode->attachObject(m_light);

    if(m_thetaOffset != 0.F || m_phiOffset != 0.F)
    {
        this->setThetaOffset(m_thetaOffset);
        this->setPhiOffset(m_phiOffset);
    }

    if(m_lightName != sight::viz::scene3d::layer::s_DEFAULT_LIGHT_NAME)
    {
        // Creates the visual feedback
        // Creates the material
        m_material = std::make_shared<data::material>();

        const module::viz::scene3d::adaptor::material::sptr material_adaptor =
            this->registerService<module::viz::scene3d::adaptor::material>(
                "sight::module::viz::scene3d::adaptor::material"
            );
        material_adaptor->set_inout(m_material, module::viz::scene3d::adaptor::material::s_MATERIAL_INOUT, true);
        material_adaptor->configure(
            this->get_id() + material_adaptor->get_id(),
            this->get_id() + material_adaptor->get_id(),
            this->getRenderService(),
            m_layerID
        );
        material_adaptor->start();

        material_adaptor->getMaterialFw()->setHasVertexColor(true);
        material_adaptor->update();

        // Size, these value allow to display light with good enough ratio.
        const float origin_radius   = m_length * 0.1F;
        const float cylinder_length = m_length - m_length / 10;
        const float cylinder_radius = m_length / 80;
        const float cone_length     = m_length - cylinder_length;
        const float cone_radius     = cylinder_radius * 2;
        const unsigned sample       = 64;

        // Creates the commun sphere position
        m_lightPosition = scene_mgr->createManualObject(this->get_id() + "_origin");
        sight::viz::scene3d::helper::manual_object::createSphere(
            m_lightPosition,
            material_adaptor->getMaterialName(),
            Ogre::ColourValue(0.98F, 0.96F, 0.62F, 1.0F),
            origin_radius,
            sample
        );
        m_lightPosition->setVisible(m_visualFeedback);
        m_lightNode->attachObject(m_lightPosition);

        // Create the directional light feedback
        m_directionalFeedback.first  = scene_mgr->createManualObject(this->get_id() + "_line");
        m_directionalFeedback.second = scene_mgr->createManualObject(this->get_id() + "_cone");

        sight::viz::scene3d::helper::manual_object::createCylinder(
            m_directionalFeedback.first,
            material_adaptor->getMaterialName(),
            Ogre::ColourValue(0.F, 0.F, 1.F, 1.0F),
            cylinder_radius,
            cylinder_length,
            sample
        );
        Ogre::SceneNode* line_node = m_lightNode->createChildSceneNode(this->get_id() + "_lineNode");
        line_node->attachObject(m_directionalFeedback.first);
        line_node->yaw(Ogre::Degree(-90));

        sight::viz::scene3d::helper::manual_object::createCone(
            m_directionalFeedback.second,
            material_adaptor->getMaterialName(),
            Ogre::ColourValue(0.F, 0.F, 1.F, 1.0F),
            cone_radius,
            cone_length,
            sample
        );
        Ogre::SceneNode* cone_node = m_lightNode->createChildSceneNode(this->get_id() + "_coneNode");

        cone_node->attachObject(m_directionalFeedback.second);
        cone_node->translate(0.F, 0.F, cylinder_length);
        cone_node->yaw(Ogre::Degree(-90));

        m_directionalFeedback.first->setVisible(m_visualFeedback && m_lightType == Ogre::Light::LT_DIRECTIONAL);
        m_directionalFeedback.second->setVisible(m_visualFeedback && m_lightType == Ogre::Light::LT_DIRECTIONAL);
    }

    updating();
}

//-----------------------------------------------------------------------------

service::connections_t light::auto_connections() const
{
    service::connections_t connections;
    connections.push(s_DIFFUSE_COLOR_INOUT, data::color::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_SPECULAR_COLOR_INOUT, data::color::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void light::updating()
{
    const auto light_diffuse_color  = m_diffuse.lock();
    const auto light_specular_color = m_specular.lock();

    this->getRenderService()->makeCurrent();

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
    m_light->setType(m_lightType);

    this->requestRender();
}

//------------------------------------------------------------------------------

void light::stopping()
{
    this->getRenderService()->makeCurrent();

    this->unregisterServices();

    Ogre::SceneManager* const scene_mgr = this->getSceneManager();
    if(m_lightName != sight::viz::scene3d::layer::s_DEFAULT_LIGHT_NAME)
    {
        m_lightNode->removeAndDestroyChild(this->get_id() + "_lineNode");
        m_lightNode->removeAndDestroyChild(this->get_id() + "_coneNode");

        scene_mgr->destroyManualObject(m_lightPosition);
        scene_mgr->destroyManualObject(m_directionalFeedback.first);
        scene_mgr->destroyManualObject(m_directionalFeedback.second);

        m_lightPosition              = nullptr;
        m_directionalFeedback.first  = nullptr;
        m_directionalFeedback.second = nullptr;
        m_material.reset();
    }

    scene_mgr->destroyLight(m_light);
    scene_mgr->destroySceneNode(m_lightNode);

    m_light     = nullptr;
    m_lightNode = nullptr;
}

//------------------------------------------------------------------------------

void light::setDiffuseColor(Ogre::ColourValue _diffuse_color)
{
    const auto light_diffuse_color = m_diffuse.lock();

    light_diffuse_color->setRGBA(_diffuse_color.r, _diffuse_color.g, _diffuse_color.b, _diffuse_color.a);

    m_light->setDiffuseColour(_diffuse_color);
    this->requestRender();
}

//------------------------------------------------------------------------------

void light::setSpecularColor(Ogre::ColourValue _specular_color)
{
    const auto light_specular_color = m_specular.lock();

    light_specular_color->setRGBA(_specular_color.r, _specular_color.g, _specular_color.b, _specular_color.a);

    m_light->setSpecularColour(_specular_color);
    this->requestRender();
}

//------------------------------------------------------------------------------

void light::switchOn(bool _on)
{
    m_switchedOn = _on;

    if(m_light != nullptr)
    {
        m_light->setVisible(m_switchedOn);
        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void light::setThetaOffset(float _theta_offset)
{
    this->getRenderService()->makeCurrent();

    const float theta_delta = _theta_offset - m_thetaOffset;
    m_thetaOffset = _theta_offset;

    Ogre::Radian theta_offset_rad_delta(Ogre::Degree(static_cast<Ogre::Real>(theta_delta)));
    Ogre::Vector3 y_axis = m_lightNode->getOrientation().yAxis();

    m_lightNode->rotate(y_axis, theta_offset_rad_delta, Ogre::Node::TS_WORLD);
    this->requestRender();
}

//------------------------------------------------------------------------------

void light::setPhiOffset(float _phi_offset)
{
    this->getRenderService()->makeCurrent();

    const float phi_delta = _phi_offset - m_phiOffset;
    m_phiOffset = _phi_offset;

    Ogre::Radian phi_offset_rad_delta(Ogre::Degree(static_cast<Ogre::Real>(phi_delta)));
    Ogre::Vector3 x_axis = m_lightNode->getOrientation().xAxis();

    m_lightNode->rotate(x_axis, phi_offset_rad_delta, Ogre::Node::TS_WORLD);
    this->requestRender();
}

//------------------------------------------------------------------------------

void light::enableVisualFeedback(bool _enable)
{
    m_visualFeedback = _enable;
    if(m_lightPosition != nullptr)
    {
        m_lightPosition->setVisible(m_visualFeedback);
        m_directionalFeedback.first->setVisible(m_visualFeedback && m_lightType == Ogre::Light::LT_DIRECTIONAL);
        m_directionalFeedback.second->setVisible(m_visualFeedback && m_lightType == Ogre::Light::LT_DIRECTIONAL);
    }
}

//------------------------------------------------------------------------------

void light::setType(Ogre::Light::LightTypes _type)
{
    m_lightType = _type;
    if(m_directionalFeedback.first != nullptr)
    {
        m_directionalFeedback.first->setVisible(m_visualFeedback && m_lightType == Ogre::Light::LT_DIRECTIONAL);
        m_directionalFeedback.second->setVisible(m_visualFeedback && m_lightType == Ogre::Light::LT_DIRECTIONAL);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
