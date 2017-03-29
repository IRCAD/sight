/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/helper/Camera.hpp"

namespace fwRenderOgre
{

namespace helper
{

//-----------------------------------------------------------------------------

Ogre::Matrix4 Camera::computeFrustumShearTransform(const ::Ogre::Camera& _camera, float _angle)
{
    ::Ogre::Matrix4 shearTransform = ::Ogre::Matrix4::IDENTITY;

    const float focalLength  = _camera.getFocalLength();
    const float xshearFactor = std::tan(_angle);

    shearTransform[0][2] = -xshearFactor;
    shearTransform[0][3] = -focalLength * xshearFactor;

    return shearTransform;
}

//-----------------------------------------------------------------------------

} // namespace helper

} // namespace fwRenderOgre
