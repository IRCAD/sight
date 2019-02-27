/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "visuOgreAdaptor/SCamera.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwRenderOgre/helper/Camera.hpp>
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

const ::fwServices::IService::KeyType s_CALIBRATION_INPUT   = "calibration";
const ::fwServices::IService::KeyType s_CAMERA_SERIES_INPUT = "cameraSeries";
const ::fwServices::IService::KeyType s_TRANSFORM_INOUT     = "transform";

//------------------------------------------------------------------------------

SCamera::SCamera() noexcept
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
    connections.push( s_TRANSFORM_INOUT, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( s_CALIBRATION_INPUT, ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG, s_CALIBRATE_SLOT );
    connections.push( s_CAMERA_SERIES_INPUT, ::arData::CameraSeries::s_MODIFIED_SIG, s_CALIBRATE_SLOT);
    connections.push( s_CAMERA_SERIES_INPUT, ::arData::CameraSeries::s_EXTRINSIC_CALIBRATED_SIG, s_CALIBRATE_SLOT);

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

    m_camera = this->getLayer()->getDefaultCamera();

    m_layerConnection.connect(this->getLayer(), ::fwRenderOgre::Layer::s_CAMERA_UPDATED_SIG,
                              this->getSptr(), s_UPDATE_TF_SLOT);
    m_layerConnection.connect(this->getLayer(), ::fwRenderOgre::Layer::s_CAMERA_RANGE_UPDATED_SIG,
                              this->getSptr(), s_CALIBRATE_SLOT);
    m_layerConnection.connect(this->getLayer(), ::fwRenderOgre::Layer::s_RESIZE_LAYER_SIG,
                              this->getSptr(), s_CALIBRATE_SLOT);

    this->calibrate();
    this->updating();
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
    ::Ogre::Affine3 ogreMatrix;

    auto transform = this->getInOut< ::fwData::TransformationMatrix3D >(s_TRANSFORM_INOUT);

    SLM_ASSERT("The '" + s_TRANSFORM_INOUT + "' inout was not set but is required by this adaptor.", transform);
    {
        ::fwData::mt::ObjectReadLock lock(transform);

        // Received input lign and column data from Sight transformation matrix
        for (size_t lt = 0; lt < 4; lt++)
        {
            for (size_t ct = 0; ct < 4; ct++)
            {
                ogreMatrix[ct][lt] = static_cast<Ogre::Real>(transform->getCoefficient(ct, lt));
            }
        }
    }

    // Decompose the camera matrix
    ::Ogre::Vector3 position;
    ::Ogre::Vector3 scale;
    ::Ogre::Quaternion orientation;
    ogreMatrix.decomposition(position, scale, orientation);

    // Reverse view-up and direction for AR
    const ::Ogre::Quaternion rotateY(::Ogre::Degree(180), ::Ogre::Vector3(0, 1, 0));
    const ::Ogre::Quaternion rotateZ(::Ogre::Degree(180), ::Ogre::Vector3(0, 0, 1));
    orientation = orientation * rotateZ * rotateY;

    ::Ogre::Node* parent = m_camera->getParentNode();

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

    ::Ogre::Matrix4 newTransMat;

    for( size_t i = 0; i < 3; i++)
    {
        for( size_t j = 0; j < 3; j++)
        {
            // Set the 3x3 matrix representing the rotation
            newTransMat[i][j] = mat33[i][j];
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

    // Now nullify the reverse of the view-up and direction
    ::Ogre::Quaternion rotate;
    const ::Ogre::Quaternion rotateY(::Ogre::Degree(180), ::Ogre::Vector3(0, 1, 0));
    const ::Ogre::Quaternion rotateZ(::Ogre::Degree(180), ::Ogre::Vector3(0, 0, 1));
    rotate = rotateZ * rotateY;
    rotate = rotate.Inverse();

    newTransMat = newTransMat * rotate;

    auto transform = this->getInOut< ::fwData::TransformationMatrix3D >(s_TRANSFORM_INOUT);
    {
        ::fwData::mt::ObjectWriteLock lock(transform);

        // Received input lign and column data from Sight transformation matrix
        for (size_t lt = 0; lt < 4; lt++)
        {
            for (size_t ct = 0; ct < 4; ct++)
            {
                transform->setCoefficient(ct, lt, static_cast<double>(newTransMat[ct][lt]));
            }
        }
    }

    auto sig = transform->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SCamera::setNearClipDistance(::Ogre::Real _nearClipDistance)
{
    SLM_ASSERT("The associated camera doesn't exist.", m_camera);

    m_camera->setNearClipDistance(_nearClipDistance);
}

//------------------------------------------------------------------------------

void SCamera::setFarClipDistance(::Ogre::Real _farClipDistance)
{
    SLM_ASSERT("The associated camera doesn't exist.", m_camera);

    m_camera->setFarClipDistance(_farClipDistance);
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
    const auto cameraSeries      = this->getInput< ::arData::CameraSeries >(s_CAMERA_SERIES_INPUT);
    const auto cameraCalibration = this->getInput< ::arData::Camera >(s_CALIBRATION_INPUT);

    SLM_WARN_IF("A '" + s_CALIBRATION_INPUT +"' input was set but will not be used because a '"
                + s_CAMERA_SERIES_INPUT + "' was defined as well", cameraSeries && cameraCalibration);

    if(cameraSeries)
    {
        this->calibrateCameraSeries(cameraSeries);
    }
    else if (cameraCalibration)
    {
        this->calibrateMonoCamera(cameraCalibration);
    }
}

//------------------------------------------------------------------------------

void SCamera::calibrateMonoCamera(const arData::Camera::csptr& _cam)
{
    const float width    = static_cast< float >(m_camera->getViewport()->getActualWidth());
    const float height   = static_cast <float >(m_camera->getViewport()->getActualHeight());
    const float nearClip = static_cast< float >(m_camera->getNearClipDistance());
    const float farClip  = static_cast< float >(m_camera->getFarClipDistance());

    if ( _cam->getIsCalibrated() )
    {
        ::Ogre::Matrix4 m = ::fwRenderOgre::helper::Camera::computeProjectionMatrix(*_cam, width, height,
                                                                                    nearClip, farClip);

        m_camera->setCustomProjectionMatrix(true, m);

        this->updating();
    }
}

//------------------------------------------------------------------------------

void SCamera::calibrateCameraSeries(const arData::CameraSeries::csptr& _cs)
{
    const float width    = static_cast< float >(m_camera->getViewport()->getActualWidth());
    const float height   = static_cast <float >(m_camera->getViewport()->getActualHeight());
    const float nearClip = static_cast< float >(m_camera->getNearClipDistance());
    const float farClip  = static_cast< float >(m_camera->getFarClipDistance());
    const size_t nbCams  = _cs->getNumberOfCameras();

    SLM_WARN_IF("There are no cameras in the '" + s_CAMERA_SERIES_INPUT + "', the default projection transform"
                "will be used.", nbCams == 0);

    auto layer = this->getLayer();

    // Calibrate only the first camera when stereo is not enabled.
    if(layer->getStereoMode() == ::fwRenderOgre::compositor::Core::StereoModeType::NONE && nbCams > 0)
    {
        this->calibrateMonoCamera(_cs->getCamera(0));
    }
    else
    {
        ::fwRenderOgre::Layer::CameraCalibrationsType calibrations;
        ::Ogre::Matrix4 extrinsicMx(::Ogre::Matrix4::IDENTITY);

        for(size_t i = 0; i < nbCams; ++i)
        {
            const ::arData::Camera::csptr camera = _cs->getCamera(i);

            if(camera->getIsCalibrated())
            {
                const auto intrinsicProjMx =
                    ::fwRenderOgre::helper::Camera::computeProjectionMatrix(*camera, width, height,
                                                                            nearClip, farClip);

                calibrations.push_back(intrinsicProjMx * extrinsicMx);
            }
            else
            {
                OSLM_ERROR("Camera #" << i << " is not calibrated.");
            }

            // In fwRenderOgre we define extrinsic calibrations as being the transform from the
            // first camera to the current one.
            if(i < nbCams - 1)
            {
                const ::fwData::TransformationMatrix3D::csptr extrinsic = _cs->getExtrinsicMatrix(i + 1);
                extrinsicMx = ::fwRenderOgre::Utils::convertTM3DToOgreMx(extrinsic) * extrinsicMx;
            }
        }

        if(calibrations.size() > 0)
        {
            layer->setCameraCalibrations(calibrations);
            this->updating();
        }
    }
}

//------------------------------------------------------------------------------

} //namespace visuOgreAdaptor
