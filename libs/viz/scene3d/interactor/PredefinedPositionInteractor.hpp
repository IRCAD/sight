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

#include "viz/scene3d/config.hpp"
#include "viz/scene3d/interactor/base.hpp"
#include "viz/scene3d/Layer.hpp"

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
class VIZ_SCENE3D_CLASS_API PredefinedPositionInteractor final : public sight::viz::scene3d::interactor::base
{
public:

    /// Predefined position with name and Rx, Ry,Rz rotation in degree.
    struct predefined_position_t
    {
        float rx {0.F};
        float ry {0.F};
        float rz {0.F};
        std::string name {""};
    };

    /// Initializes the interractor.
    VIZ_SCENE3D_API PredefinedPositionInteractor(
        SPTR(Layer)_layer                                   = nullptr,
        bool _layerOrderDependant                           = true,
        std::vector<predefined_position_t> _positions       = {},
        const std::optional<std::string>& _default_position = std::nullopt,
        bool _animate                                       = true
    );

    /// Destroys the trackball.
    VIZ_SCENE3D_API ~PredefinedPositionInteractor() override;

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
    ) override;

    /**
     * @brief Moves the camera towards or away from the focus point.
     * @param _delta distance that the wheel is rotated, in eighths of a degree.
     */
    VIZ_SCENE3D_API void wheelEvent(Modifier /*_mods*/, double _delta, int /*x*/, int /*y*/) override;

    /**
     * @brief Moves the camera towards or away the central point.
     * @param _scalingFactor distance of the fingers
     * @param _centerX the width coordinate of the center of the pinch
     * @param _centerY the height coordinate of the center of the pinch
     */
    VIZ_SCENE3D_API void pinchGestureEvent(double _scaleFactor, int _centerX, int _centerY) override;

    /**
     * @brief Defines camera actions when the keyboard is pressed.
     * @param _key pressed key code. Defines the following behaviour:
     * - 'A' or 'a': animates the camera to rotate around the focus point.
     * - 'R' or 'r': moves the camera backwards to see the whole scene.
     * @param _mouseX the mouse's width position at the time of the key press.
     * @param _mouseY the mouse's height position at the time of the key press.
     */
    VIZ_SCENE3D_API void keyPressEvent(int _key, Modifier /*_mods*/, int _mouseX, int _mouseY) override;

    /**
     * @brief Recomputes the camera's aspect ratio when the render window is resized.
     */
    VIZ_SCENE3D_API void resizeEvent(int /*_width*/, int /*_height*/) override;

    /// Recomputes the mouse's scale and focus point from the updated scene length.
    VIZ_SCENE3D_API void setSceneLength(float _sceneLength) override;

    /// SLOT: Sets a parameter value with its key.
    VIZ_SCENE3D_API void setParameter(ui::parameter_t value, std::string key);

    ///SLOT: Goes to the next predefined position (return to first one after the latest).
    VIZ_SCENE3D_API void nextPosition();
    ///SLOT: Goes to the previous predefined position (return to last one after the first).
    VIZ_SCENE3D_API void previousPosition();

    /// Enables/disables the mouse rotation.
    VIZ_SCENE3D_API void inline setMouseRotation(bool _enable);

    /// Returns current state of mouse rotation.
    VIZ_SCENE3D_API bool inline mouseRotation() const;

    VIZ_SCENE3D_API void inline setTransform(const Ogre::Matrix4& _mat);

    VIZ_SCENE3D_API Ogre::Matrix4 inline transform() const;

    VIZ_SCENE3D_API Ogre::Quaternion inline transformQuaternion() const;

private:

    /// Internal function to init the rotation to a known place.
    void init();

    /// Internal function to rotate the camera in place (remove zoom level).
    void rotateCamera(Ogre::SceneNode* const _cam, const Ogre::Quaternion& _quat) const;

    /// Internal function to update camera position & orientation using m_transform (see @setTransform).
    void followTransform();

    /**
     * @brief The camera's scene node will rotate around its point of interest (lookAt).
     * @param dx The mouse's X displacement
     * @param dy The mouse's Y displacement
     */
    void cameraRotateByMouse(int dx, int dy);

    /**
     * @brief Goes to the predefined position at index _idx
     * Does nothing if _idx >= vector size
     * @param _idx : the index in the vector of predefined positions
     */
    void toPredefinedPosition(std::size_t _idx, bool _animate = true);

    /// Resets the camera's focal length when the focus point changes.
    void updateCameraFocalLength();

    /// Current distance from the camera to the point of interest.
    float m_lookAtZ {300.F};

    /// Scale applied to mouse events.
    float m_mouseScale {1.F};

    /// Current zoom factor.
    float m_zoom {1.F};

    /// Default mouse scale factor (used to move the camera)
    static constexpr int MOUSE_SCALE_FACTOR = 200;

    /// Enables/disables mouse move events.
    bool m_mouseMove {false};

    /// Timer used to animate the camera.
    core::thread::timer::sptr m_timer;

    /// Stores the initial rotation.
    const Ogre::Quaternion m_cameraInitRotation {Ogre::Quaternion(Ogre::Degree(180), Ogre::Vector3::NEGATIVE_UNIT_X)};

    Ogre::Quaternion m_currentOrientation;

    Ogre::Quaternion m_lastOrientation;

    /// Vector of predefined position to loop.
    std::vector<predefined_position_t> m_predefined_positions;

    /// Defines if an animation is performed when switching positions
    bool m_animate {true};

    /// Stores the current index in m_predefined_positions.
    std::optional<std::size_t> m_current_position_idx {std::nullopt};

    Ogre::Matrix4 m_transform {Ogre::Matrix4::IDENTITY};

    /// Curent percentage for animation
    float m_percentage {0.F};

    /// last time step
    std::chrono::system_clock::time_point m_last_step_time {};
};

//------------------------------------------------------------------------------

void inline PredefinedPositionInteractor::setMouseRotation(bool _enable)
{
    m_mouseMove = _enable;
}

//------------------------------------------------------------------------------

bool inline PredefinedPositionInteractor::mouseRotation() const
{
    return m_mouseMove;
}

//------------------------------------------------------------------------------

void inline PredefinedPositionInteractor::setTransform(const Ogre::Matrix4& _mat)
{
    m_transform = _mat;
    this->followTransform();
}

//------------------------------------------------------------------------------

Ogre::Matrix4 PredefinedPositionInteractor::transform() const
{
    return m_transform;
}

//------------------------------------------------------------------------------

Ogre::Quaternion PredefinedPositionInteractor::transformQuaternion() const
{
    return Ogre::Quaternion(m_transform.linear());
}

} // namespace sight::viz::scene3d::interactor
