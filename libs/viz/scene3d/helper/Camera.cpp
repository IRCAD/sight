/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include "viz/scene3d/helper/Camera.hpp"

#include <OGRE/OgreViewport.h>

namespace sight::viz::scene3d
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

Ogre::Matrix4 Camera::computeProjectionMatrix(
    const data::Camera& _calibration,
    float _width,
    float _height,
    float _near,
    float _far
)
{
    SIGHT_ASSERT("camera width should be > 0.", _width > 0.0f);
    SIGHT_ASSERT("height width should be > 0.", _height > 0.0f);
    SIGHT_ASSERT("near width should be > 0.", _near > 0.0f);
    SIGHT_ASSERT("far width should be > 0.", _far > 0.0f);
    SIGHT_ASSERT("far width should be > near.", _far > _near);
    const float fx = static_cast<float>(_calibration.getFx());
    const float fy = static_cast<float>(_calibration.getFy());

    const float cx = static_cast<float>(_calibration.getCx());
    const float cy = static_cast<float>(_calibration.getCy());

    //calibration images size
    const float imW = static_cast<float>(_calibration.getWidth());
    const float imH = static_cast<float>(_calibration.getHeight());

    //compute ratio between calibration image height & displayed image height
    const float ratioH = _height / imH;

    //compute new fx, fy
    const float nfx = fx * ratioH;
    const float nfy = fy * ratioH;

    // compute principle point offset according to size of displayed image
    float px       = ratioH * cx;
    const float py = ratioH * cy;

    const long expectedWindowSize = std::lround(ratioH * imW);

    if(expectedWindowSize != static_cast<long>(_width))
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
    const float wcx = cx1 / (invWinW / 2.f) - 1.f;
    const float wcy = cy1 / (invWinH / 2.f) - 1.f;

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

::Ogre::Vector2 Camera::convertFromWindowToViewportSpace(
    const ::Ogre::Camera& _camera,
    int _renderWindowX,
    int _renderWindowY
)
{
    const auto* const vp = _camera.getViewport();
    const int height     = vp->getActualHeight();
    const int width      = vp->getActualWidth();
    const int left       = vp->getActualLeft();
    const int top        = vp->getActualTop();

    return ::Ogre::Vector2(
        static_cast<float>(_renderWindowX - left) / static_cast<float>(width),
        static_cast<float>(_renderWindowY - top) / static_cast<float>(height)
    );
}

//-----------------------------------------------------------------------------

Ogre::Vector3 Camera::convertScreenSpaceToViewSpace(const Ogre::Camera& _camera, const Ogre::Vector3& _screenPos)
{
    const ::Ogre::Viewport* viewport = _camera.getViewport();

    const ::Ogre::Vector3 vpSize(static_cast<float>(viewport->getActualWidth()),
                                 static_cast<float>(viewport->getActualHeight()), 1.f);
    const ::Ogre::Vector3 vpPosition(static_cast<float>(viewport->getActualLeft()),
                                     static_cast<float>(viewport->getActualTop()), 0.f);

    ::Ogre::Vector3 ndcPos = (_screenPos - vpPosition) / vpSize;
    ndcPos    = ndcPos * 2.f - 1.f;
    ndcPos.y *= -1.f;

    return convertNDCToViewSpace(_camera, ndcPos);
}

//-----------------------------------------------------------------------------

Ogre::Vector3 Camera::convertNDCToViewSpace(const ::Ogre::Camera& _camera, const ::Ogre::Vector3& _ndcPos)
{
    ::Ogre::Vector4 clippingCoordinatePixel;
    if(_camera.getProjectionType() == ::Ogre::ProjectionType::PT_PERSPECTIVE)
    {
        const float near = static_cast<const float>(_camera.getNearClipDistance());
        const float far  = static_cast<float>(_camera.getFarClipDistance());
        clippingCoordinatePixel.w = static_cast< ::Ogre::Real>(2.0 * near * far)
                                    / (near + far + _ndcPos.z * (near - far));
    }
    else
    {
        clippingCoordinatePixel.w = 1;
    }

    clippingCoordinatePixel.x = _ndcPos.x * clippingCoordinatePixel.w;
    clippingCoordinatePixel.y = _ndcPos.y * clippingCoordinatePixel.w;
    clippingCoordinatePixel.z = _ndcPos.z * clippingCoordinatePixel.w;

    const ::Ogre::Affine3& viewMat = _camera.getViewMatrix();
    const ::Ogre::Matrix4& projMat = _camera.getProjectionMatrixWithRSDepth();
    const auto inversedCombinedMat = (projMat * viewMat).inverse();

    // We multiply by the inverse since we are performing the usual projection in the other way around.
    const ::Ogre::Vector4 result = inversedCombinedMat * clippingCoordinatePixel;

    return result.xyz();
}

//-----------------------------------------------------------------------------

} // namespace helper

} // namespace sight::viz::scene3d
