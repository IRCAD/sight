/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "modules/viz/scene3d/adaptor/SMaterial.hpp"
#include "modules/viz/scene3d/config.hpp"

#include <data/Camera.hpp>
#include <data/Material.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/transformable.hpp>

#include <string>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor displays the frustum of a data::Camera.
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
    <service uid="..." type="sight::module::viz::scene3d::adaptor::SFrustum">
        <in key="camera" uid="..." />
        <config transform="..." near="..." far="..."/>
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b camera [sight::data::Camera]:  camera containing calibration information.
 *
 * @subsection Configuration Configuration:
 * - \b transform (optional, string, default=""): transform applied to the frustum's scene node
 * - \b near (optional, float, default=1.0): near clipping distance of the Ogre::Camera
 * - \b far (optional, float, default=20.0): far clipping distance of the Ogre::Camera
 * - \b color (optional, hexadecimal, default=0xFF0000): frustum's color
 * - \b visible (optional, bool, default=true): the visibility of the adaptor.
 */
class MODULE_VIZ_SCENE3D_CLASS_API SFrustum final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::transformable
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SFrustum, sight::viz::scene3d::adaptor);

    /// Sets default parameters and initializes necessary members.
    MODULE_VIZ_SCENE3D_API SFrustum() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~SFrustum() noexcept override;

protected:

    /// Configures.
    MODULE_VIZ_SCENE3D_API void configuring() override;

    /// Manually creates a frustum.
    MODULE_VIZ_SCENE3D_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Camera::INTRINSIC_CALIBRATED_SIG of s_CAMERA_INPUT to CALIBRATE_SLOT
     * Connect data::Camera::MODIFIED_SIG of s_CAMERA_INPUT to CALIBRATE_SLOT
     */
    MODULE_VIZ_SCENE3D_API service::connections_t getAutoConnections() const override;

    /// Deletes the frustum.
    MODULE_VIZ_SCENE3D_API void stopping() override;

    /// Checks if the camera has changed, and updates it if it has.
    MODULE_VIZ_SCENE3D_API void updating() override;

    /**
     * @brief Sets the frustum visibility.
     * @param _visible the visibility status of the frustum.
     */
    MODULE_VIZ_SCENE3D_API void setVisible(bool _visible) override;

private:

    /// Sets Ogre::Camera from data::Camera parameters.
    void setOgreCamFromData();

    /// Contains the manual object of the line.
    Ogre::ManualObject* m_frustum {nullptr};

    /// Contains the Ogre's camera (frustum) representing data::Camera position and parameters.
    Ogre::Camera* m_ogreCamera {nullptr};

    /// Contains the material data.
    data::Material::sptr m_material {nullptr};

    /// Defines the near clipping distance.
    float m_near {1.F};

    /// Defines the far clipping distance.
    float m_far {20.F};

    /// Defines the color of frustum.
    std::string m_color {"#FF0000"};

    static constexpr std::string_view s_CAMERA_INPUT = "camera";
    sight::data::ptr<sight::data::Camera, sight::data::Access::in> m_camera {this, s_CAMERA_INPUT};
};

} // namespace sight::module::viz::scene3d::adaptor.
