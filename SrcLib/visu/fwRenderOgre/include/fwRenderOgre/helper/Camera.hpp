/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include <arData/Camera.hpp>

#include <OGRE/OgreCamera.h>

namespace fwRenderOgre
{

namespace helper
{

class Camera
{
public:

    /**
     * @brief Shear transform used in autostereo.
     * @param _camera[in] camera where to get the projection matrix
     * @param _angle[in] shear angle
     */
    static FWRENDEROGRE_API Ogre::Matrix4 computeFrustumShearTransform(const Ogre::Camera& _camera, float _angle);

    static FWRENDEROGRE_API Ogre::Matrix4 computeProjectionMatrix(const ::arData::Camera& _calibration,
                                                                  float _width, float _height,
                                                                  float _near,
                                                                  float _far);

    /**
     * @brief Returns a render window's position relatively to the camera's viewport.
     * @param[in] _camera viewport in which the coordinates are transformed.
     * @param[in] _renderWindowX width position in the render window.
     * @param[in] _renderWindowY height position in the render window.
     *
     * @return screen position relatively to the viewport.
     */
    static FWRENDEROGRE_API ::Ogre::Vector2 convertFromWindowToViewportSpace(const ::Ogre::Camera& _camera,
                                                                             int _renderWindowX, int _renderWindowY);

    /**
     * @brief Converts relative viewport coordinates to view space coordinates.
     * @param[in] _camera defines the view in which the coordinates are transformed.
     * @param[in] _viewportPos viewport point.
     */
    static FWRENDEROGRE_API ::Ogre::Vector3 convertFromScreenToViewSpace(const ::Ogre::Camera& _camera,
                                                                         const ::Ogre::Vector3& _viewportPos);

    /// Convenience function, casts the inputs and
    /// calls @ref convertPixelToViewSpace(const ::Ogre::Camera&, const ::Ogre::Vector3&)
    static FWRENDEROGRE_API ::Ogre::Vector3 convertPixelToViewSpace(const ::Ogre::Camera& _camera,
                                                                    int _screenX, int _screenY);

    /// @deprecated, call @ref convertFromScreenToViewSpace(const ::Ogre::Camera&, const ::Ogre::Vector3&) instead.
    /// Removed in sight 21.0
    [[deprecated("Use convertPixelToViewSpace(const ::Ogre::Camera&, const ::Ogre::Vector3&) instead.")]]
    static FWRENDEROGRE_API ::Ogre::Vector3 convertPixelToViewSpace(const ::Ogre::Camera& _camera,
                                                                    const float _pixelPoint[3]);

};

} // namespace helper

} // namespace fwRenderOgre
