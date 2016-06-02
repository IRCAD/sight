/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SCamera.hpp"

#include <arData/Camera.hpp>

#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwRenderOgre/SRender.hpp>
#include <fwRenderOgre/Utils.hpp>
#include <fwServices/macros.hpp>

#include <OgreCamera.h>
#include <OgreMatrix4.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreVector3.h>

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SCamera, ::fwData::TransformationMatrix3D);

namespace visuOgreAdaptor
{

//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SCamera::s_CALIBRATE_SLOT = "calibrate";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_TF        = "updateTransformation";

//------------------------------------------------------------------------------

SCamera::SCamera() throw() :
    m_cameraUID("")
{
    newSlot(s_UPDATE_TF, &SCamera::updateTF3D, this);
    newSlot(s_CALIBRATE_SLOT, &SCamera::calibrate, this);
}

//------------------------------------------------------------------------------

SCamera::~SCamera() throw()
{
}

//------------------------------------------------------------------------------

void SCamera::doConfigure() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // Base class configuration
    SLM_ASSERT("Not a \"config\" configuration", m_configuration->getName() == "config");

    m_cameraUID = m_configuration->getAttributeValue("cameraUID");
}

//------------------------------------------------------------------------------

void SCamera::doStart() throw(fwTools::Failed)
{
    m_transMat = this->getObject< ::fwData::TransformationMatrix3D >();

    if (!m_cameraUID.empty())
    {
        ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(m_cameraUID);

        m_camera = ::arData::Camera::dynamicCast(obj);
        SLM_ASSERT("Missing camera", m_camera);

        m_connections->connect(m_camera, ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG,
                               this->getSptr(), s_CALIBRATE_SLOT);

        this->calibrate();
    }

    doUpdate();
}

//------------------------------------------------------------------------------

void SCamera::doUpdate() throw(fwTools::Failed)
{
    ::Ogre::Camera* camera = getCurrentCamera();

    // Multithreaded lock
    ::fwData::mt::ObjectReadLock lock(m_transMat);

    // Received input lign and column data from f4s transformation matrix
    ::Ogre::Matrix4 ogreMatrix;
    for (size_t lt = 0; lt < 4; lt++)
    {
        for (size_t ct = 0; ct < 4; ct++)
        {
            ogreMatrix[ct][lt] = static_cast<Ogre::Real>(m_transMat->getCoefficient(ct, lt));
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

    ::Ogre::Node* parent = camera->getParentNode();

    // Reset the camera position
    parent->setPosition(0, 0, 0);
    parent->setOrientation(Ogre::Quaternion::IDENTITY);

    // Update the camera position
    parent->rotate(orientation);
    parent->translate(position);

    this->requestRender();
}


//------------------------------------------------------------------------------

void SCamera::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void SCamera::doStop() throw(fwTools::Failed)
{
    m_connections->disconnect();
}

//------------------------------------------------------------------------------

void SCamera::updateTF3D()
{
    ::Ogre::Camera* camera         = getCurrentCamera();
    ::Ogre::SceneNode* camNode     = camera->getParentSceneNode();
    ::Ogre::Quaternion orientation = camNode->getOrientation();

    ::Ogre::Matrix3 mat33;
    orientation.ToRotationMatrix(mat33);

    ::Ogre::Vector3 position = camNode->getPosition();
    ::Ogre::Vector3 scale    = camNode->getScale();

    ::fwData::mt::ObjectWriteLock lock(m_transMat);

    for( size_t i = 0; i < 3; i++)
    {
        for( size_t j = 0; j < 3; j++)
        {

            // Set the 3x3 matrix representing the rotation
            m_transMat->setCoefficient(i,j,mat33[i][j]);

            if( i==j )
            {
                // Set the scale factor
                m_transMat->setCoefficient(i,j,m_transMat->getCoefficient(i,j)*scale[i]);

            }
        }
    }

    for( size_t i = 0; i < 3; i++)
    {
        // Last column represents translation
        m_transMat->setCoefficient(i,3,position[i]);

        // Set the last lines to 0
        m_transMat->setCoefficient(3,i,0);
    }

    // And the last  value
    m_transMat->setCoefficient(3,3,1);

    lock.unlock();
}

//------------------------------------------------------------------------------

Ogre::Camera* SCamera::getCurrentCamera()
{
    return this->getSceneManager()->getCamera("PlayerCam");
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType visuOgreAdaptor::SCamera::getObjSrvConnections() const
{
    ::fwServices::IService::KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    return connections;
}

//-----------------------------------------------------------------------------

void SCamera::calibrate()
{
    if ( m_camera )
    {
        ::Ogre::Camera* camera = getCurrentCamera();

        double fy = m_camera->getFy();
        camera->setFOVy(
            ::Ogre::Radian(static_cast< ::Ogre::Real >(2.0 *
                                                       atan(static_cast< double >(m_camera->getHeight() / 2.0)) /
                                                       fy)));

        this->doUpdate();
    }
}

//------------------------------------------------------------------------------

} //namespace visuOgreAdaptor
