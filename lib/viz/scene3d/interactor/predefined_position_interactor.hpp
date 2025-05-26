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

#include <sight/viz/scene3d/config.hpp>

#include "viz/scene3d/interactor/base.hpp"
#include "viz/scene3d/layer.hpp"

#include <core/thread/timer.hpp>

#include <ui/__/parameter.hpp>

#include <glm/glm.hpp>

namespace sight::viz::scene3d::interactor
{

/**
 * @brief Predefined camera interactions.
 *
 * Moves the camera around a set of predefined positions.
 *
 */
class SIGHT_VIZ_SCENE3D_CLASS_API predefined_position_interactor final : public sight::viz::scene3d::interactor::base
{
public:

    /// Predefined position with name and Rx, Ry, Rz rotation in degree.
    struct predefined_position_t
    {
        float rx {0.F};
        float ry {0.F};
        float rz {0.F};
        std::string name {""};
    };

    /// Initializes the interactor.
    SIGHT_VIZ_SCENE3D_API predefined_position_interactor(
        SPTR(layer)_layer,
        bool _layer_order_dependant,
        std::vector<predefined_position_t> _positions,
        const std::optional<std::string>& _default_position,
        bool _animate,
        bool _follow_orientation,
        float _zoom,
        const Ogre::Vector3& _view_up
    );

    /// Destroys the trackball.
    SIGHT_VIZ_SCENE3D_API ~predefined_position_interactor() override;

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
    SIGHT_VIZ_SCENE3D_API void mouse_move_event(
        mouse_button _button,
        modifier /*_mods*/,
        int _x,
        int _y,
        int _dx,
        int _dy
    ) override;

    /**
     * @brief Moves the camera towards or away from the focus point.
     * @param _delta distance that the wheel is rotated, in eighths of a degree.
     */
    SIGHT_VIZ_SCENE3D_API void wheel_event(modifier /*_mods*/, double _delta, int /*x*/, int /*y*/) override;

    /**
     * @brief Moves the camera towards or away the central point.
     * @param _scale_factor distance of the fingers
     * @param _center_x the width coordinate of the center of the pinch
     * @param _center_y the height coordinate of the center of the pinch
     */
    SIGHT_VIZ_SCENE3D_API void pinch_gesture_event(double _scale_factor, int _center_x, int _center_y) override;

    /**
     * @brief Defines camera actions when the keyboard is pressed.
     * @param _key pressed key code. Defines the following behaviour:
     * - 'A' or 'a': animates the camera to rotate around the focus point.
     * - 'R' or 'r': moves the camera backwards to see the whole scene.
     * @param _mouse_x the mouse's width position at the time of the key press.
     * @param _mouse_y the mouse's height position at the time of the key press.
     */
    SIGHT_VIZ_SCENE3D_API void key_press_event(int _key, modifier /*_mods*/, int _mouse_x, int _mouse_y) override;

    /**
     * @brief Recomputes the camera's aspect ratio when the render window is resized.
     */
    SIGHT_VIZ_SCENE3D_API void resize_event(int /*_width*/, int /*_height*/) override;

    /// Recomputes the mouse's scale and focus point from the updated scene length.
    SIGHT_VIZ_SCENE3D_API void set_scene_length(float _scene_length) override;

    /// SLOT: Sets a parameter value with its key.
    SIGHT_VIZ_SCENE3D_API void set_parameter(ui::parameter_t _value, std::string _key);

    ///SLOT: Goes to the next predefined position (return to first one after the latest).
    SIGHT_VIZ_SCENE3D_API void next_position();
    ///SLOT: Goes to the previous predefined position (return to last one after the first).
    SIGHT_VIZ_SCENE3D_API void previous_position();

    /// Enables/disables the mouse rotation.
    SIGHT_VIZ_SCENE3D_API void inline set_mouse_rotation(bool _enable);

    SIGHT_VIZ_SCENE3D_API void inline set_view_up(Ogre::Vector3 _view_up);

