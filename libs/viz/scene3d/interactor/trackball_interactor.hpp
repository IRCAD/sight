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

#include "viz/scene3d/config.hpp"
#include "viz/scene3d/interactor/base.hpp"

#include <core/thread/timer.hpp>

namespace sight::viz::scene3d::interactor
{

/**
 * @brief Trackball camera interactions.
 *
 * Lets the user move the camera around a point of interest when moving the mouse.
 *
 * @todo make the class final once VRWidgetInteractor is deleted.
 */
class VIZ_SCENE3D_CLASS_API trackball_interactor final : public sight::viz::scene3d::interactor::base
{
public:

    /// Initializes the trackball.
    VIZ_SCENE3D_API trackball_interactor(SPTR(Layer)_layer = nullptr, bool _layerOrderDependant = true);

    /// Destroys the trackball.
    VIZ_SCENE3D_API ~trackball_interactor() final;

    /**
     * @brief Moves the camera according to the pressed button.
     * @param _button mouse button pressed. Defines the following behaviour:
     * - LEFT: Rotates the camera around the focus point.
     * - MIDDLE: Moves the camera along the screen's axes.
     * - RIGHT: Moves the camera along the forward axis.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     * @param _dx the cursor's width displacement since the last event.
     * @param _dy the cursor's height displacement since the last event.
     *
     * @todo set to final when the vr interactor is deleted.
     */
    VIZ_SCENE3D_API void mouseMoveEvent(
        MouseButton _button,
        Modifier /*_mods*/,
        int _x,
        int _y,
        int _dx,
        int _dy
    ) final;

    /// Verifies if the button is pressed within the camera's viewport and enables mouse movements if that is the case.
    VIZ_SCENE3D_API void buttonPressEvent(MouseButton _button, Modifier /*_mods*/, int _x, int _y) final;

    /// Disables mouse movements.
    VIZ_SCENE3D_API void buttonReleaseEvent(MouseButton _button, Modifier /*_mods*/, int /*_x*/, int /*_y*/) final;

    /**
     * @brief Moves the camera towards or away from the focus point.
     * @param _delta distance that the wheel is rotated, in eighths of a degree.
     */
    VIZ_SCENE3D_API void wheelEvent(Modifier /*_mods*/, double _delta, int /*x*/, int /*y*/) final;

    /**
     * @brief Moves the camera towards or away the central point.
     * @param _scalingFactor distance of the fingers
     * @param _centerX the width coordinate of the center of the pinch
     * @param _centerY the height coordinate of the center of the pinch
     */
    VIZ_SCENE3D_API void pinchGestureEvent(double _scaleFactor, int _centerX, int _centerY) final;

    /**
     * @brief Moves the camera along the screen's axes.
     * @param _x current width coordinate.
     * @param _y current height coordinate.
     * @param _dx the width displacement since the last event.
     * @param _dy the height displacement since the last event.
     */
    VIZ_SCENE3D_API void panGestureMoveEvent(int _x, int _y, int _dx, int _dy) final;

    /**
     * @brief Defines camera actions when the keyboard is pressed.
     * @param _key pressed key code. Defines the following behaviour:
     * - 'A' or 'a': animates the camera to rotate around the focus point.
     * - 'R' or 'r': moves the camera backwards to see the whole scene.
     * @param _mouseX the mouse's width position at the time of the key press.
     * @param _mouseY the mouse's height position at the time of the key press.
     */
    VIZ_SCENE3D_API void keyPressEvent(int _key, Modifier /*_mods*/, int _mouseX, int _mouseY) final;

    /**
     * @brief Recomputes the camera's aspect ratio when the render window is resized.
     */
    VIZ_SCENE3D_API void resizeEvent(int /*_width*/, int /*_height*/) final;

    /// Recomputes the mouse's scale and focus point from the updated scene length.
    VIZ_SCENE3D_API void setSceneLength(float _sceneLength) final;

private:

    /**
     * @brief The camera's scene node will rotate around its point of interest (lookAt).
     * @param dx The mouse's X displacement
     * @param dy The mouse's Y displacement
     */
    void cameraRotate(int /*dx*/, int /*dy*/);

    /**
     * @brief The camera's scene node will translate along its local vertical and horizontal axes.
     * @param dx The horizontal displacement
     * @param dy The vertical displacement
     */
    void cameraTranslate(int /*x_move*/, int /*y_move*/);

    /// Resets the camera's focal length when the focus point changes.
    void updateCameraFocalLength();

    /// Current distance from the camera to the point of interest.
    float m_lookAtZ {1.F};

    /// Scale applied to mouse events.
    float m_mouseScale {1.F};

    /// Current zoom factor.
    float m_zoom {1.F};

    /// Default mouse scale factor (used to move the camera)
    static constexpr int MOUSE_SCALE_FACTOR = 200;

    /// Animate the camera to rotate around the trackball center.
    bool m_animate {false};

    /// Enables/disables mouse move events.
    bool m_mouseMove {false};

    /// Timer used to animate the camera.
    core::thread::timer::sptr m_timer;
};

} // namespace sight::viz::scene3d::interactor
