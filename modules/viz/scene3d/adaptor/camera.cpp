/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/camera.hpp"

#include <core/com/slots.hxx>

#include <viz/scene3d/helper/camera.hpp>
#include <viz/scene3d/render.hpp>
#include <viz/scene3d/Utils.hpp>

#include <Ogre.h>
#include <OgreCamera.h>
#include <OgreMatrix4.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t CALIBRATE_SLOT = "calibrate";
static const core::com::slots::key_t UPDATE_TF_SLOT = "updateTransformation";

//-----------------------------------------------------------------------------

struct camera::CameraNodeListener : public Ogre::MovableObject::Listener
{
    camera* m_layer {nullptr};

    //------------------------------------------------------------------------------

    explicit CameraNodeListener(camera* _renderer) :
        m_layer(_renderer)
    {
    }

    //------------------------------------------------------------------------------

    void objectMoved(Ogre::MovableObject* /*unused*/) override
    {
        m_layer->updateTF3D();
    }
};

//------------------------------------------------------------------------------

camera::camera() noexcept
{
    new_slot(UPDATE_TF_SLOT, &camera::updateTF3D, this);
    new_slot(CALIBRATE_SLOT, &camera::calibrate, this);
}

//------------------------------------------------------------------------------

camera::~camera() noexcept =
    default;

//------------------------------------------------------------------------------

void camera::configuring()
{
    this->configureParams();

    const config_t config     = this->get_config();
    const auto projectionType = config.get<std::string>("config.<xmlattr>.projection", "perspective");

    if(projectionType == "orthographic")
    {
        m_useOrthographicProjection = true;
    }
    else if(projectionType == "perspective")
    {
        m_useOrthographicProjection = false;
    }
    else
    {
        SIGHT_ERROR(
            "projection type '" + projectionType
            + "' is not managed use either 'perspective'(default) or 'orthographic'"
        );
    }
}

//------------------------------------------------------------------------------

void camera::starting()
{
    this->initialize();

    m_camera = this->getLayer()->getDefaultCamera();

    if(m_useOrthographicProjection)
    {
        m_camera->setProjectionType(Ogre::ProjectionType::PT_ORTHOGRAPHIC);
        // inform layer since some computations are a bit different from perspective.
        this->getLayer()->setOrthographicCamera(true);
    }

    m_cameraNodeListener = new CameraNodeListener(this);
    m_camera->setListener(m_cameraNodeListener);

    m_layerConnection.connect(
        this->getLayer(),
        sight::viz::scene3d::Layer::CAMERA_RANGE_UPDATED_SIG,
        this->get_sptr(),
        CALIBRATE_SLOT
    );
    m_layerConnection.connect(
        this->getLayer(),
        sight::viz::scene3d::Layer::RESIZE_LAYER_SIG,
        this->get_sptr(),
        CALIBRATE_SLOT
    );

    this->updating();
}

//-----------------------------------------------------------------------------

