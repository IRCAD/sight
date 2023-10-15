/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019-2021 IHU Strasbourg
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

#include "modules/viz/scene3d/config.hpp"

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/interactor/trackball_interactor.hpp>

#include <memory>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor lets the user move the camera around a point of interest using the mouse and keyboard.
 *
 * See @see sight::viz::scene3d::interactor::trackball_interactor to learn more about the interactions provided by this
 * service.
 *
 * @warning may not work as intended when used at the same time as `module::viz::scene3d::adaptor::Camera` on the same
 * layer.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::viz::scene3d::adaptor::trackball_camera">
        <config priority="0" layerOrderDependant="true" />
   </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b priority (optional, int, default=0): interaction priority, higher priority interactions are performed first.
 * - \b layerOrderDependant (optional, bool, default=true): define if interaction must take into account above layers.
 */
class MODULE_VIZ_SCENE3D_CLASS_API trackball_camera final : public sight::viz::scene3d::adaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(trackball_camera, sight::viz::scene3d::adaptor);

    /// Constructor.
    MODULE_VIZ_SCENE3D_API trackball_camera() noexcept = default;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~trackball_camera() noexcept final = default;

protected:

    /// Configures the layer and the interaction priority.
    MODULE_VIZ_SCENE3D_API void configuring() final;

    /// Adds trackball interactions to the layer.
    MODULE_VIZ_SCENE3D_API void starting() final;

    /// Updates the service. Unused here.
    MODULE_VIZ_SCENE3D_API void updating() noexcept final;

    /// Removes trackball interactions from the layer.
    MODULE_VIZ_SCENE3D_API void stopping() final;

private:

    /// Contains the interaction handler.
    std::shared_ptr<sight::viz::scene3d::interactor::trackball_interactor> m_trackball;

    /// Defines the interaction priority.
    int m_priority {0};

    /// Defines if the interaction must take into account above layers.
    bool m_layerOrderDependant {true};
};

} // namespace sight::module::viz::scene3d::adaptor.
