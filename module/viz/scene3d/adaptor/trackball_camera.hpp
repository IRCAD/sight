/************************************************************************
 *
 * Copyright (C) 2019-2026 IRCAD France
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

#include <core/thread/timer.hpp>

#include <data/matrix4.hpp>

#include <viz/scene3d/adaptor.hpp>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor lets the user move the camera around a point of interest using the mouse and keyboard.
 *
 * The camera is controlled via the following interactions:
 * - LEFT mouse button: Rotates the camera around the focus point.
 * - MIDDLE mouse button: Moves the camera along the screen's axes.
 * - RIGHT mouse button: Moves the camera along the forward axis.
 * - Mouse wheel: Moves the camera towards or away from the focus point.
 * - 'A' or 'a' key: Animates the camera to rotate around the focus point.
 * - 'R' or 'r' key: Moves the camera backwards to see the whole scene.
 *
 * @warning may not work as intended when used at the same time as `module::viz::scene3d::adaptor::Camera` on the same
 * layer.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::viz::scene3d::adaptor::trackball_camera">
        <config priority="0" />
        <data view_up="${...}" />
   </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b data.transform [sight::data::matrix4] (optional): initial transform (registration, tracking, ...) to apply to
 * the adaptor first.
 * - \b data.view_up [sight::data::matrix4] (optional): used to extract the up vector of the camera. We only use the Y
 * axis of this transform.
 *
 * @subsection Configuration Configuration:
 * - \b priority (optional, int, default=0): interaction priority, higher priority interactions are performed first.

 */
class trackball_camera final : public sight::viz::scene3d::adaptor,
                               public sight::viz::scene3d::interactor::base
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(trackball_camera, sight::viz::scene3d::adaptor);

    /// Constructor.
    trackball_camera() noexcept = default;

    /// Destroys the adaptor.
    ~trackball_camera() noexcept final;

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
     */
    void mouse_move_event(
        sight::viz::scene3d::interactor::base::mouse_button _button,
        modifier /*_mods*/,
        int _x,
        int _y,
        int _dx,
        int _dy
    ) final;

    /// Verifies if the button is pressed within the camera's viewport and enables mouse movements if that is the case.
    void button_press_event(
        sight::viz::scene3d::interactor::base::mouse_button _button,
        modifier /*_mods*/,
        int _x,
        int _y
    ) final;

    /// Disables mouse movements.
    void button_release_event(
        sight::viz::scene3d::interactor::base::mouse_button _button,
        modifier /*_mods*/,
        int /*_x*/,
        int /*_y*/
    ) final;

    /**
     * @brief Moves the camera towards or away from the focus point.
     * @param _delta distance that the wheel is rotated, in eighths of a degree.
     */
    void wheel_event(modifier /*_mods*/, double _delta, int _x, int _y) final;

    /**
     * @brief Moves the camera towards or away the central point.
     * @param _scale_factor distance of the fingers
     * @param _center_x the width coordinate of the center of the pinch
     * @param _center_y the height coordinate of the center of the pinch
     */
    void pinch_gesture_event(double _scale_factor, int _center_x, int _center_y) final;

    /**
     * @brief Moves the camera along the screen's axes.
     * @param _x current width coordinate.
     * @param _y current height coordinate.
     * @param _dx the width displacement since the last event.
     * @param _dy the height displacement since the last event.
     */
    void pan_gesture_move_event(int _x, int _y, int _dx, int _dy) final;

    /**
     * @brief Defines camera actions when the keyboard is pressed.
     * @param _key pressed key code. Defines the following behaviour:
     * - 'A' or 'a': animates the camera to rotate around the focus point.
     * - 'R' or 'r': moves the camera backwards to see the whole scene.
     * @param _mouse_x the mouse's width position at the time of the key press.
     * @param _mouse_y the mouse's height position at the time of the key press.
     */
    void key_press_event(int _key, modifier /*_mods*/, int _mouse_x, int _mouse_y) final;

    /**
     * @brief Recomputes the camera's aspect ratio when the render window is resized.
     */
    void resize_event(int /*_width*/, int /*_height*/) final;

    /// Recomputes the mouse's scale and focus point from the updated scene length.
    void set_scene_length(float _scene_length) final;

protected:

    /// Configures the layer and the interaction priority.
    void configuring() final;

    /// Adds trackball interactions to the layer.
    void starting() final;

    /// Updates the service. Unused here.
    void updating() noexcept final;

    /// Removes trackball interactions from the layer.
    void stopping() final;

private:

    /**
     * @brief The camera's scene node will rotate around its point of interest (lookAt).
     * @param _dx The mouse's X displacement
     * @param _dy The mouse's Y displacement
     */
    void camera_rotate(int /*dx*/, int /*dy*/);

    /**
     * @brief The camera's scene node will translate along its local vertical and horizontal axes.
     * @param _x_move The horizontal displacement
     * @param _y_move The vertical displacement
     */
    void camera_translate(int /*x_move*/, int /*y_move*/);

    /// Resets the camera's focal length when the focus point changes.
    void update_camera_focal_length();

    /// Current distance from the camera to the point of interest.
    float m_look_at_z {1.F};

    /// Scale applied to mouse events.
    float m_mouse_scale {1.F};

    /// Current zoom factor.
    float m_zoom {1.F};

    /// Default mouse scale factor (used to move the camera)
    static constexpr int MOUSE_SCALE_FACTOR = 200;

    /// Animate the camera to rotate around the trackball center.
    bool m_animate {false};

    /// Enables/disables mouse move events.
    bool m_mouse_move {false};

    /// Up vector of the camera, required to orientate the interactor
    Ogre::Vector3 m_view_up {sight::viz::scene3d::interactor::DEFAULT_VIEW_UP};

    /// Timer used to animate the camera.
    core::thread::timer::sptr m_timer;

    /// Defines the interaction priority.
    int m_priority {0};

    /// Matrix used to extract the up vector of the camera. We only use the Y axis of this transform.
    sight::data::ptr<sight::data::matrix4, sight::data::access::in> m_view_up_matrix {this, "data.view_up", true};
};

} // namespace sight::module::viz::scene3d::adaptor
