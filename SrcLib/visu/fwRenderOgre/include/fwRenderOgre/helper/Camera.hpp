/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_HELPER_CAMERA_HPP__
#define __FWRENDEROGRE_HELPER_CAMERA_HPP__

#include "fwRenderOgre/config.hpp"

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

};

} // namespace helper

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_HELPER_CAMERA_HPP__

