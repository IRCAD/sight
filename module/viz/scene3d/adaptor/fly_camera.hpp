/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <data/matrix4.hpp>

#include <viz/scene3d/adaptor.hpp>

#include <OgreQuaternion.h>
#include <OgreVector3.h>

#include <atomic>
#include <mutex>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief Applies an input camera pose and preserves manual rotation of the camera.
 *
 * @section XML XML Configuration
 * @code{.xml}
 *   <service uid="..." type="sight::module::viz::scene3d::adaptor::fly_camera">
 *     <data transform_in="..." />
 *     <config priority="..." />
 *   </service>
 * @endcode
 *
 * @subsection Configuration Configuration
 * - \b config.priority [int] (optional): interactor priority. Defaults to 1.
 *
 * @subsection Input Input
 * - \b data.transform_in [sight::data::matrix4] (optional): camera pose along the path.
 *
 * @subsection Slots Slots
 * - \b set_interaction_enabled(bool): enables fly interaction or restores the previous camera pose.
 *   Fly interaction is disabled by default.
 *
 * @subsection Signals Signals
 * - \b interaction_disabled(): emitted when leaving fly interaction mode.
 *
 */
class fly_camera final : public sight::viz::scene3d::adaptor,
                         public sight::viz::scene3d::interactor::base
{
public:

    SIGHT_DECLARE_SERVICE(fly_camera, sight::viz::scene3d::adaptor);

    fly_camera() noexcept;

    ~fly_camera() noexcept final = default;

    void set_interaction_enabled(bool _enabled);

    /** @brief Rotates the camera when the left mouse button is dragged. */
    void mouse_move_event(
        mouse_button _button,
        modifier /*_mods*/,
        int _x,
        int _y,
        int /*_dx*/,
        int /*_dy*/
    ) final;

    struct slots
    {
        static inline const slot_key_t SET_INTERACTION_ENABLED = "set_interaction_enabled";
    };
    struct signals
    {
        using interaction_disabled_t = core::com::signal<void()>;

        static inline const signal_key_t INTERACTION_DISABLED = "interaction_disabled";
    };

protected:

    void configuring() final;

    void starting() final;

    void updating() final;

    void stopping() final;

    service::connections_t auto_connections() const final;

private:

    /**
     * @brief Rotates the camera by the specified pixel deltas.
     * @param _dx Horizontal rotation delta.
     * @param _dy Vertical rotation delta.
     */
    void camera_rotate(int _dx, int _dy);

    /// Priority used when registering this adaptor as an interactor.
    int m_priority {1};

    /// The path orientation after the camera flip.
    Ogre::Quaternion m_base_orientation {Ogre::Degree(180), Ogre::Vector3::UNIT_X};
    Ogre::Quaternion m_user_rotation {Ogre::Quaternion::IDENTITY};
    Ogre::Vector3 m_normal_position {Ogre::Vector3::ZERO};
    Ogre::Quaternion m_normal_orientation {Ogre::Quaternion::IDENTITY};
    std::mutex m_rotation_mutex;
    std::atomic<bool> m_interaction_enabled {false};
    std::atomic<bool> m_has_pose {false};

    data::ptr<data::matrix4, data::access::in> m_transform_in {this, "data.transform_in", true};
};

} // namespace sight::module::viz::scene3d::adaptor