    /// Returns current state of mouse rotation.
    SIGHT_VIZ_SCENE3D_API bool inline mouse_rotation() const;

    SIGHT_VIZ_SCENE3D_API void inline set_transform(const Ogre::Matrix4& _mat);

    SIGHT_VIZ_SCENE3D_API Ogre::Matrix4 inline transform() const;

    SIGHT_VIZ_SCENE3D_API Ogre::Quaternion inline transform_quaternion() const;

    /// Resets the camera.
    SIGHT_VIZ_SCENE3D_API void reset();

private:

    /// Internal function to init the rotation to a known place.
    void init();

    /// Internal function to rotate the camera in place (remove zoom level).
    void rotate_camera(Ogre::SceneNode* const _cam, const Ogre::Quaternion& _quat) const;

    /// Internal function to update camera position & orientation using m_transform (see @setTransform).
    void follow_transform();

    /**
     * @brief The camera's scene node will rotate around its point of interest (lookAt).
     * @param _dx The mouse's X displacement
     * @param _dy The mouse's Y displacement
     */
    void camera_rotate_by_mouse(int _dx, int _dy);

    /**
     * @brief Goes to the predefined position at index _idx
     * Does nothing if _idx >= vector size
     * @param _idx : the index in the vector of predefined positions
     */
    void to_predefined_position(std::size_t _idx, bool _animate = true);

    /// Resets the camera's focal length when the focus point changes.
    void update_camera_focal_length();

    /// Current distance from the camera to the point of interest.
    float m_look_at_z {300.F};

    /// Scale applied to mouse events.
    float m_mouse_scale {1.F};

    /// Current zoom factor.
    float m_zoom {1.F};

    /// Default mouse scale factor (used to move the camera)
    static constexpr int MOUSE_SCALE_FACTOR = 200;

    /// Enables/disables mouse move events.
    bool m_mouse_move {false};

    /// Timer used to animate the camera.
    core::thread::timer::sptr m_timer;

    /// Stores the initial rotation.
    const Ogre::Quaternion m_camera_init_rotation {Ogre::Quaternion(Ogre::Degree(180), Ogre::Vector3::NEGATIVE_UNIT_X)};

    Ogre::Quaternion m_last_orientation;

    /// Vector of predefined position to loop.
    std::vector<predefined_position_t> m_predefined_positions;

    /// Defines if an animation is performed when switching positions
    bool m_animate {true};

    /// Defines if we use a fixed orientation or if we follow the orientation of the target
    bool m_follow_orientation {false};

    /// Stores the current index in m_predefined_positions.
    std::optional<std::size_t> m_current_position_idx {std::nullopt};

    Ogre::Matrix4 m_transform {Ogre::Matrix4::IDENTITY};

    /// Up vector of the camera, required to orientate the interactor
    Ogre::Vector3 m_view_up;

    /// Curent percentage for animation
    float m_percentage {0.F};

    /// Zoom ratio
    float m_zoom_config {1.0};

    /// last time step
    std::chrono::system_clock::time_point m_last_step_time {};
};

//------------------------------------------------------------------------------

void inline predefined_position_interactor::set_mouse_rotation(bool _enable)
{
    m_mouse_move = _enable;
}

//------------------------------------------------------------------------------

void inline predefined_position_interactor::set_view_up(Ogre::Vector3 _view_up)
{
    m_view_up = _view_up;
}

//------------------------------------------------------------------------------

bool inline predefined_position_interactor::mouse_rotation() const
{
    return m_mouse_move;
}

//------------------------------------------------------------------------------

void inline predefined_position_interactor::set_transform(const Ogre::Matrix4& _mat)
{
    m_transform = _mat;
    this->follow_transform();
}

//------------------------------------------------------------------------------

Ogre::Matrix4 predefined_position_interactor::transform() const
{
    return m_transform;
}

//------------------------------------------------------------------------------

Ogre::Quaternion predefined_position_interactor::transform_quaternion() const
{
    return Ogre::Quaternion(m_transform.linear());
}

} // namespace sight::viz::scene3d::interactor
