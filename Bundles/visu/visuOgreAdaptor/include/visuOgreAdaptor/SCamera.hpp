/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
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

#include "visuOgreAdaptor/config.hpp"
#include "visuOgreAdaptor/STransform.hpp"

#include <arData/Camera.hpp>
#include <arData/CameraSeries.hpp>

#include <fwCom/helper/SigSlotConnection.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwRenderOgre/IAdaptor.hpp>

#include <OGRE/OgreMovableObject.h>

#include <string>
#include <vector>

namespace arData
{
class Camera;
}

namespace visuOgreAdaptor
{

/**
 * @brief This adaptor transform a Sight camera to an Ogre camera.
 *
 * @section Slots Slots
 * - \b updateTF3D(): Called when the Ogre transform matrix has been updated and updates the transform service
 * accordingly.
 * - \b calibrate(): Applies calibration information to Ogre camera.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="cameraAdaptor" type="::visuOgreAdaptor::SCamera">
        <inout key="transform" uid="..." />
        <in key="calibration" uid="..." />
        <config layer="..." />
    </service>
 * @endcode
 *
 * @subsection Input Input
 * - \b calibration [::arData::Camera]: camera containing calibration information.
 *
 * @subsection InOut InOut
 * - \b transform [::fwData::TransformationMatrix3D]: transform matrix for the camera.
 *
 * @subsection Configuration Configuration
 * - \b layer (mandatory, string): defines the camera's layer
 */
class VISUOGREADAPTOR_CLASS_API SCamera final : public ::fwRenderOgre::IAdaptor
{

public:

    fwCoreServiceMacro(SCamera, ::fwRenderOgre::IAdaptor)

    /// Creates the adaptor and initialize slots..
    VISUOGREADAPTOR_API SCamera() noexcept;

    /// Does nothing.
    VISUOGREADAPTOR_API virtual ~SCamera() noexcept override;

private:

    /// Configures the adaptor.
    virtual void configuring() override;

    /// Installs layer connections and calibrates the camera if it exists.
    void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwData::TransformationMatrix3D::s_MODIFIED_SIG of s_TRANSFORM_INOUT tos_UPDATE_SLOT
     * Connect ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG of s_CALIBRATION_INPUT to s_CALIBRATE_SLOT
     * Connect ::arData::CameraSeries::s_MODIFIED_SIG of s_CAMERA_SERIES_INPUT to s_CALIBRATE_SLOT
     * Connect ::arData::CameraSeries::s_EXTRINSIC_CALIBRATED_SIG of s_CAMERA_SERIES_INPUT to s_CALIBRATE_SLOT
     */
    virtual ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Sets the camera's transform.
    virtual void updating() override;

    /// Removes layer connections.
    virtual void stopping() override;

    /// Calibrates the scene's camera(s) using the input calibration(s).
    void calibrate();

    /**
     * @brief Computes the camera's projection matrix using its intrinsic parameters and sets it in the scene.
     * @param _cam data use to retreive the camera intrinsic parameters.
     */
    void calibrateMonoCamera(const ::arData::Camera::csptr& _cam);

    /// Computes a projection matrix for each camera in the series and set them in the layer.
    /// This matrix is equal to the intrinsic times the extrinsic matrix.
    void calibrateCameraSeries(const ::arData::CameraSeries::csptr& _cs);

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
    ::fwCom::helper::SigSlotConnection m_layerConnection;

};

//------------------------------------------------------------------------------

} // namespace visuOgreAdaptor.
