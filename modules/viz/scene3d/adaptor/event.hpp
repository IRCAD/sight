/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include <viz/scene3d/adaptor.hpp>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor echoes the input events its scene got. The input events may be filtered in the configuration.
 *
 * @section Signals Signals
 * - \b triggered(sight::viz::scene3d::window_interactor::interaction_info): An event was triggered
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::event">
        <event type="buttonDoublePress buttonPress" buttons="left" modifiers="none"/>
        <event type="keyPress" keys="0x20" modifiers="ctrl"/> <!-- space -->
    </service>
   @endcode
 * The snippet above will create a event which will sends signal when the left mouse button is simple-clicked or
 * double-clicked without modifiers, and when the space key of the keyboard is pressed while holding the Control key.
 *
 * @subsection Configuration Configuration:
 * - \b event
 *   - \b type (string): A space-separated list of events which will trigger a signal. Might be: "mouseMove", "wheel",
 *             "buttonRelease", "buttonPress", "buttonDoublePress", "keyPress", "keyRelease", "resize", pinchGesture",
 *             "panGestureMove", "panGestureRelease", "longTapGesture", "pan2GestureMove" or "pan2GestureRelease".
 *   - \b buttons (optional, string): A space-separated list of mouse buttons to be held during the event. Might be
 *                "left", "middle" or "right". Only valid with event types "mouseMove", "buttonRelease", "buttonPress"
 *                and "buttonDoublePress".
 *   - \b modifiers (optional, string): A space-separated list of keyboard modifiers to be held. Only valid with event
 *                  types "mouseMove", "wheel", "buttonRelease", "buttonPress", "buttonDoublePress", "keyPress" and
 *                  "keyRelease". Might be "shift", "ctrl", "alt" or "meta".
 *   - \b keys (optional, string): A space-separated list of keys to be clicked as key codes. Only valid with event
 *             types "keyPress" and "keyRelease". @see https://doc.qt.io/qt-5/qt.html#Key-enum
 */
class event final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::interactor::base
{
public:

    SIGHT_DECLARE_SERVICE(event, sight::viz::scene3d::adaptor);

    event();

    void mouse_move_event(
        mouse_button _button,
        modifier _mods,
        int _x,
        int _y,
        int _dx,
        int _dy
    ) final;
    void wheel_event(modifier _mods, double _angle_delta, int _x, int _y) final;
    void button_release_event(mouse_button _button, modifier _mods, int _x, int _y) final;
    void button_press_event(mouse_button _button, modifier _mods, int _x, int _y) final;
    void button_double_press_event(mouse_button _button, modifier _mods, int _x, int _y) final;
    void key_press_event(int _key, modifier _mods, int _mouse_x, int _mouse_y) final;
    void key_release_event(int _key, modifier _mods, int _mouse_x, int _mouse_y) final;
    void resize_event(int _width, int _height) final;
    void pinch_gesture_event(double _scale_factor, int _center_x, int _center_y) final;
    void pan_gesture_move_event(int _x, int _y, int _dx, int _dy) final;
    void pan_gesture_release_event(int _x, int _y, int _dx, int _dy) final;
    void long_tap_gesture_event(int _x, int _y) final;

    static const core::com::signals::key_t TRIGGERED;
    using triggered_signal_t = core::com::signal<void (sight::viz::scene3d::window_interactor::interaction_info)>;

private:

    struct filter
    {
        std::vector<sight::viz::scene3d::window_interactor::interaction_info::interaction_enum> type;
        std::uint8_t buttons = 0;
        std::optional<modifier> modifiers;
        std::vector<int> keys;
    };

    bool check(
        sight::viz::scene3d::window_interactor::interaction_info::interaction_enum _type,
        std::optional<mouse_button> _button,
        std::optional<modifier> _modifiers,
        std::optional<int> _key
    );

    void configuring() final;
    void starting() final;
    void updating() final;
    void stopping() final;

    std::vector<filter> m_filters;
};

} // namespace sight::module::viz::scene3d::adaptor.
