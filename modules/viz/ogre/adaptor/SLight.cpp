/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "modules/viz/ogre/adaptor/SLight.hpp"

#include "modules/viz/ogre/adaptor/SMaterial.hpp"

#include <core/com/Slots.hxx>

#include <services/macros.hpp>
#include <services/op/Add.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreMath.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>

#include <viz/ogre/helper/ManualObject.hpp>
#include <viz/ogre/registry/macros.hpp>
#include <viz/ogre/SRender.hpp>
#include <viz/ogre/Utils.hpp>

namespace sight::modules::viz::ogre::adaptor
{

static const core::com::Slots::SlotKeyType s_SET_X_OFFSET_SLOT = "setXOffset";
static const core::com::Slots::SlotKeyType s_SET_Y_OFFSET_SLOT = "setYOffset";

static const services::IService::KeyType s_TRANSFORM_INOUT      = "transform";
static const services::IService::KeyType s_DIFFUSE_COLOR_INOUT  = "diffuseColor";
static const services::IService::KeyType s_SPECULAR_COLOR_INOUT = "specularColor";

static const std::string s_NAME_CONFIG         = "name";
static const std::string s_SWITCHED_ON_CONFIG  = "switchedOn";
static const std::string s_THETA_OFFSET_CONFIG = "thetaOffset";
static const std::string s_PHI_OFFSET_CONFIG   = "phiOffset";

fwRenderOgreRegisterLightMacro(::sight::modules::viz::ogre::adaptor::SLight, ::sight::viz::ogre::ILight::REGISTRY_KEY)

//------------------------------------------------------------------------------

SLight::SLight() noexcept
{
    newSlot(s_SET_X_OFFSET_SLOT, &SLight::setThetaOffset, this);
    newSlot(s_SET_Y_OFFSET_SLOT, &SLight::setPhiOffset, this);
}

//------------------------------------------------------------------------------

SLight::SLight(::sight::viz::ogre::ILight::Key /*key*/)
{
    newSlot(s_SET_X_OFFSET_SLOT, &SLight::setThetaOffset, this);
    newSlot(s_SET_Y_OFFSET_SLOT, &SLight::setPhiOffset, this);
}

//------------------------------------------------------------------------------

SLight::~SLight() noexcept
{
}

//------------------------------------------------------------------------------

void SLight::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    m_lightName = config.get<std::string>("name");

    this->setTransformId(config.get<std::string>( ::sight::viz::ogre::ITransformable::s_TRANSFORM_CONFIG,
                                                  this->getID() + "_transform"));

    m_switchedOn  = config.get<bool>(s_SWITCHED_ON_CONFIG, m_switchedOn);
    m_thetaOffset = config.get<float>(s_THETA_OFFSET_CONFIG, m_thetaOffset);
    m_phiOffset   = config.get<float>(s_PHI_OFFSET_CONFIG, m_phiOffset);
}

//------------------------------------------------------------------------------

