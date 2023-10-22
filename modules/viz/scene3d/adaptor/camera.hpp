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

#pragma once

#include "modules/viz/scene3d/adaptor/transform.hpp"
#include "modules/viz/scene3d/config.hpp"

#include <data/camera.hpp>
#include <data/camera_set.hpp>
#include <data/matrix4.hpp>

#include <viz/scene3d/adaptor.hpp>

#include <OGRE/OgreMovableObject.h>

namespace sight::data
{

class Camera;

} // namespace sight::data

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor transforms a Sight camera to an Ogre camera.
 *
 * @section Slots Slots
 * - \b updateTF3D(): called when the Ogre transform matrix has been updated and updates the transform service
 * accordingly.
 * - \b calibrate(): applies calibration information to Ogre camera.
 *
 * @section XML XML Configuration
 *  *
 * @code{.xml}
    <service uid="cameraAdaptor" type="sight::module::viz::scene3d::adaptor::camera">
        <inout key="transform" uid="..." />
        <in key="calibration" uid="..." />
        <in key="cameraSet" uid="..." />
    </service>
 * @endcode
 *
 * @subsection Input Input
 * - \b calibration [sight::data::camera] (optional): camera containing calibration information.
 * - \b calibration [sight::data::camera_set] (optional): camera series containing calibration information.
 *
 * @subsection InOut InOut
 * - \b transform [sight::data::matrix4]: transform matrix for the camera.
 */
class MODULE_VIZ_SCENE3D_CLASS_API camera final : public sight::viz::scene3d::adaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(camera, sight::viz::scene3d::adaptor);

    /// Creates the adaptor and initialize slots.
    MODULE_VIZ_SCENE3D_API camera() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~camera() noexcept override;

protected:

    /// Configures the adaptor.
    MODULE_VIZ_SCENE3D_API void configuring() override;

    /// Installs layer connections and calibrates the camera if it exists.
    MODULE_VIZ_SCENE3D_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::matrix4::MODIFIED_SIG of s_TRANSFORM_INOUT to service::slots::UPDATE
     * Connect data::camera::INTRINSIC_CALIBRATED_SIG of s_CALIBRATION_INPUT to CALIBRATE_SLOT
     * Connect data::camera_set::MODIFIED_SIG of s_CAMERA_SET_INPUT to CALIBRATE_SLOT
     * Connect data::camera_set::EXTRINSIC_CALIBRATED_SIG of s_CAMERA_SET_INPUT to CALIBRATE_SLOT
     */
    MODULE_VIZ_SCENE3D_API service::connections_t auto_connections() const override;

    /// Sets the camera's transform.
    MODULE_VIZ_SCENE3D_API void updating() override;

    /// Removes layer connections.
    MODULE_VIZ_SCENE3D_API void stopping() override;

private:

    /// Calibrates the scene's camera(s) using the input calibration(s).
    bool calibrate();

    /**
     * @brief Computes the camera's projection matrix using its intrinsic parameters and sets it in the scene.
     * @param _cam data use to retreive the camera intrinsic parameters.
     */
    void calibrateMonoCamera(const data::camera& _cam);

    /// Computes a projection matrix for each camera in the series and set them in the layer.
    /// This matrix is equal to the intrinsic times the extrinsic matrix.
    void calibrateCameraSet(const data::camera_set& _cs);

    /// Updates Transformation Matrix.
    void updateTF3D();

    /// Defines the near clipping plane position.
    void setNearClipDistance(Ogre::Real _near_clip_distance);

    /// Defines the far clipping plane position.
    void setFarClipDistance(Ogre::Real _far_clip_distance);

    /// Defines the aspect ratio.
    void setAspectRatio(Ogre::Real _ratio);

    /// Contains the Ogre camera managed by this adaptor.
    Ogre::Camera* m_camera {nullptr};

    /// Defines the aspect ratio for the frustum viewport.
    Ogre::Real m_aspectRatio {0.F};

    /// Handles connection with the layer.
    core::com::helper::sig_slot_connection m_layerConnection;

    /// Defines the camera listener class used to pass the projection matrix for autostereo shaders.
    struct CameraNodeListener;
    CameraNodeListener* m_cameraNodeListener {nullptr};

    /// This avoids a self-call to updateTF3D() when we update() the camera
    bool m_skipUpdate {false};

    /// Indicates if the calibration has been done successfully. If not, no update can be done without a calibrate
    bool m_calibrationDone {false};

    static constexpr std::string_view s_CALIBRATION_INPUT = "calibration";
    static constexpr std::string_view s_CAMERA_SET_INPUT  = "cameraSet";
    static constexpr std::string_view s_TRANSFORM_INOUT   = "transform";

    data::ptr<data::camera, data::Access::in> m_cameraCalibration {this, s_CALIBRATION_INPUT, true, true};
    data::ptr<data::camera_set, data::Access::in> m_camera_set {this, s_CAMERA_SET_INPUT, true, true};
    data::ptr<data::matrix4, data::Access::inout> m_transform {this, s_TRANSFORM_INOUT, true};

    bool m_useOrthographicProjection {false};
};

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
