/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SCamera.hpp"

#include <arData/Camera.hpp>

#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwRenderOgre/SRender.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <OgreCamera.h>
#include <OgreMatrix4.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreVector3.h>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SCamera, ::fwData::TransformationMatrix3D);

//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SCamera::s_CALIBRATE_SLOT = "calibrate";
const ::fwCom::Slots::SlotKeyType SCamera::s_UPDATE_TF_SLOT = "updateTransformation";

//------------------------------------------------------------------------------

SCamera::SCamera() noexcept :
    m_camera(nullptr),
    m_nearClipDistance(1.f),
    m_aspectRatio(0.f)
{
    newSlot(s_UPDATE_TF_SLOT, &SCamera::updateTF3D, this);
    newSlot(s_CALIBRATE_SLOT, &SCamera::calibrate, this);
}

//------------------------------------------------------------------------------

SCamera::~SCamera() noexcept
{
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap visuOgreAdaptor::SCamera::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( "transform", ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( "calibration", ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG, s_CALIBRATE_SLOT );
    return connections;
}

//------------------------------------------------------------------------------

void SCamera::configuring()
{
    this->configureParams();
}

//------------------------------------------------------------------------------

void SCamera::starting()
{
    this->initialize();

    m_camera      = this->getLayer()->getDefaultCamera();
    m_calibration = this->getInput< ::arData::Camera >("calibration");

    m_layerConnection.connect(this->getLayer(), ::fwRenderOgre::Layer::s_CAMERA_UPDATED_SIG,
                              this->getSptr(), s_UPDATE_TF_SLOT);
    if (m_calibration)
    {
        this->calibrate();
    }
}

//------------------------------------------------------------------------------

void SCamera::stopping()
{
    m_layerConnection.disconnect();

    this->unregisterServices();
}

//------------------------------------------------------------------------------

void SCamera::updating()
{
    ::Ogre::Camera* camera = m_camera;

    // Multithreaded lock
    auto transform = this->getInOut< ::fwData::TransformationMatrix3D >("transform");
    ::fwData::mt::ObjectReadLock lock(transform);

    // Received input lign and column data from f4s transformation matrix
    ::Ogre::Matrix4 ogreMatrix;
    for (size_t lt = 0; lt < 4; lt++)
    {
        for (size_t ct = 0; ct < 4; ct++)
        {
            ogreMatrix[ct][lt] = static_cast<Ogre::Real>(transform->getCoefficient(ct, lt));
        }
    }

    lock.unlock();

    // Decompose the camera matrix
    ::Ogre::Vector3 position;
    ::Ogre::Vector3 scale;
    ::Ogre::Quaternion orientation;
    ogreMatrix.decomposition(position, scale, orientation);

    const ::Ogre::Quaternion rotateY(::Ogre::Degree(180), ::Ogre::Vector3(0, 1, 0));
    const ::Ogre::Quaternion rotateZ(::Ogre::Degree(180), ::Ogre::Vector3(0, 0, 1));
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

void SCamera::updateTF3D()
{
    const ::Ogre::SceneNode* camNode      = m_camera->getParentSceneNode();
    const ::Ogre::Quaternion& orientation = camNode->getOrientation();

    ::Ogre::Matrix3 mat33;
    orientation.ToRotationMatrix(mat33);

    const ::Ogre::Vector3& position = camNode->getPosition();
    const ::Ogre::Vector3& scale    = camNode->getScale();

    ::Ogre::Matrix4 newTransMat;

    for( size_t i = 0; i < 3; i++)
    {
        for( size_t j = 0; j < 3; j++)
        {
            // Set the 3x3 matrix representing the rotation
            newTransMat[i][j] = mat33[i][j];

            if( i == j )
            {
                // Set the scale factor
                newTransMat[i][j] = newTransMat[i][j] * scale[i];
            }
        }
    }

    for( size_t i = 0; i < 3; i++)
    {
        // Last column represents translation
        newTransMat[i][3] = position[i];

        // Set the last lines to 0
        newTransMat[3][i] = 0;
    }

    // And the last  value
    newTransMat[3][3] = 1;
}

//------------------------------------------------------------------------------

void SCamera::setNearClipDistance(::Ogre::Real _nearClipDistance)
{
    SLM_ASSERT("The associated camera doesn't exist.", m_camera);

    m_nearClipDistance = _nearClipDistance;
    m_camera->setNearClipDistance(m_nearClipDistance);
}

//------------------------------------------------------------------------------

void SCamera::setFarClipDistance(::Ogre::Real _farClipDistance)
{
    SLM_ASSERT("The associated camera doesn't exist.", m_camera);

    m_farClipDistance = _farClipDistance;
    m_camera->setFarClipDistance(m_farClipDistance);
}

//-----------------------------------------------------------------------------

void SCamera::setAspectRatio(::Ogre::Real _ratio)
{
    SLM_ASSERT("The associated camera doesn't exist.", m_camera);

    m_aspectRatio = _ratio;
    m_camera->setAspectRatio(m_aspectRatio);
}

//-----------------------------------------------------------------------------

void SCamera::calibrate()
{
    if ( m_calibration && m_calibration->getIsCalibrated() )
    {
        const double fy = m_calibration->getFy();

        m_camera->setFOVy(
            ::Ogre::Radian(static_cast< ::Ogre::Real >(2.0 *
                                                       atan(static_cast< double >(m_calibration->getHeight() / 2) /
                                                            fy))));
        this->updating();
    }
}

//------------------------------------------------------------------------------

} //namespace visuOgreAdaptor
