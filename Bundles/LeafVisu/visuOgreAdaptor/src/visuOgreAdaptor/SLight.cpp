/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SLight.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwRenderOgre/registry/macros.hpp>
#include <fwRenderOgre/SRender.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreMath.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>

//------------------------------------------------------------------------------

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SLight);

fwRenderOgreRegisterLightMacro( ::visuOgreAdaptor::SLight,
                                ::fwRenderOgre::ILight::REGISTRY_KEY );

//------------------------------------------------------------------------------

namespace visuOgreAdaptor
{

//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SLight::s_SET_X_OFFSET_SLOT         = "setXOffset";
const ::fwCom::Slots::SlotKeyType SLight::s_SET_Y_OFFSET_SLOT         = "setYOffset";
const ::fwCom::Slots::SlotKeyType SLight::s_SET_DOUBLE_PARAMETER_SLOT = "setDoubleParameter";

//------------------------------------------------------------------------------

SLight::SLight() noexcept :
    m_light(nullptr),
    m_lightName(""),
    m_lightType(::Ogre::Light::LT_DIRECTIONAL),
    m_useOrphanNode(true),
    m_switchedOn(true),
    m_thetaOffset(0.f),
    m_phiOffset(0.f)
{
    newSlot(s_SET_X_OFFSET_SLOT, &SLight::setThetaOffset, this);
    newSlot(s_SET_Y_OFFSET_SLOT, &SLight::setPhiOffset, this);
}

//------------------------------------------------------------------------------

SLight::SLight(::fwRenderOgre::ILight::Key key) :
    m_light(nullptr),
    m_lightName(""),
    m_useOrphanNode(true),
    m_switchedOn(true),
    m_thetaOffset(0.f),
    m_phiOffset(0.f)
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

    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");

    m_lightName = config.get<std::string>("name");

    if(config.count("switchedOn"))
    {
        m_switchedOn = config.get<std::string>("switchedOn") == "yes";
    }

    if(config.count("parentTransformId"))
    {
        this->setParentTransformId(config.get<std::string>("parentTransformId"));

        if(config.count("thetaOffset"))
        {
            m_thetaOffset = config.get<float>("thetaOffset");
        }

        if(config.count("phiOffset"))
        {
            m_phiOffset = config.get<float>("phiOffset");
        }
    }
}

//------------------------------------------------------------------------------

void SLight::starting()
{
    this->initialize();

    m_lightDiffuseColor  = this->getInOut< ::fwData::Color >("diffuseColor");
    m_lightSpecularColor = this->getInOut< ::fwData::Color >("specularColor");

    m_lightName = this->getID() + "_" + m_lightName;
    m_light     = this->getSceneManager()->createLight(m_lightName);
    m_light->setType(m_lightType ? m_lightType : ::Ogre::Light::LT_DIRECTIONAL);
    m_light->setVisible(m_switchedOn);

    if(!this->getParentTransformId().empty())
    {
        ::Ogre::SceneNode* parentSceneNode =
            this->getSceneManager()->getSceneNode(this->getParentTransformId());

        if(parentSceneNode)
        {
            m_useOrphanNode = false;

            // First update of the offset only if there is a parent transform.
            if(m_thetaOffset != 0.f || m_phiOffset != 0.f)
            {
                this->setThetaOffset(m_thetaOffset);
                this->setPhiOffset(m_phiOffset);
            }
        }
    }

    this->setTransformId(m_lightName + "_node");
    this->createTransformService();

    updating();
}

//------------------------------------------------------------------------------

void SLight::updating()
{
    SLM_ASSERT("Missing color data objects.", m_lightDiffuseColor && m_lightSpecularColor);

    const ::Ogre::ColourValue diffuseColor(m_lightDiffuseColor->red(),
                                           m_lightDiffuseColor->green(),
                                           m_lightDiffuseColor->blue(),
                                           m_lightDiffuseColor->alpha());
    const ::Ogre::ColourValue specularColor(m_lightSpecularColor->red(),
                                            m_lightSpecularColor->green(),
                                            m_lightSpecularColor->blue(),
                                            m_lightSpecularColor->alpha());

    m_light->setDiffuseColour(diffuseColor);
    m_light->setSpecularColour(specularColor);

    this->requestRender();
}

