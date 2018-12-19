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
 * @brief Adaptor from Sight Camera to Ogre Camera.
 *
 * @section Slots Slots
 * - \b updateTF3D(): Called when the Ogre transform matrix has been updated and updates the transform service
 * accordingly.
 * - \b calibrate(): Applies calibration information to Ogre camera.
 *
 * @section XML XML Configuration
 * @code{.xml}
 *  <service uid="cameraAdaptor" type="::visuOgreAdaptor::SCamera">
 *      <inout key="transform" uid="..." />
 *      <in key="calibration" uid="..." />
 *      <config layer="..." />
 *  </service>
 * @endcode
 * @subsection Configuration Configuration
 * - \b layer (mandatory): defines the camera's layer
 * @subsection Input Input
 * - \b calibration [::arData::Camera]: camera containing calibration information.
 * @subsection InOut InOut
 * - \b transform [::fwData::TransformationMatrix3D]: transform matrix for the camera.
 */
class VISUOGREADAPTOR_CLASS_API SCamera : public ::fwRenderOgre::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro((SCamera)(::fwRenderOgre::IAdaptor))

    /// Constructor.
    VISUOGREADAPTOR_API SCamera() noexcept;

    /// Destructor. Does nothing.
    VISUOGREADAPTOR_API virtual ~SCamera() noexcept override;

    /**
     * @name Slots API
     * @{
     */
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_CALIBRATE_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_TF_SLOT;
    /** @} */

    /// Updates Transformation Matrix.
    VISUOGREADAPTOR_API virtual void updateTF3D();

    /// Returns proposals to connect service slots to associated object signals.
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Near clipping plane position setter.
    VISUOGREADAPTOR_API virtual void setNearClipDistance(::Ogre::Real _nearClipDistance);

    /// Far clipping plane position setter.
    VISUOGREADAPTOR_API virtual void setFarClipDistance(::Ogre::Real _farClipDistance);

    /// Aspect ratio setter.
    VISUOGREADAPTOR_API virtual void setAspectRatio(::Ogre::Real _ratio);

protected:
    /// Configures the adaptor.
    VISUOGREADAPTOR_API void configuring() override;

    /// Installs layer connections and calibrates the camera if it exists.
    VISUOGREADAPTOR_API void starting() override;

    /// Removes layer connections.
    VISUOGREADAPTOR_API void stopping() override;

    /// Sets the camera's transform.
    VISUOGREADAPTOR_API void updating() override;

private:

    /// Calibrates the scene's camera(s) using the input calibration(s).
    void calibrate();

    /// Computes the camera's projection matrix using its intrinsic parameters and sets it in the scene.
    void calibrateMonoCamera(const ::arData::Camera::csptr& _cam);

    /// Computes a projection matrix for each camera in the series and set them in the layer.
    /// This matrix is equal to the intrinsic times the extrinsic matrix.
    void calibrateCameraSeries(const ::arData::CameraSeries::csptr& _cs);

    /// Ogre camera managed by this adaptor.
    ::Ogre::Camera* m_camera { nullptr };

    /// Aspect ratio for the frustum viewport.
    ::Ogre::Real m_aspectRatio { 0.f };

    /// Connection with the layer
    ::fwCom::helper::SigSlotConnection m_layerConnection;
};

//------------------------------------------------------------------------------

} //namespace visuOgreAdaptor
