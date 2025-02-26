/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include <sight/viz/scene3d/config.hpp>

#include <core/com/has_signals.hpp>
#include <core/com/signal.hpp>
#include <core/com/signals.hpp>
#include <core/com/slot.hpp>
#include <core/com/slots.hpp>

#include <data/tools/picking_info.hpp>

#include <viz/scene3d/factory/new.hpp>

#include <OGRE/Ogre.h>
#include <OGRE/OgreSceneManager.h>

namespace sight::viz::scene3d
{

class layer;

namespace interactor
{

static const inline Ogre::Vector3 DEFAULT_VIEW_UP(0., -1., 0.);

/**
 * @brief Interface implementation for all the interaction with the mouse
 */
class SIGHT_VIZ_SCENE3D_CLASS_API base
{
public:

    using wptr = std::weak_ptr<base>;

    using sptr = std::shared_ptr<base>;

    /// Defines the list of recognized mouse buttons.
    enum mouse_button
    {
        none,
        left,
        middle,
        right,
        unknown
    };

    /// Defines the list of recognized mouse modifier.
    enum class modifier : std::uint8_t
    {
        none    = 0x0,
        shift   = 0x1,
        control = shift << 1,
        meta    = shift << 2,
        alt     = shift << 3
    };

    /**
     * @brief Sets the scene manager from the layer if it exists.
     * @param _layer the layer on which the interator work.
     * @param _layer_order_dependant define if the interaction must take into account above layers.
     */
    SIGHT_VIZ_SCENE3D_API base(SPTR(layer)_layer = nullptr, bool _layer_order_dependant = true);

    /// Destroys the interactor.
    SIGHT_VIZ_SCENE3D_API virtual ~base();

    /// Sets the scene's length, that is the length of the scene's bounding cube.
    SIGHT_VIZ_SCENE3D_API virtual void set_scene_length(float _scene_length);

    /**
     * @brief Listens to mouse move events when one of the mouse buttons is pressed.
     * @param _button currently active mouse button.
     * @param _mods keyboard modifiers.
     * @param _x width coordinate of the mouse.
     * @param _y height coordinate of the mouse.
     * @param _dx width displacement of the mouse since the last event.
     * @param _dy height displacement of the mouse since the last event.
     */
    SIGHT_VIZ_SCENE3D_API virtual void mouse_move_event(
        mouse_button _button,
        modifier _mods,
        int _x,
        int _y,
        int _dx,
        int _dy
    );

    /**
     * @brief Listens to the mouse's wheel being spun.
     * @param _mods keyboard modifiers.
     * @param _angle_delta distance that the wheel is rotated, in eighths of a degree.
     *                     See @ref https://doc.qt.io/qt-5/qwheelevent.html#angleDelta
     * @param _x width coordinate of the mouse.
     * @param _y height coordinate of the mouse.
     */
    SIGHT_VIZ_SCENE3D_API virtual void wheel_event(modifier _mods, double _angle_delta, int _x, int _y);

    /**
     * @brief Listens to mouse move buttons being released.
     * @param _button released mouse button.
     * @param _mods keyboard modifiers.
     * @param _x width coordinate of the mouse.
     * @param _y height coordinate of the mouse.
     */
    SIGHT_VIZ_SCENE3D_API virtual void button_release_event(mouse_button _button, modifier _mods, int _x, int _y);

    /**
     * @brief Listens to mouse move buttons being pressed.
     * @param _button pressed mouse button.
     * @param _mods keyboard modifiers.
     * @param _x width coordinate of the mouse.
     * @param _y height coordinate of the mouse.
     */
    SIGHT_VIZ_SCENE3D_API virtual void button_press_event(mouse_button _button, modifier _mods, int _x, int _y);

    /**
     * @brief Listens to mouse buttons being double pressed.
     * @param _button pressed mouse button.
     * @param _mods keyboard modifiers.
     * @param _x width coordinate of the mouse.
     * @param _y height coordinate of the mouse.
     */
    SIGHT_VIZ_SCENE3D_API virtual void button_double_press_event(mouse_button _button, modifier _mods, int _x, int _y);

    /**
     * @brief Listens to keyboard keys being pressed.
     * @param _key pressed key.
     * @param _mods keyboard modifiers.
     * @param _mouse_x width coordinate of the mouse at the time of the event.
     * @param _mouse_y height coordinate of the mouse at the time of the event.
     */
    SIGHT_VIZ_SCENE3D_API virtual void key_press_event(int _key, modifier _mods, int _mouse_x, int _mouse_y);

