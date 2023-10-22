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
#include <viz/scene3d/utils.hpp>

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

    const config_t config      = this->get_config();
    const auto projection_type = config.get<std::string>("config.<xmlattr>.projection", "perspective");

    if(projection_type == "orthographic")
    {
        m_useOrthographicProjection = true;
    }
    else if(projection_type == "perspective")
    {
        m_useOrthographicProjection = false;
    }
    else
    {
        SIGHT_ERROR(
            "projection type '" + projection_type
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
        sight::viz::scene3d::layer::CAMERA_RANGE_UPDATED_SIG,
        this->get_sptr(),
        CALIBRATE_SLOT
    );
    m_layerConnection.connect(
        this->getLayer(),
        sight::viz::scene3d::layer::RESIZE_LAYER_SIG,
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
        Ogre::Affine3 ogre_matrix;
        {
            const auto transform = m_transform.lock();

            // Received input line and column data from Sight transformation matrix
            for(std::size_t lt = 0 ; lt < 4 ; lt++)
            {
                for(std::size_t ct = 0 ; ct < 4 ; ct++)
                {
                    ogre_matrix[ct][lt] = static_cast<Ogre::Real>((*transform)(ct, lt));
                }
            }
        }

        // Decompose the camera matrix
        Ogre::Vector3 position;
        Ogre::Vector3 scale;
        Ogre::Quaternion orientation;
        ogre_matrix.decomposition(position, scale, orientation);

        // Reverse view-up and direction for AR
        const Ogre::Quaternion rotate_y(Ogre::Degree(180), Ogre::Vector3(0, 1, 0));
        const Ogre::Quaternion rotate_z(Ogre::Degree(180), Ogre::Vector3(0, 0, 1));
        orientation = orientation * rotate_z * rotate_y;

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

    const Ogre::SceneNode* cam_node     = m_camera->getParentSceneNode();
    const Ogre::Quaternion& orientation = cam_node->getOrientation();

    Ogre::Matrix3 mat33;
    orientation.ToRotationMatrix(mat33);

    const Ogre::Vector3& position = cam_node->getPosition();

    Ogre::Matrix4 new_trans_mat;

    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        for(std::size_t j = 0 ; j < 3 ; j++)
        {
            // Set the 3x3 matrix representing the rotation
            new_trans_mat[i][j] = mat33[i][j];
        }
    }

    for(std::size_t i = 0 ; i < 3 ; i++)
    {
        // Last column represents translation
        new_trans_mat[i][3] = position[i];

        // Set the last lines to 0
        new_trans_mat[3][i] = 0;
    }

    // And the last  value
    new_trans_mat[3][3] = 1;

    // Now nullify the reverse of the view-up and direction
    Ogre::Quaternion rotate;
    const Ogre::Quaternion rotate_y(Ogre::Degree(180), Ogre::Vector3(0, 1, 0));
    const Ogre::Quaternion rotate_z(Ogre::Degree(180), Ogre::Vector3(0, 0, 1));
    rotate = rotate_z * rotate_y;
    rotate = rotate.Inverse();

    new_trans_mat = new_trans_mat * Ogre::Matrix4(rotate);

    const auto transform = m_transform.lock();

    // Received input line and column data from Sight transformation matrix
    for(std::size_t lt = 0 ; lt < 4 ; lt++)
    {
        for(std::size_t ct = 0 ; ct < 4 ; ct++)
        {
            (*transform)(ct, lt) = static_cast<double>(new_trans_mat[ct][lt]);
        }
    }

    auto sig = transform->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    {
        core::com::connection::blocker blocker(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

void camera::setNearClipDistance(Ogre::Real _near_clip_distance)
{
    SIGHT_ASSERT("The associated camera doesn't exist.", m_camera);

    m_camera->setNearClipDistance(_near_clip_distance);
}

//------------------------------------------------------------------------------

void camera::setFarClipDistance(Ogre::Real _far_clip_distance)
{
    SIGHT_ASSERT("The associated camera doesn't exist.", m_camera);

    m_camera->setFarClipDistance(_far_clip_distance);
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
    const auto camera_set         = m_camera_set.lock();
    const auto camera_calibration = m_cameraCalibration.lock();

    SIGHT_WARN_IF(
        "A '" << s_CALIBRATION_INPUT << "' input was set but will not be used because a '"
        << s_CAMERA_SET_INPUT << "' was defined as well",
        camera_set && camera_calibration
    );

    if(camera_set)
    {
        this->calibrateCameraSet(*camera_set);
    }
    else if(camera_calibration)
    {
        this->calibrateMonoCamera(*camera_calibration);
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

        const float aspect_ratio = width / height;
        m_camera->setAspectRatio(aspect_ratio);
    }

    m_calibrationDone = true;
    return true;
}

//------------------------------------------------------------------------------

void camera::calibrateMonoCamera(const data::camera& _cam)
{
    const auto width     = static_cast<float>(m_camera->getViewport()->getActualWidth());
    const auto height    = static_cast<float>(m_camera->getViewport()->getActualHeight());
    const auto near_clip = static_cast<float>(m_camera->getNearClipDistance());
    const auto far_clip  = static_cast<float>(m_camera->getFarClipDistance());

    if(_cam.getIsCalibrated())
    {
        Ogre::Matrix4 m = sight::viz::scene3d::helper::camera::computeProjectionMatrix(
            _cam,
            width,
            height,
            near_clip,
            far_clip
        );

        m_camera->setCustomProjectionMatrix(true, m);

        this->updating();
    }
}

//------------------------------------------------------------------------------

void camera::calibrateCameraSet(const data::camera_set& _cs)
{
    const auto width          = static_cast<float>(m_camera->getViewport()->getActualWidth());
    const auto height         = static_cast<float>(m_camera->getViewport()->getActualHeight());
    const auto near_clip      = static_cast<float>(m_camera->getNearClipDistance());
    const auto far_clip       = static_cast<float>(m_camera->getFarClipDistance());
    const std::size_t nb_cams = _cs.size();

    SIGHT_WARN_IF(
        "There are no cameras in the '" << s_CAMERA_SET_INPUT << "', the default projection transform"
                                                                 "will be used.",
        nb_cams == 0
    );

    auto layer = this->getLayer();

    // Calibrate only the first camera when stereo is not enabled.
    if(layer->getStereoMode() == sight::viz::scene3d::compositor::core::stereo_mode_t::NONE && nb_cams > 0)
    {
        this->calibrateMonoCamera(*_cs.get_camera(0));
    }
    else
    {
        sight::viz::scene3d::layer::camera_calibrations_t calibrations;
        Ogre::Matrix4 extrinsic_mx(Ogre::Matrix4::IDENTITY);

        for(std::size_t i = 0 ; i < nb_cams ; ++i)
        {
            const data::camera::csptr camera = _cs.get_camera(i);

            if(camera->getIsCalibrated())
            {
                const auto intrinsic_proj_mx =
                    sight::viz::scene3d::helper::camera::computeProjectionMatrix(
                        *camera,
                        width,
                        height,
                        near_clip,
                        far_clip
                    );

                calibrations.push_back(intrinsic_proj_mx * extrinsic_mx);
            }
            else
            {
                SIGHT_ERROR("Camera #" << i << " is not calibrated.");
            }

            // In fwRenderOgre we define extrinsic calibrations as being the transform from the
            // first camera to the current one.
            if(i < nb_cams - 1)
            {
                const data::matrix4::csptr extrinsic = _cs.get_extrinsic_matrix(i + 1);
                extrinsic_mx = sight::viz::scene3d::utils::convertTM3DToOgreMx(extrinsic) * extrinsic_mx;
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
