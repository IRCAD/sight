/************************************************************************
 *
 * Copyright (C) 2018-2020 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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
#include "visuOgreAdaptor/SMaterial.hpp"

#include <fwData/Material.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>

#include <string>

namespace visuOgreAdaptor
{

/**
 * @brief This adaptor displays the frustum of an ::arData::Camera.
 *
 * @section Slots Slots
 * -\b updateVisibility(bool): Sets whether the frustum is shown or not.
 * -\b toggleVisibility(): Toggles whether the frustum is shown or not.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SFrustum">
        <in key="camera" uid="..." />
        <config layer="..." transform="..." near="..." far="..."/>
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b camera [::arData::Camera]:  camera containing calibration information.
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): defines the frustum's layer
 * - \b transform (optional, string, default=""): transform applied to the frustum's scene node
 * - \b near (optional, float, default=1.0): near clipping distance of the ::Ogre::Camera
 * - \b far (optional, float, default=20.0): far clipping distance of the ::Ogre::Camera
 * - \b color (optional, hexadecimal, default=0xFF0000): frustum's color
 */
class VISUOGREADAPTOR_CLASS_API SFrustum final :
    public ::fwRenderOgre::IAdaptor,
    public ::fwRenderOgre::ITransformable
{
public:

    fwCoreServiceMacro(SFrustum, ::fwRenderOgre::IAdaptor)

    /// Sets default parameters and initializes necessary members.
    VISUOGREADAPTOR_API SFrustum() noexcept;

    /// Does nothing
    VISUOGREADAPTOR_API virtual ~SFrustum() noexcept;

private:

    /// Configures.
    virtual void configuring() override;

    /// Manually creates a frustum.
    virtual void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG of s_CAMERA_INPUT to s_CALIBRATE_SLOT
     * Connect ::arData::Camera::s_MODIFIED_SIG of s_CAMERA_INPUT to s_CALIBRATE_SLOT
     */
    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Deletes the frustum.
    virtual void stopping() override;

    /// Checks if the camera has changed, and updates it if it has.
    virtual void updating() override;

    /// Sets ::Ogre::Camera from arData::Camera parameters.
    void setOgreCamFromData();

    /**
     * @brief SLOT: sets the visibility of the frustum.
     * @param _isVisible the visibility status.
     */
    void updateVisibility(bool _isVisible);

    /// SLOT: toggles the visibility of the frustum.
    void toggleVisibility();

    /// Contains the Ogre's camera (frustum) representing arData::Camera position and parameters.
    ::Ogre::Camera* m_ogreCamera { nullptr };

    /// Contains the material data.
    ::fwData::Material::sptr m_material { nullptr };

    /// Enables the frustum visibility.
    bool m_visibility { true };

    /// Defines the near clipping distance.
    float m_near { 1.f };

    /// Defines the far clipping distance.
    float m_far { 20.f };

    /// Defines the color of frustum.
    std::string m_color { "#FF0000" };

};

} // namespace visuOgreAdaptor.
