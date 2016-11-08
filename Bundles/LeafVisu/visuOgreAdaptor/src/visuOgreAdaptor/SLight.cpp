/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SLight.hpp"

#include "visuOgreAdaptor/STransform.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/TransformationMatrix3D.hpp>
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

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SLight, ::fwData::TransformationMatrix3D);

fwRenderOgreRegisterLightMacro( ::visuOgreAdaptor::SLight,
                                ::fwRenderOgre::ILight::REGISTRY_KEY );

//------------------------------------------------------------------------------

namespace visuOgreAdaptor
{

//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SLight::s_UPDATE_X_OFFSET_SLOT      = "updateXOffset";
const ::fwCom::Slots::SlotKeyType SLight::s_UPDATE_Y_OFFSET_SLOT      = "updateYOffset";
const ::fwCom::Slots::SlotKeyType SLight::s_SET_DOUBLE_PARAMETER_SLOT = "setDoubleParameter";

//------------------------------------------------------------------------------

SLight::SLight() throw() :
    m_light         (nullptr),
    m_lightName     (""),
    m_lightType     (::Ogre::Light::LT_DIRECTIONAL),
    m_useOrphanNode (true),
    m_xOffset       (0.f),
    m_yOffset       (0.f)
{
    newSlot(s_UPDATE_X_OFFSET_SLOT, &SLight::updateXOffset, this);
    newSlot(s_UPDATE_Y_OFFSET_SLOT, &SLight::updateYOffset, this);
}

//------------------------------------------------------------------------------

SLight::SLight(::fwRenderOgre::ILight::Key key) :
    m_light        (nullptr),
    m_lightName    (""),
    m_useOrphanNode(true),
    m_xOffset      (0.f),
    m_yOffset      (0.f)
{
    newSlot(s_UPDATE_X_OFFSET_SLOT, &SLight::updateXOffset, this);
    newSlot(s_UPDATE_Y_OFFSET_SLOT, &SLight::updateYOffset, this);
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

    if(m_configuration->hasAttribute("parentTransformId"))
    {
        this->setParentTransformId(m_configuration->getAttributeValue("parentTransformId"));

        if(m_configuration->hasAttribute("xOffset"))
        {
            std::string xOffsetString = m_configuration->getAttributeValue("xOffset");
            m_xOffset = std::stof(xOffsetString);
        }

        if(m_configuration->hasAttribute("yOffset"))
        {
            std::string yOffsetString = m_configuration->getAttributeValue("yOffset");
            m_yOffset = std::stof(yOffsetString);
        }
    }
}

//------------------------------------------------------------------------------

void SLight::doStart() throw(fwTools::Failed)
{
    if(!m_lightDiffuseColor)
    {
        m_lightDiffuseColor = this->getInput< ::fwData::Color >("diffuseColor");
    }

    if(!m_lightSpecularColor)
    {
        m_lightSpecularColor = this->getInput< ::fwData::Color >("specularColor");
    }

    m_lightName = this->getID() + "_" + m_lightName;
    m_light     = this->getSceneManager()->createLight(m_lightName);
    m_light->setType(::Ogre::Light::LT_DIRECTIONAL);

    this->setTransformId(m_lightName + "_node");

    if(!this->getParentTransformId().empty())
    {
        auto tfAdaptors = this->getRenderService()->getAdaptors< ::visuOgreAdaptor::STransform>();

        auto result =
            std::find_if(tfAdaptors.begin(), tfAdaptors.end(),[this](const ::visuOgreAdaptor::STransform::sptr& srv)
            {
                return srv->getTransformId() == this->getParentTransformId();
            });

        // If the parent transform adaptor is retrieved
        if(result != tfAdaptors.end())
        {
            m_useOrphanNode = false;

            // First update of the offset only if there is a parent transform.
            if(m_xOffset != 0.f || m_yOffset != 0.f)
            {
                this->updateXOffset(m_xOffset);
                this->updateYOffset(m_yOffset);
            }
        }
    }

    this->createTransformService();

    doUpdate();
}

//------------------------------------------------------------------------------

void SLight::doUpdate() throw(fwTools::Failed)
{
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
    if(!m_lightDiffuseColor)
    {
        m_lightDiffuseColor = ::fwRenderOgre::Utils::convertOgreColorToFwColor(_diffuseColor);
    }
}

//------------------------------------------------------------------------------

void SLight::setSpecularColor(::Ogre::ColourValue _specularColor)
{
    if(!m_lightSpecularColor)
    {
        m_lightSpecularColor = ::fwRenderOgre::Utils::convertOgreColorToFwColor(_specularColor);
    }
}

//------------------------------------------------------------------------------

void SLight::updateXOffset(float _xOffset)
{
    SLM_ASSERT("Unable to update an offset if the light's node isn't attached to a parent node", m_useOrphanNode);

    m_xOffset = _xOffset;

    ::Ogre::Radian xOffsetRad(::Ogre::Degree(static_cast< ::Ogre::Real>(m_xOffset)));
    ::Ogre::Vector3 xAxis = this->getSceneManager()->getCamera("PlayerCam")->getRight();

    m_light->getParentSceneNode()->rotate(xAxis, xOffsetRad, ::Ogre::Node::TS_WORLD);
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLight::updateYOffset(float _yOffset)
{
    SLM_ASSERT("Unable to update an offset if the light's node isn't attached to a parent node", m_useOrphanNode);

    m_yOffset = _yOffset;

    ::Ogre::Radian yOffsetRad(::Ogre::Degree(static_cast< ::Ogre::Real>(m_yOffset)));
    ::Ogre::Vector3 yAxis = this->getSceneManager()->getCamera("PlayerCam")->getUp();

    m_light->getParentSceneNode()->rotate(yAxis, yOffsetRad, ::Ogre::Node::TS_WORLD);
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLight::setDoubleParameter(double _val, std::string _key)
{
    if(_key == "xOffset")
    {
        this->updateXOffset(static_cast<float>(_val));
    }
    else if(_key == "yOffset")
    {
        this->updateYOffset(static_cast<float>(_val));
    }
}

//------------------------------------------------------------------------------

void SLight::createTransformService()
{
    // We need to create a transform service only once
    if(m_transformService.expired())
    {
        ::fwData::TransformationMatrix3D::csptr transform =
            this->getInput< ::fwData::TransformationMatrix3D >("lightTF");

        if(!transform)
        {
            transform = ::fwData::TransformationMatrix3D::New();
        }

        m_transformService = ::fwServices::add< ::fwRenderOgre::IAdaptor >(transform,
                                                                           "::visuOgreAdaptor::STransform");
        SLM_ASSERT("Transform service is null", m_transformService.lock());
        auto transformService = ::visuOgreAdaptor::STransform::dynamicCast(m_transformService.lock());

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
}

//-----------------------------------------------------------------------------

void SLight::attachNode(Ogre::MovableObject* _node)
{
    auto transformService = ::visuOgreAdaptor::STransform::dynamicCast(m_transformService.lock());

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