service::connections_t camera::auto_connections() const
{
    service::connections_t connections;
    connections.push(s_TRANSFORM_INOUT, data::matrix4::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_CALIBRATION_INPUT, data::camera::MODIFIED_SIG, CALIBRATE_SLOT);
    connections.push(s_CALIBRATION_INPUT, data::camera::INTRINSIC_CALIBRATED_SIG, CALIBRATE_SLOT);
    connections.push(s_CAMERA_SET_INPUT, data::camera_set::MODIFIED_SIG, CALIBRATE_SLOT);
    connections.push(s_CAMERA_SET_INPUT, data::camera_set::EXTRINSIC_CALIBRATED_SIG, CALIBRATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void camera::updating()
{
    if(m_calibrationDone || this->calibrate())
    {
        Ogre::Affine3 ogreMatrix;
        {
            const auto transform = m_transform.lock();

            // Received input line and column data from Sight transformation matrix
            for(std::size_t lt = 0 ; lt < 4 ; lt++)
            {
                for(std::size_t ct = 0 ; ct < 4 ; ct++)
                {
                    ogreMatrix[ct][lt] = static_cast<Ogre::Real>((*transform)(ct, lt));
                }
            }
        }

        // Decompose the camera matrix
        Ogre::Vector3 position;
        Ogre::Vector3 scale;
        Ogre::Quaternion orientation;
        ogreMatrix.decomposition(position, scale, orientation);

        // Reverse view-up and direction for AR
        const Ogre::Quaternion rotateY(Ogre::Degree(180), Ogre::Vector3(0, 1, 0));
        const Ogre::Quaternion rotateZ(Ogre::Degree(180), Ogre::Vector3(0, 0, 1));
        orientation = orientation * rotateZ * rotateY;

        // Flag to skip updateTF3D() when called from the camera listener
        m_skipUpdate = true;

        Ogre::Node* parent = m_camera->getParentNode();

        // Reset the camera position
        parent->setPosition(0, 0, 0);
        parent->setOrientation(Ogre::Quaternion::IDENTITY);

        // Update the camera position
        parent->rotate(orientation);
        parent->translate(position);

        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void camera::stopping()
{
    m_layerConnection.disconnect();

    if(m_cameraNodeListener != nullptr)
    {
        m_camera->setListener(nullptr);
        delete m_cameraNodeListener;
        m_cameraNodeListener = nullptr;
    }
}

//------------------------------------------------------------------------------

void camera::updateTF3D()
{
    if(m_skipUpdate)
    {
        // We were called from the listener after update() was called, so skip that
        m_skipUpdate = false;
        return;
    }

    const Ogre::SceneNode* camNode      = m_camera->getParentSceneNode();
    const Ogre::Quaternion& orientation = camNode->getOrientation();

    Ogre::Matrix3 mat33;
    orientation.ToRotationMatrix(mat33);

    const Ogre::Vector3& position = camNode->getPosition();

    Ogre::Matrix4 newTransMat;

    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        for(std::size_t j = 0 ; j < 3 ; j++)
        {
            // Set the 3x3 matrix representing the rotation
            newTransMat[i][j] = mat33[i][j];
        }
    }

    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        // Last column represents translation
        newTransMat[i][3] = position[i];

        // Set the last lines to 0
        newTransMat[3][i] = 0;
    }

    // And the last  value
    newTransMat[3][3] = 1;

    // Now nullify the reverse of the view-up and direction
    Ogre::Quaternion rotate;
    const Ogre::Quaternion rotateY(Ogre::Degree(180), Ogre::Vector3(0, 1, 0));
    const Ogre::Quaternion rotateZ(Ogre::Degree(180), Ogre::Vector3(0, 0, 1));
    rotate = rotateZ * rotateY;
    rotate = rotate.Inverse();

    newTransMat = newTransMat * Ogre::Matrix4(rotate);

    const auto transform = m_transform.lock();

    // Received input line and column data from Sight transformation matrix
    for(std::size_t lt = 0 ; lt < 4 ; lt++)
    {
        for(std::size_t ct = 0 ; ct < 4 ; ct++)
        {
            (*transform)(ct, lt) = static_cast<double>(newTransMat[ct][lt]);
        }
    }

    auto sig = transform->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
    {
        core::com::connection::blocker blocker(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

void camera::setNearClipDistance(Ogre::Real _nearClipDistance)
{
    SIGHT_ASSERT("The associated camera doesn't exist.", m_camera);

    m_camera->setNearClipDistance(_nearClipDistance);
}

//------------------------------------------------------------------------------

void camera::setFarClipDistance(Ogre::Real _farClipDistance)
{
    SIGHT_ASSERT("The associated camera doesn't exist.", m_camera);

    m_camera->setFarClipDistance(_farClipDistance);
}

//-----------------------------------------------------------------------------

void camera::setAspectRatio(Ogre::Real _ratio)
{
    SIGHT_ASSERT("The associated camera doesn't exist.", m_camera);

    m_aspectRatio = _ratio;
    SIGHT_ASSERT("Width and height should be strictly positive", !std::isnan(_ratio));
    m_camera->setAspectRatio(m_aspectRatio);
}

//-----------------------------------------------------------------------------

bool camera::calibrate()
{
    const auto camera_set        = m_camera_set.lock();
    const auto cameraCalibration = m_cameraCalibration.lock();

    SIGHT_WARN_IF(
        "A '" << s_CALIBRATION_INPUT << "' input was set but will not be used because a '"
        << s_CAMERA_SET_INPUT << "' was defined as well",
        camera_set && cameraCalibration
    );

    if(camera_set)
    {
        this->calibrateCameraSet(*camera_set);
    }
    else if(cameraCalibration)
    {
        this->calibrateMonoCamera(*cameraCalibration);
    }
    else
    {
        const auto width  = static_cast<float>(m_camera->getViewport()->getActualWidth());
        const auto height = static_cast<float>(m_camera->getViewport()->getActualHeight());
        if(width <= 0 || height <= 0)
        {
            SIGHT_ERROR("Width and height should be strictly positive");
            return false;
        }

        const float aspectRatio = width / height;
        m_camera->setAspectRatio(aspectRatio);
    }

    m_calibrationDone = true;
    return true;
}

//------------------------------------------------------------------------------

void camera::calibrateMonoCamera(const data::camera& _cam)
{
    const auto width    = static_cast<float>(m_camera->getViewport()->getActualWidth());
    const auto height   = static_cast<float>(m_camera->getViewport()->getActualHeight());
    const auto nearClip = static_cast<float>(m_camera->getNearClipDistance());
    const auto farClip  = static_cast<float>(m_camera->getFarClipDistance());

    if(_cam.getIsCalibrated())
    {
        Ogre::Matrix4 m = sight::viz::scene3d::helper::camera::computeProjectionMatrix(
            _cam,
            width,
            height,
            nearClip,
            farClip
        );

        m_camera->setCustomProjectionMatrix(true, m);

        this->updating();
    }
}

//------------------------------------------------------------------------------

void camera::calibrateCameraSet(const data::camera_set& _cs)
{
    const auto width         = static_cast<float>(m_camera->getViewport()->getActualWidth());
    const auto height        = static_cast<float>(m_camera->getViewport()->getActualHeight());
    const auto nearClip      = static_cast<float>(m_camera->getNearClipDistance());
    const auto farClip       = static_cast<float>(m_camera->getFarClipDistance());
    const std::size_t nbCams = _cs.size();

    SIGHT_WARN_IF(
        "There are no cameras in the '" << s_CAMERA_SET_INPUT << "', the default projection transform"
                                                                 "will be used.",
        nbCams == 0
    );

    auto layer = this->getLayer();

    // Calibrate only the first camera when stereo is not enabled.
    if(layer->getStereoMode() == sight::viz::scene3d::compositor::core::StereoModeType::NONE && nbCams > 0)
    {
        this->calibrateMonoCamera(*_cs.get_camera(0));
    }
    else
    {
        sight::viz::scene3d::Layer::CameraCalibrationsType calibrations;
        Ogre::Matrix4 extrinsicMx(Ogre::Matrix4::IDENTITY);

        for(std::size_t i = 0 ; i < nbCams ; ++i)
        {
            const data::camera::csptr camera = _cs.get_camera(i);

            if(camera->getIsCalibrated())
            {
                const auto intrinsicProjMx =
                    sight::viz::scene3d::helper::camera::computeProjectionMatrix(
                        *camera,
                        width,
                        height,
                        nearClip,
                        farClip
                    );

                calibrations.push_back(intrinsicProjMx * extrinsicMx);
            }
            else
            {
                SIGHT_ERROR("Camera #" << i << " is not calibrated.");
            }

            // In fwRenderOgre we define extrinsic calibrations as being the transform from the
            // first camera to the current one.
            if(i < nbCams - 1)
            {
                const data::matrix4::csptr extrinsic = _cs.get_extrinsic_matrix(i + 1);
                extrinsicMx = sight::viz::scene3d::Utils::convertTM3DToOgreMx(extrinsic) * extrinsicMx;
            }
        }

        if(!calibrations.empty())
        {
            layer->setCameraCalibrations(calibrations);
            this->updating();
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
