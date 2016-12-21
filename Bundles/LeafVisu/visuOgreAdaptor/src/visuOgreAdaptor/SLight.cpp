/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SLight.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwRenderOgre/SRender.hpp>
#include <fwRenderOgre/Utils.hpp>
#include <fwRenderOgre/registry/macros.hpp>

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

SLight::SLight() throw() :
    m_light        (nullptr),
    m_lightName    (""),
    m_lightType    (::Ogre::Light::LT_DIRECTIONAL),
    m_useOrphanNode(true),
    m_switchedOn   (true),
    m_thetaOffset  (0.f),
    m_phiOffset    (0.f)
{
    newSlot(s_SET_X_OFFSET_SLOT, &SLight::setThetaOffset, this);
    newSlot(s_SET_Y_OFFSET_SLOT, &SLight::setPhiOffset, this);
}

//------------------------------------------------------------------------------

SLight::SLight(::fwRenderOgre::ILight::Key key) :
    m_light        (nullptr),
    m_lightName    (""),
    m_useOrphanNode(true),
    m_switchedOn   (true),
    m_thetaOffset  (0.f),
    m_phiOffset    (0.f)
{
    newSlot(s_SET_X_OFFSET_SLOT, &SLight::setThetaOffset, this);
    newSlot(s_SET_Y_OFFSET_SLOT, &SLight::setPhiOffset, this);
}

//------------------------------------------------------------------------------

SLight::~SLight() throw()
{
}

//------------------------------------------------------------------------------

void SLight::doConfigure() throw(fwTools::Failed)
{
    SLM_ASSERT("No config tag", m_configuration->getName() == "config");

    if(m_configuration->hasAttribute("name"))
    {
        m_lightName = m_configuration->getAttributeValue("name");
    }

    if(m_configuration->hasAttribute("switchedOn"))
    {
        std::string switchedOnStr = m_configuration->getAttributeValue("switchedOn");
        m_switchedOn = (switchedOnStr == "yes");
    }

    if(m_configuration->hasAttribute("parentTransformId"))
    {
        this->setParentTransformId(m_configuration->getAttributeValue("parentTransformId"));

        if(m_configuration->hasAttribute("thetaOffset"))
        {
            std::string thetaOffsetStr = m_configuration->getAttributeValue("thetaOffset");
            m_thetaOffset = std::stof(thetaOffsetStr);
        }

        if(m_configuration->hasAttribute("phiOffset"))
        {
            std::string phiOffsetStr = m_configuration->getAttributeValue("phiOffset");
            m_phiOffset = std::stof(phiOffsetStr);
        }
    }
}

//------------------------------------------------------------------------------

void SLight::doStart() throw(fwTools::Failed)
{
    m_lightDiffuseColor  = this->getInOut< ::fwData::Color >("diffuseColor");
    m_lightSpecularColor = this->getInOut< ::fwData::Color >("specularColor");

    m_lightName = this->getID() + "_" + m_lightName;
    m_light     = this->getSceneManager()->createLight(m_lightName);
    m_light->setType(::Ogre::Light::LT_DIRECTIONAL);
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

    doUpdate();
}

//------------------------------------------------------------------------------

void SLight::doUpdate() throw(fwTools::Failed)
{
    SLM_ASSERT("Missing color data objects.", m_lightDiffuseColor && m_lightSpecularColor);

    ::Ogre::ColourValue diffuseColor(m_lightDiffuseColor->red(),
                                     m_lightDiffuseColor->green(),
                                     m_lightDiffuseColor->blue(),
                                     m_lightDiffuseColor->alpha());
    ::Ogre::ColourValue specularColor(m_lightSpecularColor->red(),
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

    m_thetaOffset = _thetaOffset;

    ::Ogre::Radian thetaOffsetRad(::Ogre::Degree(static_cast< ::Ogre::Real>(m_thetaOffset)));
    ::Ogre::Vector3 xAxis = this->getLayer()->getDefaultCamera()->getRight();

    m_light->getParentSceneNode()->rotate(xAxis, thetaOffsetRad, ::Ogre::Node::TS_WORLD);
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLight::setPhiOffset(float _phiOffset)
{
    SLM_ASSERT("Unable to update an offset if the light's node isn't attached to a parent node", !m_useOrphanNode);

    m_phiOffset = _phiOffset;

    ::Ogre::Radian phiOffsetRad(::Ogre::Degree(static_cast< ::Ogre::Real>(m_phiOffset)));
    ::Ogre::Vector3 yAxis = this->getLayer()->getDefaultCamera()->getUp();

    m_light->getParentSceneNode()->rotate(yAxis, phiOffsetRad, ::Ogre::Node::TS_WORLD);
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
    transformService->setRenderService ( this->getRenderService() );
    transformService->setLayerID(m_layerID);
    transformService->setTransformId(this->getTransformId());
    transformService->setParentTransformId(this->getParentTransformId());

    transformService->start();
    transformService->connect();
    this->registerService(transformService);

    this->attachNode(m_light);
}

//-----------------------------------------------------------------------------

void SLight::attachNode(Ogre::MovableObject* _node)
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

void SLight::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void SLight::doStop() throw(fwTools::Failed)
{
    if(!m_transformService.expired())
    {
        m_transformService.lock()->stop();
        ::fwServices::OSR::unregisterService(m_transformService.lock());
    }

    this->unregisterServices();
}

//------------------------------------------------------------------------------

} //namespace visuOgreAdaptor