void SLight::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    ::Ogre::SceneManager* const sceneMgr = this->getSceneManager();
    m_light                              = sceneMgr->createLight(this->getID() + "_" + m_lightName);

    // Sets the default light direction to the camera's view direction,
    m_light->setDirection(::Ogre::Vector3::NEGATIVE_UNIT_Z);
    m_light->setType(m_lightType);
    m_light->setVisible(m_switchedOn);

    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* transNode     = this->getTransformNode(rootSceneNode);
    m_lightNode                      = transNode->createChildSceneNode(this->getID() + "_light");
    m_lightNode->attachObject(m_light);

    if(m_thetaOffset != 0.f || m_phiOffset != 0.f)
    {
        this->setThetaOffset(m_thetaOffset);
        this->setPhiOffset(m_phiOffset);
    }

    if(m_lightName != ::sight::viz::ogre::Layer::s_DEFAULT_LIGHT_NAME)
    {
        // Creates the visual feedback
        // Creates the material
        m_material = data::Material::New();

        const modules::viz::ogre::adaptor::SMaterial::sptr materialAdaptor =
            this->registerService< modules::viz::ogre::adaptor::SMaterial >(
                "::sight::modules::viz::ogre::adaptor::SMaterial");
        materialAdaptor->registerInOut(m_material, modules::viz::ogre::adaptor::SMaterial::s_MATERIAL_INOUT, true);
        materialAdaptor->setID(this->getID() + materialAdaptor->getID());
        materialAdaptor->setMaterialName(this->getID() + materialAdaptor->getID());
        materialAdaptor->setRenderService( this->getRenderService() );
        materialAdaptor->setLayerID(m_layerID);
        materialAdaptor->setMaterialTemplateName(::sight::viz::ogre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
        materialAdaptor->start();

        materialAdaptor->getMaterialFw()->setHasVertexColor(true);
        materialAdaptor->update();

        // Size, these value allow to display light with good enough ratio.
        const float originRadius   = m_length * 0.1f;
        const float cylinderLength = m_length - m_length/10;
        const float cylinderRadius = m_length/80;
        const float coneLength     = m_length - cylinderLength;
        const float coneRadius     = cylinderRadius*2;
        const unsigned sample      = 64;

        // Creates the commun sphere position
        m_lightPosition = sceneMgr->createManualObject(this->getID() + "_origin");
        ::sight::viz::ogre::helper::ManualObject::createSphere(m_lightPosition, materialAdaptor->getMaterialName(),
                                                               ::Ogre::ColourValue(0.98f, 0.96f, 0.62f, 1.0f),
                                                               originRadius,
                                                               sample);
        m_lightPosition->setVisible(m_visualFeedback);
        m_lightNode->attachObject(m_lightPosition);

        // Create the directional light feedback
        m_directionalFeedback.first  = sceneMgr->createManualObject(this->getID() + "_line");
        m_directionalFeedback.second = sceneMgr->createManualObject(this->getID() + "_cone");

        ::sight::viz::ogre::helper::ManualObject::createCylinder(m_directionalFeedback.first,
                                                                 materialAdaptor->getMaterialName(),
                                                                 ::Ogre::ColourValue(0.f, 0.f, 1.f, 1.0f),
                                                                 cylinderRadius,
                                                                 cylinderLength,
                                                                 sample);
        ::Ogre::SceneNode* lineNode = m_lightNode->createChildSceneNode(this->getID() + "_lineNode");
        lineNode->attachObject(m_directionalFeedback.first);
        lineNode->yaw(::Ogre::Degree(-90));

        ::sight::viz::ogre::helper::ManualObject::createCone(m_directionalFeedback.second,
                                                             materialAdaptor->getMaterialName(),
                                                             ::Ogre::ColourValue(0.f, 0.f, 1.f, 1.0f),
                                                             coneRadius,
                                                             coneLength,
                                                             sample);
        ::Ogre::SceneNode* coneNode = m_lightNode->createChildSceneNode(this->getID() + "_coneNode");

        coneNode->attachObject(m_directionalFeedback.second);
        coneNode->translate(0.f, 0.f, cylinderLength);
        coneNode->yaw(::Ogre::Degree(-90));

        m_directionalFeedback.first->setVisible(m_visualFeedback && m_lightType == ::Ogre::Light::LT_DIRECTIONAL);
        m_directionalFeedback.second->setVisible(m_visualFeedback && m_lightType == ::Ogre::Light::LT_DIRECTIONAL);
    }

    updating();
}

//-----------------------------------------------------------------------------

