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

#include <viz/scene3d/adaptor.hpp>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor echoes the input events its scene got. The input events may be filtered in the configuration.
 *
 * @section Signals Signals
 * - \b triggered(sight::viz::scene3d::window_interactor::InteractionInfo): An event was triggered
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::SEvent">
        <event type="buttonDoublePress buttonPress" buttons="left" modifiers="none"/>
        <event type="keyPress" keys="0x20" modifiers="ctrl"/> <!-- space -->
    </service>
   @endcode
 * The snippet above will create a SEvent which will sends signal when the left mouse button is simple-clicked or
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
class MODULE_VIZ_SCENE3D_CLASS_API SEvent final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::interactor::base
{
public:

    SIGHT_DECLARE_SERVICE(SEvent, sight::viz::scene3d::adaptor);

    SEvent();

    MODULE_VIZ_SCENE3D_API void mouseMoveEvent(MouseButton button, Modifier mods, int x, int y, int dx, int dy) final;
    MODULE_VIZ_SCENE3D_API void wheelEvent(Modifier mods, double angleDelta, int x, int y) final;
    MODULE_VIZ_SCENE3D_API void buttonReleaseEvent(MouseButton button, Modifier mods, int x, int y) final;
    MODULE_VIZ_SCENE3D_API void buttonPressEvent(MouseButton button, Modifier mods, int x, int y) final;
    MODULE_VIZ_SCENE3D_API void buttonDoublePressEvent(MouseButton button, Modifier mods, int x, int y) final;
    MODULE_VIZ_SCENE3D_API void keyPressEvent(int key, Modifier mods, int mouseX, int mouseY) final;
    MODULE_VIZ_SCENE3D_API void keyReleaseEvent(int key, Modifier mods, int mouseX, int mouseY) final;
    MODULE_VIZ_SCENE3D_API void resizeEvent(int width, int height) final;
    MODULE_VIZ_SCENE3D_API void pinchGestureEvent(double scaleFactor, int centerX, int centerY) final;
    MODULE_VIZ_SCENE3D_API void panGestureMoveEvent(int x, int y, int dx, int dy) final;
    MODULE_VIZ_SCENE3D_API void panGestureReleaseEvent(int x, int y, int dx, int dy) final;
    MODULE_VIZ_SCENE3D_API void longTapGestureEvent(int x, int y) final;

    MODULE_VIZ_SCENE3D_API static const core::com::signals::key_t TRIGGERED;
    typedef core::com::signal<void (sight::viz::scene3d::window_interactor::InteractionInfo)> TriggeredSignal;

private:

    struct Filter
    {
        std::vector<sight::viz::scene3d::window_interactor::InteractionInfo::InteractionEnum> type;
        std::uint8_t buttons = 0;
        std::optional<Modifier> modifiers;
        std::vector<int> keys;
    };

    bool check(
        sight::viz::scene3d::window_interactor::InteractionInfo::InteractionEnum type,
        std::optional<MouseButton> button,
        std::optional<Modifier> modifiers,
        std::optional<int> key
    );

    MODULE_VIZ_SCENE3D_API void configuring() final;
    MODULE_VIZ_SCENE3D_API void starting() final;
    MODULE_VIZ_SCENE3D_API void updating() final;
    MODULE_VIZ_SCENE3D_API void stopping() final;

    std::vector<Filter> m_filters;
};

} // namespace sight::module::viz::scene3d::adaptor.
