/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
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

#include "viz/scene3d/config.hpp"

#include <data/Camera.hpp>

#include <OGRE/OgreCamera.h>

namespace sight::viz::scene3d::helper
{

class Camera
{
public:

    /**
     * @brief Shear transform used in autostereo.
     * @param _camera[in] camera where to get the projection matrix
     * @param _angle[in] shear angle
     */
    static VIZ_SCENE3D_API Ogre::Matrix4 computeFrustumShearTransform(const Ogre::Camera& _camera, float _angle);

    static VIZ_SCENE3D_API Ogre::Matrix4 computeProjectionMatrix(
        const data::Camera& _calibration,
        float _width,
        float _height,
        float _near,
        float _far
    );

    /**
     * @brief Returns a render window's position relatively to the camera's viewport.
     * @param[in] _camera viewport in which the coordinates are transformed.
     * @param[in] _renderWindowX width position in the render window.
     * @param[in] _renderWindowY height position in the render window.
     *
     * @return screen position relatively to the viewport.
     */
    static VIZ_SCENE3D_API Ogre::Vector2 convertFromWindowToViewportSpace(
        const Ogre::Camera& _camera,
        int _renderWindowX,
        int _renderWindowY
    );

    /**
     * @brief Converts from screen space (u, v, w) to view space (x, y, z).
     * @param _camera ogre camera.
     * @param _screenPos screen position (u, v, w).
     * @return view space coordinates (x, y, z).
     */
    static VIZ_SCENE3D_API Ogre::Vector3 convertScreenSpaceToViewSpace(
        const Ogre::Camera& _camera,
        const Ogre::Vector3& _screenPos
    );

    /**
     * @brief Converts from NDC space to view space (call by @ref convertScreenSpaceToViewSpace)
     * @param _camera ogre camera.
     * @param _ndcPos NDC coordinates (3 components).
     * @return the view space coordinates (x, y, z).
     */
    static VIZ_SCENE3D_API Ogre::Vector3 convertNDCToViewSpace(
        const Ogre::Camera& _camera,
        const Ogre::Vector3& _ndcPos
    );
};

} // namespace sight::viz::scene3d::helper
