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

#include <viz/scene3d/helper/ManualObject.hpp>
#include <viz/scene3d/registry/macros.hpp>
#include <viz/scene3d/render.hpp>
#include <viz/scene3d/Utils.hpp>

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
    sight::viz::scene3d::ILight::REGISTRY_KEY
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

    Ogre::SceneManager* const sceneMgr = this->getSceneManager();
    m_light = sceneMgr->createLight(this->get_id() + "_" + m_lightName);

    // Sets the default light direction to the camera's view direction,
    m_light->setType(m_lightType);
    m_light->setVisible(m_switchedOn);

    Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    Ogre::SceneNode* transNode     = this->getOrCreateTransformNode(rootSceneNode);
    m_lightNode = transNode->createChildSceneNode(this->get_id() + "_light");
    m_lightNode->attachObject(m_light);

    if(m_thetaOffset != 0.F || m_phiOffset != 0.F)
    {
        this->setThetaOffset(m_thetaOffset);
        this->setPhiOffset(m_phiOffset);
    }

    if(m_lightName != sight::viz::scene3d::Layer::s_DEFAULT_LIGHT_NAME)
    {
        // Creates the visual feedback
        // Creates the material
        m_material = std::make_shared<data::material>();

        const module::viz::scene3d::adaptor::material::sptr materialAdaptor =
            this->registerService<module::viz::scene3d::adaptor::material>(
                "sight::module::viz::scene3d::adaptor::material"
            );
        materialAdaptor->set_inout(m_material, module::viz::scene3d::adaptor::material::s_MATERIAL_INOUT, true);
        materialAdaptor->configure(
            this->get_id() + materialAdaptor->get_id(),
            this->get_id() + materialAdaptor->get_id(),
            this->getRenderService(),
            m_layerID
        );
        materialAdaptor->start();

        materialAdaptor->getMaterialFw()->setHasVertexColor(true);
        materialAdaptor->update();

        // Size, these value allow to display light with good enough ratio.
        const float originRadius   = m_length * 0.1F;
        const float cylinderLength = m_length - m_length / 10;
        const float cylinderRadius = m_length / 80;
        const float coneLength     = m_length - cylinderLength;
        const float coneRadius     = cylinderRadius * 2;
        const unsigned sample      = 64;

        // Creates the commun sphere position
        m_lightPosition = sceneMgr->createManualObject(this->get_id() + "_origin");
        sight::viz::scene3d::helper::ManualObject::createSphere(
            m_lightPosition,
            materialAdaptor->getMaterialName(),
            Ogre::ColourValue(0.98F, 0.96F, 0.62F, 1.0F),
            originRadius,
            sample
        );
        m_lightPosition->setVisible(m_visualFeedback);
        m_lightNode->attachObject(m_lightPosition);

        // Create the directional light feedback
        m_directionalFeedback.first  = sceneMgr->createManualObject(this->get_id() + "_line");
        m_directionalFeedback.second = sceneMgr->createManualObject(this->get_id() + "_cone");

        sight::viz::scene3d::helper::ManualObject::createCylinder(
            m_directionalFeedback.first,
            materialAdaptor->getMaterialName(),
            Ogre::ColourValue(0.F, 0.F, 1.F, 1.0F),
            cylinderRadius,
            cylinderLength,
            sample
        );
        Ogre::SceneNode* lineNode = m_lightNode->createChildSceneNode(this->get_id() + "_lineNode");
        lineNode->attachObject(m_directionalFeedback.first);
        lineNode->yaw(Ogre::Degree(-90));

        sight::viz::scene3d::helper::ManualObject::createCone(
            m_directionalFeedback.second,
            materialAdaptor->getMaterialName(),
            Ogre::ColourValue(0.F, 0.F, 1.F, 1.0F),
            coneRadius,
            coneLength,
            sample
        );
        Ogre::SceneNode* coneNode = m_lightNode->createChildSceneNode(this->get_id() + "_coneNode");

        coneNode->attachObject(m_directionalFeedback.second);
        coneNode->translate(0.F, 0.F, cylinderLength);
        coneNode->yaw(Ogre::Degree(-90));

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
    const auto lightDiffuseColor  = m_diffuse.lock();
    const auto lightSpecularColor = m_specular.lock();

    this->getRenderService()->makeCurrent();

    const Ogre::ColourValue diffuseColor(lightDiffuseColor->red(),
                                         lightDiffuseColor->green(),
                                         lightDiffuseColor->blue(),
                                         lightDiffuseColor->alpha());

    const Ogre::ColourValue specularColor(lightSpecularColor->red(),
                                          lightSpecularColor->green(),
                                          lightSpecularColor->blue(),
                                          lightSpecularColor->alpha());

    m_light->setDiffuseColour(diffuseColor);
    m_light->setSpecularColour(specularColor);
    m_light->setType(m_lightType);

    this->requestRender();
}