services::IService::KeyConnectionsMap SLight::getAutoConnections() const
{
    services::IService::KeyConnectionsMap connections;
    connections.push(s_TRANSFORM_INOUT, data::TransformationMatrix3D::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push(s_DIFFUSE_COLOR_INOUT, data::Color::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push(s_SPECULAR_COLOR_INOUT, data::Color::s_MODIFIED_SIG, s_UPDATE_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

void SLight::updating()
{
    const auto lightDiffuseColor  = this->getLockedInOut< data::Color >(s_DIFFUSE_COLOR_INOUT);
    const auto lightSpecularColor = this->getLockedInOut< data::Color >(s_SPECULAR_COLOR_INOUT);

    this->getRenderService()->makeCurrent();

    const ::Ogre::ColourValue diffuseColor(lightDiffuseColor->red(),
                                           lightDiffuseColor->green(),
                                           lightDiffuseColor->blue(),
                                           lightDiffuseColor->alpha());

    const ::Ogre::ColourValue specularColor(lightSpecularColor->red(),
                                            lightSpecularColor->green(),
                                            lightSpecularColor->blue(),
                                            lightSpecularColor->alpha());

    m_light->setDiffuseColour(diffuseColor);
    m_light->setSpecularColour(specularColor);
    m_light->setType(m_lightType);

    this->requestRender();
}

//------------------------------------------------------------------------------

void SLight::stopping()
{
    this->getRenderService()->makeCurrent();

    this->unregisterServices();

    ::Ogre::SceneManager* const sceneMgr = this->getSceneManager();
    if(m_lightName != ::sight::viz::ogre::Layer::s_DEFAULT_LIGHT_NAME)
    {
        m_lightNode->removeAndDestroyChild(this->getID() + "_lineNode");
        m_lightNode->removeAndDestroyChild(this->getID() + "_coneNode");

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

void SLight::setDiffuseColor(::Ogre::ColourValue _diffuseColor)
{
    const auto lightDiffuseColor = this->getLockedInOut< data::Color >(s_DIFFUSE_COLOR_INOUT);

    lightDiffuseColor->setRGBA(_diffuseColor.r, _diffuseColor.g, _diffuseColor.b, _diffuseColor.a);

    m_light->setDiffuseColour(_diffuseColor);
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLight::setSpecularColor(::Ogre::ColourValue _specularColor)
{
    const auto lightSpecularColor = this->getLockedInOut< data::Color >(s_SPECULAR_COLOR_INOUT);

    lightSpecularColor->setRGBA(_specularColor.r, _specularColor.g, _specularColor.b, _specularColor.a);

    m_light->setSpecularColour(_specularColor);
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLight::switchOn(bool _on)
{
    m_switchedOn = _on;

    if(m_light)
    {
        m_light->setVisible(m_switchedOn);
        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void SLight::setThetaOffset(float _thetaOffset)
{
    this->getRenderService()->makeCurrent();

    const float thetaDelta = _thetaOffset - m_thetaOffset;
    m_thetaOffset = _thetaOffset;

    ::Ogre::Radian thetaOffsetRadDelta(::Ogre::Degree(static_cast< ::Ogre::Real>(thetaDelta)));
    ::Ogre::Vector3 yAxis = m_lightNode->getOrientation().yAxis();

    m_lightNode->rotate(yAxis, thetaOffsetRadDelta, ::Ogre::Node::TS_WORLD);
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLight::setPhiOffset(float _phiOffset)
{
    this->getRenderService()->makeCurrent();

    const float phiDelta = _phiOffset - m_phiOffset;
    m_phiOffset = _phiOffset;

    ::Ogre::Radian phiOffsetRadDelta(::Ogre::Degree(static_cast< ::Ogre::Real>(phiDelta)));
    ::Ogre::Vector3 xAxis = m_lightNode->getOrientation().xAxis();

    m_lightNode->rotate(xAxis, phiOffsetRadDelta, ::Ogre::Node::TS_WORLD);
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLight::enableVisualFeedback(bool _enable)
{
    m_visualFeedback = _enable;
    if(m_lightPosition)
    {
        m_lightPosition->setVisible(m_visualFeedback);
        m_directionalFeedback.first->setVisible(m_visualFeedback && m_lightType == ::Ogre::Light::LT_DIRECTIONAL);
        m_directionalFeedback.second->setVisible(m_visualFeedback && m_lightType == ::Ogre::Light::LT_DIRECTIONAL);
    }
}

//------------------------------------------------------------------------------

void SLight::setType(::Ogre::Light::LightTypes _type)
{
    m_lightType = _type;
    if(m_directionalFeedback.first)
    {
        m_directionalFeedback.first->setVisible(m_visualFeedback && m_lightType == ::Ogre::Light::LT_DIRECTIONAL);
        m_directionalFeedback.second->setVisible(m_visualFeedback && m_lightType == ::Ogre::Light::LT_DIRECTIONAL);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::modules::viz::ogre::adaptor.
