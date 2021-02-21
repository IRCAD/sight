/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "modules/viz/scene3d/adaptor/STransform.hpp"
#include "modules/viz/scene3d/config.hpp"

#include <core/com/helper/SigSlotConnection.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>

#include <data/Camera.hpp>
#include <data/CameraSeries.hpp>

#include <viz/scene3d/IAdaptor.hpp>

#include <OGRE/OgreMovableObject.h>

#include <string>
#include <vector>

namespace sight::data
{
class Camera;
}

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
 * @code{.xml}
    <service uid="cameraAdaptor" type="::sight::module::viz::scene3d::adaptor::SCamera">
        <inout key="transform" uid="..." />
        <in key="calibration" uid="..." />
        <config layer="..." />
    </service>
 * @endcode
 *
 * @subsection Input Input
 * - \b calibration [sight::data::Camera]: camera containing calibration information.
 *
 * @subsection InOut InOut
 * - \b transform [sight::data::Matrix4]: transform matrix for the camera.
 *
 * @subsection Configuration Configuration
 * - \b layer (mandatory, string): defines the camera's layer
 */
class MODULE_VIZ_SCENE3D_CLASS_API SCamera final : public sight::viz::scene3d::IAdaptor
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SCamera, ::sight::viz::scene3d::IAdaptor)

    /// Creates the adaptor and initialize slots.
    MODULE_VIZ_SCENE3D_API SCamera() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~SCamera() noexcept override;

protected:

    /// Configures the adaptor.
    MODULE_VIZ_SCENE3D_API void configuring() override;

    /// Installs layer connections and calibrates the camera if it exists.
    MODULE_VIZ_SCENE3D_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Matrix4::s_MODIFIED_SIG of s_TRANSFORM_INOUT to s_UPDATE_SLOT
     * Connect data::Camera::s_INTRINSIC_CALIBRATED_SIG of s_CALIBRATION_INPUT to s_CALIBRATE_SLOT
     * Connect data::CameraSeries::s_MODIFIED_SIG of s_CAMERA_SERIES_INPUT to s_CALIBRATE_SLOT
     * Connect data::CameraSeries::s_EXTRINSIC_CALIBRATED_SIG of s_CAMERA_SERIES_INPUT to s_CALIBRATE_SLOT
     */
    MODULE_VIZ_SCENE3D_API service::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Sets the camera's transform.
    MODULE_VIZ_SCENE3D_API void updating() override;

    /// Removes layer connections.
    MODULE_VIZ_SCENE3D_API void stopping() override;

private:

    /// Calibrates the scene's camera(s) using the input calibration(s).
    void calibrate();

    /**
     * @brief Computes the camera's projection matrix using its intrinsic parameters and sets it in the scene.
     * @param _cam data use to retreive the camera intrinsic parameters.
     */
    void calibrateMonoCamera(const data::Camera::csptr& _cam);

    /// Computes a projection matrix for each camera in the series and set them in the layer.
    /// This matrix is equal to the intrinsic times the extrinsic matrix.
    void calibrateCameraSeries(const data::CameraSeries::csptr& _cs);

    /// Updates Transformation Matrix.
    void updateTF3D();

    /// Defines the near clipping plane position.
    void setNearClipDistance(::Ogre::Real _nearClipDistance);

    /// Defines the far clipping plane position.
    void setFarClipDistance(::Ogre::Real _farClipDistance);

    /// Defines the aspect ratio.
    void setAspectRatio(::Ogre::Real _ratio);

    /// Contains the Ogre camera managed by this adaptor.
    ::Ogre::Camera* m_camera { nullptr };

    /// Defines the aspect ratio for the frustum viewport.
    ::Ogre::Real m_aspectRatio { 0.f };

    /// Handles connection with the layer.
    core::com::helper::SigSlotConnection m_layerConnection;

    /// Defines the camera listener class used to pass the projection matrix for autostereo shaders.
    struct CameraNodeListener;
    CameraNodeListener* m_cameraNodeListener { nullptr };

    /// This avoids a self-call to updateTF3D() when we update() the camera
    bool m_skipUpdate { false };
};

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
