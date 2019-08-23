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

#include "fwRenderOgre/helper/Camera.hpp"

#include <OGRE/OgreViewport.h>

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

Ogre::Matrix4 Camera::computeProjectionMatrix(const ::arData::Camera& _calibration, float _width,
                                              float _height, float _near, float _far)
{
    OSLM_ASSERT("camera width should be > 0.", _width > 0.0f);
    OSLM_ASSERT("height width should be > 0.", _height > 0.0f);
    OSLM_ASSERT("near width should be > 0.", _near > 0.0f);
    OSLM_ASSERT("far width should be > 0.", _far > 0.0f);
    OSLM_ASSERT("far width should be > near.", _far > _near);
    const float fx = static_cast< float >(_calibration.getFx());
    const float fy = static_cast< float >(_calibration.getFy());

    const float cx = static_cast< float >( _calibration.getCx() );
    const float cy = static_cast< float >( _calibration.getCy() );

    //calibration images size
    const float imW = static_cast< float >( _calibration.getWidth() );
    const float imH = static_cast< float >( _calibration.getHeight() );

    //compute ratio between calibration image height & displayed image height
    const float ratioH = _height / imH;

    //compute new fx, fy
    const float nfx = fx * ratioH;
    const float nfy = fy * ratioH;

    // compute principle point offset according to size of displayed image
    float px       = ratioH * cx;
    const float py = ratioH * cy;

    const long expectedWindowSize = std::lround(ratioH * imW);

    if( expectedWindowSize != static_cast<long>(_width))
    {
        const long diffX = (static_cast<long>(_width) - expectedWindowSize) / 2;
        px += static_cast<float>(diffX);
    }

    const float cx1 = _width - px;
    const float cy1 = _height - py;

    // avoid divide by zero below
    const float invWinW = std::max(1e-5f, _width - 1.f);
    const float invWinH = std::max(1e-5f, _height - 1.f);

    // compute the offset according to current size
    const float wcx = cx1 / ( invWinW / 2.f) -1.f;
    const float wcy = cy1 / ( invWinH / 2.f) -1.f;

    // setup projection matrix
    ::Ogre::Matrix4 m;

    m[0][0] = 2.f * nfx / _width;
    m[0][1] = 0.f;
    m[0][2] = wcx;
    m[0][3] = 0.f;

    m[1][0] = 0.f;
    m[1][1] = 2.f * nfy / _height;
    m[1][2] = -wcy;
    m[1][3] = 0.f;

    m[2][0] = 0.f;
    m[2][1] = 0.f;
    m[2][2] = -(_far + _near) / (_far - _near);
    m[2][3] = -2.f * _far * _near / (_far - _near);

    m[3][0] = 0.f;
    m[3][1] = 0.f;
    m[3][2] = -1.f;
    m[3][3] = 0.f;

    return m;
}

//-----------------------------------------------------------------------------

Ogre::Vector3 Camera::convertPixelToViewSpace(const ::Ogre::Camera& _camera, const float _pixelPoint[3])
{
    const ::Ogre::Viewport* viewport = _camera.getViewport();

    const float x = _pixelPoint[0]/static_cast<float>(viewport->getActualWidth()) * 2.f - 1.f;
    const float y = -(_pixelPoint[1]/static_cast<float>(viewport->getActualHeight()) * 2.f - 1.f);
    const float z = _pixelPoint[2] * 2.f - 1.f;
    const ::Ogre::Vector3 ndcCoordinates(x, y, z);

    ::Ogre::Vector4 clippingCoordinatePixel;
    if(_camera.getProjectionType() == ::Ogre::ProjectionType::PT_PERSPECTIVE)
    {
        const float near = static_cast<const float>(_camera.getNearClipDistance());
        const float far  = static_cast<float>(_camera.getFarClipDistance());
        clippingCoordinatePixel.w = static_cast< ::Ogre::Real >(2.0 * near * far)  /
                                    (near + far + ndcCoordinates.z * (near - far));
    }
    else
    {
        clippingCoordinatePixel.w = 1;
    }
    clippingCoordinatePixel.x = ndcCoordinates.x * clippingCoordinatePixel.w;
    clippingCoordinatePixel.y = ndcCoordinates.y * clippingCoordinatePixel.w;
    clippingCoordinatePixel.z = ndcCoordinates.z * clippingCoordinatePixel.w;

    const ::Ogre::Affine3& viewMat = _camera.getViewMatrix();
    const ::Ogre::Matrix4& projMat = _camera.getProjectionMatrixWithRSDepth();
    const auto inversedCombinedMat = (projMat * viewMat).inverse();

    // We multiply by the inverse since we are performing the usual projection in the other way around.
    const ::Ogre::Vector4 result = inversedCombinedMat * clippingCoordinatePixel;

    return result.xyz();
}

} // namespace helper

} // namespace fwRenderOgre
