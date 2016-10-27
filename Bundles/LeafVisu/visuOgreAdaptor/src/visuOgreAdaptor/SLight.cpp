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

#include <fwServices/macros.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreLight.h>
#include <OGRE/OgreMath.h>
#include <OGRE/OgreMatrix4.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreVector3.h>

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SLight, ::fwData::TransformationMatrix3D);

namespace visuOgreAdaptor
{

//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SLight::s_UPDATE_X_OFFSET_SLOT      = "updateXOffset";
const ::fwCom::Slots::SlotKeyType SLight::s_UPDATE_Y_OFFSET_SLOT      = "updateYOffset";
const ::fwCom::Slots::SlotKeyType SLight::s_SET_DOUBLE_PARAMETER_SLOT = "setDoubleParameter";

//------------------------------------------------------------------------------

SLight::SLight() throw() :
    m_light           (nullptr),
    m_lightName       (""),
    m_attachedToCamera(false),
    m_xOffset         (0.f),
    m_yOffset         (0.f)
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

    if(m_configuration->hasAttribute("attachToCamera"))
    {
        m_attachedToCamera = (m_configuration->getAttributeValue("attachToCamera") == "yes");

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
    m_lightColor = this->getInput< ::fwData::Color >("lightColor");

    ::Ogre::String lightName = this->getID() + m_lightName;
    m_light                  = this->getSceneManager()->createLight(lightName);
    m_light->setType(::Ogre::Light::LT_DIRECTIONAL);

    ::Ogre::SceneNode* lightSceneNode;

    if(m_attachedToCamera)
    {
        lightSceneNode = this->getSceneManager()->getCamera("PlayerCam")->getParentSceneNode()->createChildSceneNode(
            lightName + "_node");
        lightSceneNode->attachObject(m_light);

        if(m_xOffset != 0.f || m_yOffset != 0.f)
        {
            this->updateXOffset(m_xOffset);
            this->updateYOffset(m_yOffset);
        }
    }
    else
    {
        lightSceneNode = this->getSceneManager()->getRootSceneNode()->createChildSceneNode(lightName + "_node");
        lightSceneNode->attachObject(m_light);

        // We use our own transform matrix if the light isn't attached to a camera
        m_lightMat = this->getInput< ::fwData::TransformationMatrix3D >("lightMat");
    }

    doUpdate();
}

//------------------------------------------------------------------------------

void SLight::doUpdate() throw(fwTools::Failed)
{
    ::Ogre::SceneNode* lightSceneNode = m_light->getParentSceneNode();

    // Updates light's position from associated f4s matrix only if the light is independent from the camera
    if(!m_attachedToCamera)
    {
        // Multithreaded lock
        ::fwData::mt::ObjectReadLock lock(m_lightMat);

        // Received input lign and column data from f4s transformation matrix
        ::Ogre::Matrix4 ogreMatrix;
        for (size_t lt = 0; lt < 4; lt++)
        {
            for (size_t ct = 0; ct < 4; ct++)
            {
                ogreMatrix[ct][lt] = static_cast< ::Ogre::Real>(m_lightMat->getCoefficient(ct, lt));
            }
        }

        lock.unlock();

        // Decompose the camera matrix
        ::Ogre::Vector3 position;
        ::Ogre::Vector3 scale;
        ::Ogre::Quaternion orientation;
        ogreMatrix.decomposition(position, scale, orientation);

        ::Ogre::Quaternion rotateY(::Ogre::Degree(180), ::Ogre::Vector3(0,1,0));
        ::Ogre::Quaternion rotateZ(::Ogre::Degree(180), ::Ogre::Vector3(0,0,1));
        orientation = orientation * rotateZ * rotateY;

        // Reset the light position
        lightSceneNode->setPosition(0, 0, 0);
        lightSceneNode->setOrientation(Ogre::Quaternion::IDENTITY);

        // Update the light position
        lightSceneNode->rotate(orientation);
        lightSceneNode->translate(position);
    }

    ::Ogre::ColourValue currentColor(m_lightColor->red(),
                                     m_lightColor->green(),
                                     m_lightColor->blue(),
                                     m_lightColor->alpha());

    m_light->setDiffuseColour(currentColor);
    m_light->setSpecularColour(currentColor);

    this->requestRender();
}

//------------------------------------------------------------------------------

void SLight::updateXOffset(float _xOffset)
{
    SLM_ASSERT("The light has to be attached to the camera before updating an offset", m_attachedToCamera);

    m_xOffset = _xOffset;

    ::Ogre::Radian xOffsetRad(::Ogre::Degree(static_cast< ::Ogre::Real>(m_xOffset)));
    ::Ogre::Vector3 xAxis = this->getSceneManager()->getCamera("PlayerCam")->getRight();

    m_light->getParentSceneNode()->rotate(xAxis, xOffsetRad, ::Ogre::Node::TS_WORLD);
}

//------------------------------------------------------------------------------

void SLight::updateYOffset(float _yOffset)
{
    SLM_ASSERT("The light has to be attached to the camera before updating an offset", m_attachedToCamera);

    m_yOffset = _yOffset;

    ::Ogre::Radian yOffsetRad(::Ogre::Degree(static_cast< ::Ogre::Real>(m_yOffset)));
    ::Ogre::Vector3 yAxis = this->getSceneManager()->getCamera("PlayerCam")->getUp();

    m_light->getParentSceneNode()->rotate(yAxis, yOffsetRad, ::Ogre::Node::TS_WORLD);
}

//------------------------------------------------------------------------------

void SLight::setDoubleParameter(double val, std::string key)
{
    if(key == "xOffset")
    {
        this->updateXOffset(static_cast<float>(val));
    }
    else if(key == "yOffset")
    {
        this->updateYOffset(static_cast<float>(val));
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
}

//------------------------------------------------------------------------------

} //namespace visuOgreAdaptor
