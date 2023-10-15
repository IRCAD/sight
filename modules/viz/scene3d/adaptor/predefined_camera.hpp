/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include <data/landmarks.hpp>

#include <ui/__/parameter.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/interactor/predefined_position_interactor.hpp>

#include <memory>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor lets the user move the camera around a point of interest using the mouse and keyboard.
 *
 * See @see sight::viz::scene3d::interactor::predefined_position_interactor to learn more about the interactions
 * provided
 * by this service.
 *
 * @warning may not work as intended when used at the same time as `module::viz::scene3d::adaptor::Camera` on the same
 * layer.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::viz::scene3d::adaptor::predefined_camera">
        <config priority="0" mouseRotation="true" defaultPosition="pos3" />
        <in key="transform" uid="..." auto_connect="true"/>
        <positions>
            <position name="pos1" rx="-30.0" />
            <position name="pos2" rx="-30.0" ry="90.0" />
            <position name="pos3" rx="-30.0" ry="-90.0"/>
        </positions>
   </service>
   @endcode
 *
 *
 * @subsection Input Input:
 * - \b transform: initial transform (registration, tracking, ...) to apply to the adaptor first (optional).
 *
 * @subsection Configuration Configuration:
 * - \b priority (optional, int, default=0): interaction priority, higher priority interactions are performed first.
 * - \b layerOrderDependant (optional, bool, default=true): defines if interaction must take into account above layers.
 * - \b mouseRotation (optional, bool, default=true): defines if mouse rotation through mouse is activated or not.
 * - \b defaultPosition (optional, string, default=""): defines the default position to use.
 * - \b animate (optional, bool, default=true): defines if an animation is used when switching position or not.
 *
 * @section Slots Slots
 * - \b setParameter(parameter_t value, std::string key): If key = "position", looking for value in the position name,
 * then goes to that position if found.
 * - \b nextPosition: Go to the next position (cyclic iteration).
 * - \b previousPosition: Go to the previous position (cyclic iteration).
 * - \b updateTransform: Update adaptor using the input transformation,
 * this slot is called automatically if auto_connect="true".
 */
class MODULE_VIZ_SCENE3D_CLASS_API predefined_camera final : public sight::viz::scene3d::adaptor
{
public:

    struct slots
    {
        using slots_t = core::com::slots::key_t;

        inline static const slots_t SET_PARAMETER     = "setParameter";
        inline static const slots_t NEXT_POSITION     = "nextPosition";
        inline static const slots_t PREVIOUS_POSITION = "previousPosition";
        inline static const slots_t UPDATE_TRANSFORM  = "updateTransform";
    };

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(predefined_camera, sight::viz::scene3d::adaptor);

    /// Constructor.
    MODULE_VIZ_SCENE3D_API predefined_camera() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~predefined_camera() noexcept final = default;

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

    connections_t auto_connections() const final;

    void updateTransform();
    void setParameter(ui::parameter_t value, std::string key);

    using predefined_position_t =
        sight::viz::scene3d::interactor::predefined_position_interactor::predefined_position_t;

    std::vector<predefined_position_t> m_cameraPositions;
    /// Contains the interaction handler.
    std::shared_ptr<sight::viz::scene3d::interactor::predefined_position_interactor> m_interactor;

    /// Defines the interaction priority.
    int m_priority {0};

    /// Defines if the interaction must take into account above layers.
    bool m_layerOrderDependant {true};

    /// Defines if one can move using the mouse interaction (default off)
    bool m_manualRotation {false};

    /// Defines the default position to use.
    std::optional<std::string> m_defaultPosition;

    /// Defines if an animation is performed when switching positions
    bool m_animate {true};

    /// Input transform.
    static constexpr std::string_view s_REGISTRATION_TRANSFORM_IN = "transform";
    sight::data::ptr<sight::data::matrix4, sight::data::Access::in> m_transform {
        this, s_REGISTRATION_TRANSFORM_IN, true, true
    };
};

} // namespace sight::module::viz::scene3d::adaptor.
