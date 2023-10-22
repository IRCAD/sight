/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include <core/com/has_signals.hpp>
#include <core/com/signal.hpp>
#include <core/com/signals.hpp>
#include <core/com/slot.hpp>
#include <core/com/slots.hpp>

#include <data/tools/picking_info.hpp>

#include <viz/scene3d/config.hpp>
#include <viz/scene3d/factory/new.hpp>

#include <OGRE/Ogre.h>
#include <OGRE/OgreSceneManager.h>

namespace sight::viz::scene3d
{

class layer;

namespace interactor
{

/**
 * @brief Interface implementation for all the interaction with the mouse
 */
class VIZ_SCENE3D_CLASS_API base
{
public:

    typedef std::weak_ptr<base> wptr;

    typedef std::shared_ptr<base> sptr;

    /// Defines the list of recognized mouse buttons.
    enum MouseButton
    {
        NONE,
        LEFT,
        MIDDLE,
        RIGHT,
        UNKNOWN
    };

    /// Defines the list of recognized mouse modifier.
    enum class Modifier : std::uint8_t
    {
        NONE    = 0x0,
        SHIFT   = 0x1,
        CONTROL = SHIFT << 1,
        META    = SHIFT << 2,
        ALT     = SHIFT << 3
    };

    /**
     * @brief Sets the scene manager from the layer if it exists.
     * @param _layer the layer on which the interator work.
     * @param _layerOrderDependant define if the interaction must take into account above layers.
     */
    VIZ_SCENE3D_API base(SPTR(layer)_layer = nullptr, bool _layer_order_dependant = true);

    /// Destroys the interactor.
    VIZ_SCENE3D_API virtual ~base();

    /// Sets the scene's length, that is the length of the scene's bounding cube.
    VIZ_SCENE3D_API virtual void setSceneLength(float _scene_length);

    /**
     * @brief Listens to mouse move events when one of the mouse buttons is pressed.
     * @param _button currently active mouse button.
     * @param _mods keyboard modifiers.
     * @param _x width coordinate of the mouse.
     * @param _y height coordinate of the mouse.
     * @param _dx width displacement of the mouse since the last event.
     * @param _dx height displacement of the mouse since the last event.
     */
    VIZ_SCENE3D_API virtual void mouseMoveEvent(MouseButton _button, Modifier _mods, int _x, int _y, int _dx, int _dy);

    /**
     * @brief Listens to the mouse's wheel being spun.
     * @param _mods keyboard modifiers.
     * @param _angleDelta distance that the wheel is rotated, in eighths of a degree.
     *                    See @ref https://doc.qt.io/qt-5/qwheelevent.html#angleDelta
     * @param _x width coordinate of the mouse.
     * @param _y height coordinate of the mouse.
     */
    VIZ_SCENE3D_API virtual void wheelEvent(Modifier _mods, double _angle_delta, int _x, int _y);

    /**
     * @brief Listens to mouse move buttons being released.
     * @param _button released mouse button.
     * @param _mods keyboard modifiers.
     * @param _x width coordinate of the mouse.
     * @param _y height coordinate of the mouse.
     */
    VIZ_SCENE3D_API virtual void buttonReleaseEvent(MouseButton _button, Modifier _mods, int _x, int _y);

    /**
     * @brief Listens to mouse move buttons being pressed.
     * @param _button pressed mouse button.
     * @param _mods keyboard modifiers.
     * @param _x width coordinate of the mouse.
     * @param _y height coordinate of the mouse.
     */
    VIZ_SCENE3D_API virtual void buttonPressEvent(MouseButton _button, Modifier _mods, int _x, int _y);

    /**
     * @brief Listens to mouse buttons being double pressed.
     * @param _button pressed mouse button.
     * @param _mods keyboard modifiers.
     * @param _x width coordinate of the mouse.
     * @param _y height coordinate of the mouse.
     */
    VIZ_SCENE3D_API virtual void buttonDoublePressEvent(MouseButton _button, Modifier _mods, int _x, int _y);

    /**
     * @brief Listens to keyboard keys being pressed.
     * @param _key pressed key.
     * @param _mods keyboard modifiers.
     * @param _x width coordinate of the mouse at the time of the event.
     * @param _y height coordinate of the mouse at the time of the event.
     */
    VIZ_SCENE3D_API virtual void keyPressEvent(int _key, Modifier _mods, int _mouse_x, int _mouse_y);