//------------------------------------------------------------------------------

void light::stopping()
{
    this->getRenderService()->makeCurrent();

    this->unregisterServices();

    Ogre::SceneManager* const sceneMgr = this->getSceneManager();
    if(m_lightName != sight::viz::scene3d::Layer::s_DEFAULT_LIGHT_NAME)
    {
        m_lightNode->removeAndDestroyChild(this->get_id() + "_lineNode");
        m_lightNode->removeAndDestroyChild(this->get_id() + "_coneNode");

        sceneMgr->destroyManualObject(m_lightPosition);
        sceneMgr->destroyManualObject(m_directionalFeedback.first);
        sceneMgr->destroyManualObject(m_directionalFeedback.second);

        m_lightPosition              = nullptr;
        m_directionalFeedback.first  = nullptr;
        m_directionalFeedback.second = nullptr;
        m_material.reset();
    }

    sceneMgr->destroyLight(m_light);
    sceneMgr->destroySceneNode(m_lightNode);

    m_light     = nullptr;
    m_lightNode = nullptr;
}

//------------------------------------------------------------------------------

void light::setDiffuseColor(Ogre::ColourValue _diffuseColor)
{
    const auto lightDiffuseColor = m_diffuse.lock();

    lightDiffuseColor->setRGBA(_diffuseColor.r, _diffuseColor.g, _diffuseColor.b, _diffuseColor.a);

    m_light->setDiffuseColour(_diffuseColor);
    this->requestRender();
}

//------------------------------------------------------------------------------

void light::setSpecularColor(Ogre::ColourValue _specularColor)
{
    const auto lightSpecularColor = m_specular.lock();

    lightSpecularColor->setRGBA(_specularColor.r, _specularColor.g, _specularColor.b, _specularColor.a);

    m_light->setSpecularColour(_specularColor);
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

void light::setThetaOffset(float _thetaOffset)
{
    this->getRenderService()->makeCurrent();

    const float thetaDelta = _thetaOffset - m_thetaOffset;
    m_thetaOffset = _thetaOffset;

    Ogre::Radian thetaOffsetRadDelta(Ogre::Degree(static_cast<Ogre::Real>(thetaDelta)));
    Ogre::Vector3 yAxis = m_lightNode->getOrientation().yAxis();

    m_lightNode->rotate(yAxis, thetaOffsetRadDelta, Ogre::Node::TS_WORLD);
    this->requestRender();
}

//------------------------------------------------------------------------------

void light::setPhiOffset(float _phiOffset)
{
    this->getRenderService()->makeCurrent();

    const float phiDelta = _phiOffset - m_phiOffset;
    m_phiOffset = _phiOffset;

    Ogre::Radian phiOffsetRadDelta(Ogre::Degree(static_cast<Ogre::Real>(phiDelta)));
    Ogre::Vector3 xAxis = m_lightNode->getOrientation().xAxis();

    m_lightNode->rotate(xAxis, phiOffsetRadDelta, Ogre::Node::TS_WORLD);
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