    /**
     * @brief Listens to keyboard keys being released.
     * @param _key pressed key.
     * @param _mods keyboard modifiers.
     * @param _mouse_x width coordinate of the mouse at the time of the event.
     * @param _mouse_y height coordinate of the mouse at the time of the event.
     */
    SIGHT_VIZ_SCENE3D_API virtual void key_release_event(int _key, modifier _mods, int _mouse_x, int _mouse_y);

    /**
     * @brief Called when the mouse leaves the widget
     */
    SIGHT_VIZ_SCENE3D_API virtual void leave_event();

    /**
     * @brief Called when the mouse enters the widget
     */
    SIGHT_VIZ_SCENE3D_API virtual void enter_event();

    /**
     * @brief Checks if the cursor is inside a layer's viewport.
     * @param _mouse_x width coordinate of the mouse in pixels.
     * @param _mouse_y height coordinate of the mouse in pixels.
     * @param _layer layer on which to check the cursor's belonging.
     * @param _layer_order_dependant define if interaction must take into account above layers.
     */
    SIGHT_VIZ_SCENE3D_API static bool is_in_layer(int _mouse_x, int _mouse_y, SPTR(layer) _layer,
                                                  bool _layer_order_dependant);

    /**
     * @brief Listen to render window resize events.
     * @param _width the render window's new width.
     * @param _height the render window's new height.
     */
    SIGHT_VIZ_SCENE3D_API virtual void resize_event(int _width, int _height);

    /**
     * @brief Listens to pinch gesture events.
     * @param _scale_factor the scale factor of the pinch
     *                      (See @ref https://doc.qt.io/qt-5/qpinchgesture.html#scaleFactor-prop)
     * @param _center_x the width coordinate of the center of the pinch
     * @param _center_y the height coordinate of the center of the pinch
     */
    SIGHT_VIZ_SCENE3D_API virtual void pinch_gesture_event(double _scale_factor, int _center_x, int _center_y);

    /**
     * @brief Listens to pan gesture move events.
     * @param _x the current width coordinate of the finger
     * @param _y the current height coordinate of the finger
     * @param _dx width displacement of the finger since the last event.
     * @param _dy height displacement of the finger since the last event.
     */
    SIGHT_VIZ_SCENE3D_API virtual void pan_gesture_move_event(int _x, int _y, int _dx, int _dy);

    /**
     * @brief Listens to pan gesture release events.
     * @param _x the last width coordinate of the finger
     * @param _y the last height coordinate of the finger
     * @param _dx width displacement of the finger since the last event.
     * @param _dy height displacement of the finger since the last event.
     */
    SIGHT_VIZ_SCENE3D_API virtual void pan_gesture_release_event(int _x, int _y, int _dx, int _dy);

    /**
     * @brief Listens to long tap gesture events.
     * @param _x the width coordinate of the finger
     * @param _y the height coordinate of the finger
     */
    SIGHT_VIZ_SCENE3D_API virtual void long_tap_gesture_event(int _x, int _y);

protected:

    /// Weak reference to the layer on which the interactor interacts.
    WPTR(layer) m_layer;

    /// Defines if the interaction must take into account above layers.
    bool m_layer_order_dependant {true};

private:

    /// Checks if the cursor is on top of the given viewport.
    static bool is_in_viewport(int _mouse_x, int _mouse_y, const Ogre::Viewport* _vp);
};

//------------------------------------------------------------------------------

template<typename INT_TYPE>
static inline bool operator==(INT_TYPE _i, base::modifier _m)
{
    static_assert(std::is_integral<INT_TYPE>::value, "Integral type required.");
    return static_cast<INT_TYPE>(_m) == _i;
}

//------------------------------------------------------------------------------

template<typename INT_TYPE>
static inline bool operator==(base::modifier _m, INT_TYPE _i)
{
    static_assert(std::is_integral<INT_TYPE>::value, "Integral type required.");
    return _i == static_cast<INT_TYPE>(_m);
}

//------------------------------------------------------------------------------

static inline base::modifier operator&(base::modifier _m1, base::modifier _m2)
{
    return static_cast<base::modifier>(
        static_cast<std::underlying_type<base::modifier>::type>(_m1)
        & static_cast<std::underlying_type<base::modifier>::type>(_m2)
    );
}

//------------------------------------------------------------------------------

static inline base::modifier operator|(base::modifier _m1, base::modifier _m2)
{
    return static_cast<base::modifier>(
        static_cast<std::underlying_type<base::modifier>::type>(_m1)
        | static_cast<std::underlying_type<base::modifier>::type>(_m2)
    );
}

//------------------------------------------------------------------------------

static inline base::modifier operator|=(base::modifier& _m1, base::modifier _m2)
{
    return _m1 = _m1 | _m2;
}

} // namespace interactor.

} // namespace sight::viz::scene3d.