    /**
     * @brief Listens to keyboard keys being released.
     * @param _key pressed key.
     * @param _mods keyboard modifiers.
     * @param _x width coordinate of the mouse at the time of the event.
     * @param _y height coordinate of the mouse at the time of the event.
     */
    VIZ_SCENE3D_API virtual void keyReleaseEvent(int _key, Modifier _mods, int _mouse_x, int _mouse_y);

    /**
     * @brief Called when the mouse leaves the widget
     */
    VIZ_SCENE3D_API virtual void leaveEvent();

    /**
     * @brief Called when the mouse enters the widget
     */
    VIZ_SCENE3D_API virtual void enterEvent();

    /**
     * @brief Checks if the cursor is inside a layer's viewport.
     * @param mouseX width coordinate of the mouse in pixels.
     * @param mouseY height coordinate of the mouse in pixels.
     * @param _layer layer on which to check the cursor's belonging.
     * @param _layerOrderDependant define if interaction must take into account above layers.
     */
    VIZ_SCENE3D_API static bool isInLayer(int _mouse_x, int _mouse_y, SPTR(layer) _layer, bool _layer_order_dependant);

    /**
     * @brief Listen to render window resize events.
     * @param _width the render window's new width.
     * @param _height the render window's new height.
     */
    VIZ_SCENE3D_API virtual void resizeEvent(int _width, int _height);

    /**
     * @brief Listens to pinch gesture events.
     * @param _scaleFactor the scale factor of the pinch
     *                     (See @ref https://doc.qt.io/qt-5/qpinchgesture.html#scaleFactor-prop)
     * @param _centerX the width coordinate of the center of the pinch
     * @param _centerY the height coordinate of the center of the pinch
     */
    VIZ_SCENE3D_API virtual void pinchGestureEvent(double _scale_factor, int _center_x, int _center_y);

    /**
     * @brief Listens to pan gesture move events.
     * @param _x the current width coordinate of the finger
     * @param _y the current height coordinate of the finger
     * @param _dx width displacement of the finger since the last event.
     * @param _dy height displacement of the finger since the last event.
     */
    VIZ_SCENE3D_API virtual void panGestureMoveEvent(int _x, int _y, int _dx, int _dy);

    /**
     * @brief Listens to pan gesture release events.
     * @param _x the last width coordinate of the finger
     * @param _y the last height coordinate of the finger
     * @param _dx width displacement of the finger since the last event.
     * @param _dy height displacement of the finger since the last event.
     */
    VIZ_SCENE3D_API virtual void panGestureReleaseEvent(int _x, int _y, int _dx, int _dy);

    /**
     * @brief Listens to long tap gesture events.
     * @param _x the width coordinate of the finger
     * @param _y the height coordinate of the finger
     */
    VIZ_SCENE3D_API virtual void longTapGestureEvent(int _x, int _y);

protected:

    /// Weak reference to the layer on which the interactor interacts.
    WPTR(layer) m_layer;

    /// Defines if the interaction must take into account above layers.
    bool m_layerOrderDependant {true};

private:

    /// Checks if the cursor is on top of the given viewport.
    static bool isInViewport(int _mouse_x, int _mouse_y, const Ogre::Viewport* _vp);
};

//------------------------------------------------------------------------------

template<typename INT_TYPE>
static inline bool operator==(INT_TYPE _i, base::Modifier _m)
{
    static_assert(std::is_integral<INT_TYPE>::value, "Integral type required.");
    return static_cast<INT_TYPE>(_m) == _i;
}

//------------------------------------------------------------------------------

template<typename INT_TYPE>
static inline bool operator==(base::Modifier _m, INT_TYPE _i)
{
    static_assert(std::is_integral<INT_TYPE>::value, "Integral type required.");
    return _i == static_cast<INT_TYPE>(_m);
}

//------------------------------------------------------------------------------

static inline base::Modifier operator&(base::Modifier _m1, base::Modifier _m2)
{
    return static_cast<base::Modifier>(
        static_cast<std::underlying_type<base::Modifier>::type>(_m1)
        & static_cast<std::underlying_type<base::Modifier>::type>(_m2)
    );
}

//------------------------------------------------------------------------------

static inline base::Modifier operator|(base::Modifier _m1, base::Modifier _m2)
{
    return static_cast<base::Modifier>(
        static_cast<std::underlying_type<base::Modifier>::type>(_m1)
        | static_cast<std::underlying_type<base::Modifier>::type>(_m2)
    );
}

//------------------------------------------------------------------------------

static inline base::Modifier operator|=(base::Modifier& _m1, base::Modifier _m2)
{
    return _m1 = _m1 | _m2;
}

} // namespace interactor.

} // namespace sight::viz::scene3d.
