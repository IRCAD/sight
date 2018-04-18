/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

};

} // namespace helper

} // namespace fwRenderOgre
