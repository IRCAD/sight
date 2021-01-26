/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include <data/Material.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>

#include <string>

namespace visuOgreAdaptor
{

/**
 * @brief This adaptor displays the frustum of an data::Camera.
 *
 * @section Slots Slots
 * - \b updateVisibility(bool): sets whether the frustum is shown or not.
 * - \b toggleVisibility(): toggles whether the frustum is shown or not.
 * - \b show(): shows the frustum.
 * - \b hide(): hides the frustum.
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
 * - \b camera [data::Camera]:  camera containing calibration information.
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): defines the frustum's layer
 * - \b transform (optional, string, default=""): transform applied to the frustum's scene node
 * - \b near (optional, float, default=1.0): near clipping distance of the ::Ogre::Camera
 * - \b far (optional, float, default=20.0): far clipping distance of the ::Ogre::Camera
 * - \b color (optional, hexadecimal, default=0xFF0000): frustum's color
 * - \b visible (optional, bool, default=true): the visibility of the adaptor.
 */
class VISUOGREADAPTOR_CLASS_API SFrustum final :
    public ::fwRenderOgre::IAdaptor,
    public ::fwRenderOgre::ITransformable
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SFrustum, ::fwRenderOgre::IAdaptor)

    /// Sets default parameters and initializes necessary members.
    VISUOGREADAPTOR_API SFrustum() noexcept;

    /// Destroys the adaptor.
    VISUOGREADAPTOR_API ~SFrustum() noexcept override;

protected:

    /// Configures.
    VISUOGREADAPTOR_API void configuring() override;

    /// Manually creates a frustum.
    VISUOGREADAPTOR_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Camera::s_INTRINSIC_CALIBRATED_SIG of s_CAMERA_INPUT to s_CALIBRATE_SLOT
     * Connect data::Camera::s_MODIFIED_SIG of s_CAMERA_INPUT to s_CALIBRATE_SLOT
     */
    VISUOGREADAPTOR_API services::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Deletes the frustum.
    VISUOGREADAPTOR_API void stopping() override;

    /// Checks if the camera has changed, and updates it if it has.
    VISUOGREADAPTOR_API void updating() override;

    /**
     * @brief Sets the frustum visibility.
     * @param _visible the visibility status of the frustum.
     */
    VISUOGREADAPTOR_API void setVisible(bool _visible) override;

private:

    /// Sets ::Ogre::Camera from data::Camera parameters.
    void setOgreCamFromData();

    /// Contains the Ogre's camera (frustum) representing data::Camera position and parameters.
    ::Ogre::Camera* m_ogreCamera { nullptr };

    /// Contains the material data.
    data::Material::sptr m_material { nullptr };

    /// Defines the near clipping distance.
    float m_near { 1.f };

    /// Defines the far clipping distance.
    float m_far { 20.f };

    /// Defines the color of frustum.
    std::string m_color { "#FF0000" };

};

} // namespace visuOgreAdaptor.
