/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/interactor/IInteractor.hpp"

#include <core/include/core/thread/Timer.hpp>

namespace fwRenderOgre::interactor
{

/**
 * @brief Trackball camera interactions.
 *
 * Lets the user move the camera around a point of interest when moving the mouse.
 *
 * @todo make the class final once VRWidgetInteractor is deleted.
 */
class FWRENDEROGRE_CLASS_API TrackballInteractor final : public ::fwRenderOgre::interactor::IInteractor
{

public:

    /// Initializes the trackball.
    FWRENDEROGRE_API TrackballInteractor(SPTR(Layer)_layer = nullptr, bool _layerOrderDependant = true);

    /// Destroys the trackball.
    FWRENDEROGRE_API ~TrackballInteractor() override;

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
    FWRENDEROGRE_API void mouseMoveEvent(MouseButton _button, Modifier, int _x, int _y,
                                         int _dx, int _dy) override;

    /// Verifies if the button is pressed within the camera's viewport and enables mouse movements if that is the case.
    FWRENDEROGRE_API void buttonPressEvent(MouseButton _button, Modifier, int _x, int _y) override;

    /// Disables mouse movements.
    FWRENDEROGRE_API void buttonReleaseEvent(MouseButton _button, Modifier, int, int) override;

    /**
     * @brief Moves the camera towards or away from the focus point.
     * @param _delta distance that the wheel is rotated, in eighths of a degree.
     */
    FWRENDEROGRE_API void wheelEvent(Modifier, int _delta, int, int) override;

    /**
     * @brief Defines camera actions when the keyboard is pressed.
     * @param _key pressed key code. Defines the following behaviour:
     * - 'A' or 'a': animates the camera to rotate around the focus point.
     * - 'R' or 'r': moves the camera backwards to see the whole scene.
     * @param _mouseX the mouse's width position at the time of the key press.
     * @param _mouseY the mouse's height position at the time of the key press.
     */
    FWRENDEROGRE_API void keyPressEvent(int _key, Modifier, int _mouseX, int _mouseY) override;

    /**
     * @brief Recomputes the camera's aspect ratio when the render window is resized.
     */
    FWRENDEROGRE_API void resizeEvent(int, int) override;

    /// Recomputes the mouse's scale and focus point from the updated scene length.
    FWRENDEROGRE_API void setSceneLength(float _sceneLength) override;

private:

    /**
     * @brief The camera's scene node will rotate around its point of interest (lookAt).
     * @param dx The mouse's X displacement
     * @param dy The mouse's Y displacement
     */
    void cameraRotate(int, int);

    /**
     * @brief The camera's scene node will translate along its local vertical and horizontal axes.
     * @param dx The horizontal displacement
     * @param dy The vertical displacement
     */
    void cameraTranslate(int, int);

    /// Resets the camera's focal length when the focus point changes.
    void updateCameraFocalLength();

    /// Current distance from the camera to the point of interest.
    float m_lookAtZ;

    /// Scale applied to mouse events.
    float m_mouseScale;

    /// Current zoom factor.
    float m_zoom;

    /// Default mouse scale factor (used to move the camera)
    static constexpr int MOUSE_SCALE_FACTOR = 200;

    /// Animate the camera to rotate around the trackball center.
    bool m_animate { false };

    /// Enables/disables mouse move events.
    bool m_mouseMove { false };

    /// Timer used to animate the camera.
    core::thread::Timer::sptr m_timer;

};

} // namespace fwRenderOgre::interactor
