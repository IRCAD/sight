/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include <data/camera.hpp>

#include <OGRE/OgreCamera.h>

namespace sight::viz::scene3d::helper
{

class camera
{
public:

    /**
     * @brief Shear transform used in autostereo.
     * @param _camera[in] camera where to get the projection matrix
     * @param _angle[in] shear angle
     */
    static SIGHT_VIZ_SCENE3D_API Ogre::Matrix4 compute_frustum_shear_transform(
        const Ogre::Camera& _camera,
        float _angle
    );

    static SIGHT_VIZ_SCENE3D_API Ogre::Matrix4 compute_projection_matrix(
        const data::camera& _calibration,
        float _width,
        float _height,
        float _near,
        float _far
    );

    /**
     * @brief Returns a render window's position relatively to the camera's viewport.
     * @param[in] _camera viewport in which the coordinates are transformed.
     * @param[in] _render_window_x width position in the render window.
     * @param[in] _render_window_y height position in the render window.
     *
     * @return screen position relatively to the viewport.
     */
    static SIGHT_VIZ_SCENE3D_API Ogre::Vector2 convert_from_window_to_viewport_space(
        const Ogre::Camera& _camera,
        int _render_window_x,
        int _render_window_y
    );

    /**
     * @brief Converts from screen space (u, v, w) to view space (x, y, z).
     * @param _camera ogre camera.
     * @param _screen_pos screen position (u, v, w).
     * @return view space coordinates (x, y, z).
     */
    static SIGHT_VIZ_SCENE3D_API Ogre::Vector3 convert_screen_space_to_view_space(
        const Ogre::Camera& _camera,
        const Ogre::Vector3& _screen_pos
    );

    /**
     * @brief Converts from NDC space to view space (call by @ref convert_screen_space_to_view_space)
     * @param _camera ogre camera.
     * @param _ndc_pos NDC coordinates (3 components).
     * @return the view space coordinates (x, y, z).
     */
    static SIGHT_VIZ_SCENE3D_API Ogre::Vector3 convert_ndc_to_view_space(
        const Ogre::Camera& _camera,
        const Ogre::Vector3& _ndc_pos
    );

    /**
     * @brief Converts from world space (x, y, z) to screen space (u, v, w).
     * @param _camera ogre camera.
     * @param _world_pos world position (x, y, z).
     * @return screen space coordinates in pixels (x, y).
     */
    static SIGHT_VIZ_SCENE3D_API Ogre::Vector2 convert_world_space_to_screen_space(
        const Ogre::Camera& _camera,
        const Ogre::Vector3& _world_pos
    );

    /**
     * @brief Gets the normalized camera direction vector.
     * @return A vector representing the camera direction
     */
    static SIGHT_VIZ_SCENE3D_API Ogre::Vector3 get_cam_direction(const Ogre::Camera* _cam);
};

} // namespace sight::viz::scene3d::helper
