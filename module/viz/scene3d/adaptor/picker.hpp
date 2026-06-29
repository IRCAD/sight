/************************************************************************
 *
 * Copyright (C) 2019-2026 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include <core/com/signal.hpp>

#include <data/tools/picking_info.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/interactor/base.hpp>

#include <array>
#include <cstdint>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor performs picking queries on mouse clicks and forwards the picked data through a signal.
 *
 * Lets the user click on the scene to perform picking queries. Emits a signal with the relevant intersection data
 * when picking succeeds.
 *
 * @section Signals Signals
 * - \b picked(data::tools::picking_info): emitted when a picking query is successful.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="sight::module::viz::scene3d::adaptor::picker">
            <config priority="0" queryMask="0xFFFFFFFF" />
       </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b priority (optional, int, default=0): picking priority, higher priority interactions are performed first.
 * - \b queryMask (optional, uint32, default=0xFFFFFFFF): filters out entities with mismatching flags when picking.
 */
class picker final : public sight::viz::scene3d::adaptor,
                     public sight::viz::scene3d::interactor::base
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(picker, sight::viz::scene3d::adaptor);

    struct signals
    {
        using point_clicked_t = core::com::signal<void (data::tools::picking_info)>;
        static inline const signal_key_t PICKED = "picked";
    };

    /// Initializes the adaptor.
    picker() noexcept;

    /// Destroys the adaptor.
    ~picker() noexcept final = default;

    /// Runs a picking query when a mouse button is released.
    void button_release_event(
        sight::viz::scene3d::interactor::base::mouse_button _button,
        modifier _mods,
        int _x,
        int _y
    ) final;

    /// Runs a picking query when a mouse button is pressed.
    void button_press_event(
        sight::viz::scene3d::interactor::base::mouse_button _button,
        modifier _mods,
        int _x,
        int _y
    ) final;

    /// Runs a picking query if a mouse button was pressed beforehand.
    void mouse_move_event(
        mouse_button _button,
        modifier _mods,
        int _x,
        int _y,
        int _dx,
        int _dy
    ) final;

protected:

    /// Configures the picker's query mask and priority.
    void configuring() final;

    /// Creates the interactor and adds it to the layer.
    void starting() final;

    /// Updates the service. Doesn't do anything here.
    void updating() noexcept final;

    /// Destroys the interactor and removes it from the layer.
    void stopping() final;

private:

    /**
     * @brief Triggers a picking query and sends a signal with the corresponding data::tools::picking_info.
     *
     * @param _button mouse button pressed.
     * @param _mods keyboard modifiers.
     * @param _x width coordinate of the mouse.
     * @param _y height coordinate of the mouse.
     * @param _pressed whether the button is pressed (true) or released (false).
     */
    void pick(mouse_button _button, modifier _mods, int _x, int _y, bool _pressed);

    /// Determines the execution order of the picking interactor.
    int m_priority {2};

    /// Defines the mask used to filter out entities when picking.
    std::uint32_t m_query_mask {0xFFFFFFFF};

    /// Mouse button press states
    std::array<bool, sight::viz::scene3d::interactor::base::mouse_button::num_values> m_pressed {false, false, false,
                                                                                                 false, false
    };
};

} // namespace sight::module::viz::scene3d::adaptor
