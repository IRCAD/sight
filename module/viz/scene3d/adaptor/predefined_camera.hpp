/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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
#include <data/string.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/interactor/base.hpp>

#include <optional>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor lets the user move the camera around a point of interest using predefined positions.
 *
 * Lets the user interact with predefined camera positions. Camera can be moved around a set of predefined
 * positions using keyboard keys or slots.
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
class predefined_camera final : public sight::viz::scene3d::adaptor,
                                public sight::viz::scene3d::interactor::base
{
public:

    struct slots
    {
        inline static const slot_key_t RESET         = "reset";
        inline static const slot_key_t SET_TRANSFORM = "set_transform";
        inline static const slot_key_t SET_POSITION  = "set_position";
        inline static const slot_key_t NEXT_POSITION = "next_position";
        inline static const slot_key_t PREV_POSITION = "prev_position";
    };

    /// Predefined position with name and Rx, Ry, Rz rotation in degree.
    struct predefined_position_t
    {
        float rx {0.F};
        float ry {0.F};
        float rz {0.F};
        std::string name;
    };

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(predefined_camera, sight::viz::scene3d::adaptor);

    /// Constructor.
    predefined_camera() noexcept;

    /// Destroys the adaptor.
    ~predefined_camera() noexcept final;

    /**
     * @brief Moves the camera according to the pressed button.
     * @param _button mouse button pressed. Defines the following behaviour:
     * - LEFT: Rotates the camera around the focus point.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     * @param _dx the cursor's width displacement since the last event.
     * @param _dy the cursor's height displacement since the last event.
     */
    void mouse_move_event(
        mouse_button _button,
        modifier /*_mods*/,
        int _x,
        int _y,
        int _dx,
        int _dy
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
     * @brief Defines camera actions when the keyboard is pressed.
     * @param _key pressed key code. Defines the following behaviour:
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

    /// Sets a position according to its name.
    void set_position(const std::string& _value);

    /// Goes to the next predefined position (return to first one after the latest).
    void next_position();
    /// Goes to the previous predefined position (return to last one after the first).
    void previous_position();

    inline void set_view_up(Ogre::Vector3 _view_up);

    /// Returns current state of mouse rotation.
    [[nodiscard]] inline bool mouse_rotation() const;

    inline void set_transform(const Ogre::Matrix4& _mat);

    [[nodiscard]] inline Ogre::Matrix4 transform() const;

    [[nodiscard]] inline Ogre::Quaternion transform_quaternion() const;

    /// Resets the camera.
    void reset();

    connections_t auto_connections() const final;

    /// Configures the layer and the interaction priority.
    void configuring() final;

    /// Adds camera interactions to the layer.
    void starting() final;

    /// Updates the service.
    void updating() noexcept final;

    /// Removes camera interactions from the layer.
    void stopping() final;

private:

    enum class update_flags : std::uint8_t
    {
        reset,
        set_transform,
        set_position,
        next_position,
        prev_position
    };

    /// Internal function to init the rotation to a known place.
    void init();

    /// Internal function to rotate the camera in place (remove zoom level).
    void rotate_camera(Ogre::SceneNode* _cam, const Ogre::Quaternion& _quat) const;

    /// Internal function to update camera position & orientation using m_transform (see @set_transform).
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

    std::vector<predefined_position_t> m_camera_positions;

    /// Defines the interaction priority.
    int m_priority {0};

    /// Defines if the interaction must take into account above layers.
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

    /// Defines the default position to use.
    sight::data::property<sight::data::string> m_position {this, "position", {}};

    // Interactor members

    /// Current distance from the camera to the point of interest.
    float m_look_at_z {300.F};

    /// Scale applied to mouse events.
    float m_mouse_scale {1.F};

    /// Current zoom factor.
    float m_zoom_factor {1.F};

    /// Default mouse scale factor (used to move the camera)
    static constexpr int MOUSE_SCALE_FACTOR = 200;

    /// Timer used to animate the camera.
    core::thread::timer::sptr m_timer;

    /// Stores the initial rotation.
    const Ogre::Quaternion m_camera_init_rotation {Ogre::Quaternion(Ogre::Degree(180), Ogre::Vector3::NEGATIVE_UNIT_X)};

    Ogre::Quaternion m_last_orientation;

    /// Defines if an animation is performed when switching positions
    bool m_animate_interactor {true};

    /// Defines if we use a fixed orientation or if we follow the orientation of the target
    bool m_follow_orientation_interactor {false};

    /// Stores the current index in m_camera_positions.
    std::optional<std::size_t> m_current_position_idx {std::nullopt};

    Ogre::Matrix4 m_transform_matrix {Ogre::Matrix4::IDENTITY};

    /// Up vector of the camera, required to orientate the interactor
    Ogre::Vector3 m_view_up_vector;

    /// Current percentage for animation
    float m_percentage {0.F};

    /// Zoom ratio
    float m_zoom_config {1.0};

    /// last time step
    std::chrono::system_clock::time_point m_last_step_time;
};

//------------------------------------------------------------------------------

void inline predefined_camera::set_view_up(Ogre::Vector3 _view_up)
{
    m_view_up_vector = _view_up;
}

//------------------------------------------------------------------------------

void inline predefined_camera::set_transform(const Ogre::Matrix4& _mat)
{
    m_transform_matrix = _mat;
    this->follow_transform();
}

//------------------------------------------------------------------------------

Ogre::Matrix4 inline predefined_camera::transform() const
{
    return m_transform_matrix;
}

//------------------------------------------------------------------------------

Ogre::Quaternion inline predefined_camera::transform_quaternion() const
{
    return {m_transform_matrix.linear()};
}

} // namespace sight::module::viz::scene3d::adaptor