//------------------------------------------------------------------------------

void SLight::setDiffuseColor(::Ogre::ColourValue _diffuseColor)
{
    SLM_ASSERT("Missing diffuse color data object.", m_lightDiffuseColor);
    m_lightDiffuseColor->setRGBA(_diffuseColor.r, _diffuseColor.g, _diffuseColor.b, _diffuseColor.a);

    m_light->setDiffuseColour(_diffuseColor);
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLight::setSpecularColor(::Ogre::ColourValue _specularColor)
{
    SLM_ASSERT("Missing specular color data object.", m_lightSpecularColor);
    m_lightSpecularColor->setRGBA(_specularColor.r, _specularColor.g, _specularColor.b, _specularColor.a);

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
    SLM_ASSERT("Unable to update an offset if the light's node isn't attached to a parent node", !m_useOrphanNode);

    const float thetaDelta = _thetaOffset - m_thetaOffset;
    m_thetaOffset = _thetaOffset;

    ::Ogre::Radian thetaOffsetRadDelta(::Ogre::Degree(static_cast< ::Ogre::Real>(thetaDelta)));
    ::Ogre::Vector3 yAxis = m_light->getParentSceneNode()->getParentSceneNode()->getOrientation().yAxis();

    m_light->getParentSceneNode()->rotate(yAxis, thetaOffsetRadDelta, ::Ogre::Node::TS_WORLD);
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLight::setPhiOffset(float _phiOffset)
{
    SLM_ASSERT("Unable to update an offset if the light's node isn't attached to a parent node", !m_useOrphanNode);

    const float phiDelta = _phiOffset - m_phiOffset;
    m_phiOffset = _phiOffset;

    ::Ogre::Radian phiOffsetRadDelta(::Ogre::Degree(static_cast< ::Ogre::Real>(phiDelta)));
    ::Ogre::Vector3 xAxis = m_light->getParentSceneNode()->getParentSceneNode()->getOrientation().xAxis();

    m_light->getParentSceneNode()->rotate(xAxis, phiOffsetRadDelta, ::Ogre::Node::TS_WORLD);
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLight::setDoubleParameter(double _val, std::string _key)
{
    if(_key == "thetaOffset")
    {
        this->setThetaOffset(static_cast<float>(_val));
    }
    else if(_key == "phiOffset")
    {
        this->setPhiOffset(static_cast<float>(_val));
    }
}

//------------------------------------------------------------------------------

void SLight::createTransformService()
{
    ::fwData::TransformationMatrix3D::sptr transform =
        this->getInOut< ::fwData::TransformationMatrix3D >("transform");

    SLM_ASSERT("Missing tranform data object.", transform);

    m_transformService = ::fwServices::add< ::fwRenderOgre::IAdaptor >(transform,
                                                                       "::visuOgreAdaptor::STransform");
    SLM_ASSERT("Transform service is null", m_transformService.lock());
    auto transformService = this->getTransformService();

    transformService->setID(this->getID() + "_" + transformService->getID());
    transformService->setRenderService( this->getRenderService() );
    transformService->setLayerID(m_layerID);
    transformService->setTransformId(this->getTransformId());
    transformService->setParentTransformId(this->getParentTransformId());

    transformService->start();
    transformService->connect();
    this->registerService(transformService);

    this->attachNode(m_light);
}

//-----------------------------------------------------------------------------

void SLight::attachNode(::Ogre::MovableObject* _node)
{
    auto transformService = this->getTransformService();

    ::Ogre::SceneNode* transNode = transformService->getSceneNode();
    ::Ogre::SceneNode* node      = _node->getParentSceneNode();
    if ((node != transNode) && transNode)
    {
        _node->detachFromParent();
        transNode->attachObject(_node);
    }
}

//------------------------------------------------------------------------------

void SLight::swapping()
{
    this->updating();
}

//------------------------------------------------------------------------------

void SLight::stopping()
{
    if(!m_transformService.expired())
    {
        m_transformService.lock()->stop();
        ::fwServices::OSR::unregisterService(m_transformService.lock());
    }

    this->getSceneManager()->destroyLight(m_light);

    this->unregisterServices();
}

//------------------------------------------------------------------------------

} //namespace visuOgreAdaptor
