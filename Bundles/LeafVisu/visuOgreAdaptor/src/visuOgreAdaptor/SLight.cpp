/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SLight.hpp"

#include <arData/Camera.hpp>

#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwRenderOgre/SRender.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <fwServices/macros.hpp>

#include <OGRE/OgreLight.h>
#include <OGRE/OgreMatrix4.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreVector3.h>

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SLight, ::fwData::TransformationMatrix3D);

namespace visuOgreAdaptor
{

//------------------------------------------------------------------------------

SLight::SLight() throw() :
    m_light    (nullptr),
    m_lightName("")
{
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
}

//------------------------------------------------------------------------------

void SLight::doStart() throw(fwTools::Failed)
{
    m_lightMat   = this->getInput< ::fwData::TransformationMatrix3D >("lightMat");
    m_lightColor = this->getInput< ::fwData::Color >("lightColor");

    ::Ogre::String lightName = this->getID() + m_lightName;
    m_light                  = this->getSceneManager()->createLight(lightName);
    m_light->setType(::Ogre::Light::LT_DIRECTIONAL);

    ::Ogre::SceneNode* lightNode =
        this->getSceneManager()->getRootSceneNode()->createChildSceneNode(lightName + "_node");
    lightNode->attachObject(m_light);

    doUpdate();
}

//------------------------------------------------------------------------------

void SLight::doUpdate() throw(fwTools::Failed)
{
    // Multithreaded lock
    ::fwData::mt::ObjectReadLock lock(m_lightMat);

    // Received input lign and column data from f4s transformation matrix
    ::Ogre::Matrix4 ogreMatrix;
    for (size_t lt = 0; lt < 4; lt++)
    {
        for (size_t ct = 0; ct < 4; ct++)
        {
            ogreMatrix[ct][lt] = static_cast<Ogre::Real>(m_lightMat->getCoefficient(ct, lt));
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

    ::Ogre::SceneNode* parent = m_light->getParentSceneNode();

    // Reset the light position
    parent->setPosition(0, 0, 0);
    parent->setOrientation(Ogre::Quaternion::IDENTITY);

    // Update the light position
    parent->rotate(orientation);
    parent->translate(position);

    ::Ogre::ColourValue currentColor(m_lightColor->red(),
                                     m_lightColor->green(),
                                     m_lightColor->blue(),
                                     m_lightColor->alpha());

    m_light->setDiffuseColour(currentColor);
    m_light->setSpecularColour(currentColor);

    this->requestRender();
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
