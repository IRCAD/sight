/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

#include <data/string.hpp>

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
        <config priority="0" mouseRotation="true" zoom="1.5"/>
        <in key="transform" uid="..." auto_connect="true"/>
        <in key="view_up" uid="..." />
        <positions>
            <position name="pos1" rx="-30.0" />
            <position name="pos2" rx="-30.0" ry="90.0" />
            <position name="pos3" rx="-30.0" ry="-90.0"/>
        </positions>
        <properties position="pos3" follow_orientation="false" />
   </service>
   @endcode
 *
 *
 * @subsection Input Input:
 * - \b transform (optional): initial transform (registration, tracking, ...) to apply to the adaptor first.
 * - \b view_up (optional): used to extract the up vector of the camera. We only use the Y axis of this transform.
 *
 * @subsection Configuration Configuration:
 * - \b priority (optional, int, default=0): interaction priority, higher priority interactions are performed first.
 * - \b layerOrderDependant (optional, bool, default=true): defines if interaction must take into account above layers.
 * - \b mouseRotation (optional, bool, default=true): defines if mouse rotation through mouse is activated or not.
 * - \b animate (optional, bool, default=true): defines if an animation is used when switching position or not.
 * - \b zoom (optional, default="1.0"): defines the zoom ratio against the size of the scene.
 *
 * @subsection Properties Properties:
 * - \b position (optional, string, default=""): defines the default position to use.
 * - \b follow_orientation (optional, bool, default=false) defines if we use a fixed orientation
 *      or if we follow the orientation of the target.
 *
 * @section Slots Slots
 * - \b reset: Reset the point of view.
 * - \b set_transform: Update adaptor using the input transformation.
 * - \b set_position: looking for value in the position name, then goes to that position if found.
 * - \b next_position: Go to the next position (cyclic iteration).
 * - \b prev_position: Go to the previous position (cyclic iteration).
 */
class predefined_camera final : public sight::viz::scene3d::adaptor
{
public:

    struct slots
    {
        using slots_t = core::com::slots::key_t;

        inline static const slots_t RESET         = "reset";
        inline static const slots_t SET_TRANSFORM = "set_transform";
        inline static const slots_t SET_POSITION  = "set_position";
        inline static const slots_t NEXT_POSITION = "next_position";
        inline static const slots_t PREV_POSITION = "prev_position";
    };

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(predefined_camera, sight::viz::scene3d::adaptor);

    /// Constructor.
    predefined_camera() noexcept;

    /// Destroys the adaptor.
    ~predefined_camera() noexcept final = default;

protected:

    void reset();

    /// Configures the layer and the interaction priority.
    void configuring() final;

    /// Adds trackball interactions to the layer.
    void starting() final;

    /// Updates the service. Unused here.
    void updating() noexcept final;

    /// Removes trackball interactions from the layer.
    void stopping() final;

private:

    connections_t auto_connections() const final;

    enum class update_flags : std::uint8_t
    {
        RESET,
        SET_TRANSFORM,
        SET_POSITION,
        NEXT_POSITION,
        PREV_POSITION,
        _NUM
    };

    using predefined_position_t =
        sight::viz::scene3d::interactor::predefined_position_interactor::predefined_position_t;

    std::vector<predefined_position_t> m_camera_positions;

    /// Contains the interaction handler.
    std::shared_ptr<sight::viz::scene3d::interactor::predefined_position_interactor> m_interactor;

    /// Defines the interaction priority.
    int m_priority {0};

    /// Defines if the interaction must take into account above layers.
    bool m_layer_order_dependant {true};

    /// Defines if one can move using the mouse interaction (default off).
    bool m_manual_rotation {false};

    /// Defines if an animation is performed when switching positions.
    bool m_animate {true};

    /// Zoom ratio.
    float m_zoom {1.0};

    /// Input transform.
    sight::data::ptr<sight::data::matrix4, sight::data::access::in> m_transform {this, "transform", true};

    /// Matrix used to extract the up vector of the camera. We only use the Y axis of this transform.
    sight::data::ptr<sight::data::matrix4, sight::data::access::in> m_view_up {this, "view_up", true};

    /// Defines if we use a fixed orientation or if we follow the orientation of the target.
    sight::data::property<sight::data::boolean> m_follow_orientation {this, "follow_orientation", false};

    /// Defines if we use a fixed orientation or if we follow the orientation of the target.
    sight::data::property<sight::data::string> m_position {this, "position", {}};
};

} // namespace sight::module::viz::scene3d::adaptor.
