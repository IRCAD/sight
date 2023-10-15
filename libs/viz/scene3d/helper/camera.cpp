/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "viz/scene3d/helper/camera.hpp"

#include <OGRE/OgreViewport.h>

namespace sight::viz::scene3d::helper
{

//-----------------------------------------------------------------------------

Ogre::Matrix4 camera::computeFrustumShearTransform(const Ogre::Camera& _camera, float _angle)
{
    Ogre::Matrix4 shearTransform = Ogre::Matrix4::IDENTITY;

    const float focalLength  = _camera.getFocalLength();
    const float xShearFactor = std::tan(_angle);

    shearTransform[0][2] = -xShearFactor;
    shearTransform[0][3] = -focalLength * xShearFactor;

    return shearTransform;
}

//-----------------------------------------------------------------------------

Ogre::Matrix4 camera::computeProjectionMatrix(
    const data::camera& _calibration,
    float _width,
    float _height,
    float _near,
    float _far
)
{
    SIGHT_ASSERT("camera width should be > 0.", _width > 0.0F);
    SIGHT_ASSERT("height width should be > 0.", _height > 0.0F);
    SIGHT_ASSERT("near width should be > 0.", _near > 0.0F);
    SIGHT_ASSERT("far width should be > 0.", _far > 0.0F);
    SIGHT_ASSERT("far width should be > near.", _far > _near);
    const auto fx = static_cast<float>(_calibration.getFx());
    const auto fy = static_cast<float>(_calibration.getFy());

    const auto cx = static_cast<float>(_calibration.getCx());
    const auto cy = static_cast<float>(_calibration.getCy());

    //calibration images size
    const auto imW = static_cast<float>(_calibration.getWidth());
    const auto imH = static_cast<float>(_calibration.getHeight());

    //compute ratio between calibration image height & displayed image height
    const float ratioH = _height / imH;

    //compute new fx, fy
    const float nfx = fx * ratioH;
    const float nfy = fy * ratioH;

    // compute principle point offset according to size of displayed image
    float px       = ratioH * cx;
    const float py = ratioH * cy;

    const std::int64_t expectedWindowSize = std::lround(ratioH * imW);

    if(expectedWindowSize != static_cast<std::int64_t>(_width))
    {
        const std::int64_t diffX = (static_cast<std::int64_t>(_width) - expectedWindowSize) / 2;
        px += static_cast<float>(diffX);
    }

    const float cx1 = _width - px;
    const float cy1 = _height - py;

    // avoid divide by zero below
    const float invWinW = std::max(1e-5F, _width - 1.F);
    const float invWinH = std::max(1e-5F, _height - 1.F);

    // compute the offset according to current size
    const float wcx = cx1 / (invWinW / 2.F) - 1.F;
    const float wcy = cy1 / (invWinH / 2.F) - 1.F;

    // setup projection matrix
    Ogre::Matrix4 m;

    m[0][0] = 2.F * nfx / _width;
    m[0][1] = 0.F;
    m[0][2] = wcx;
    m[0][3] = 0.F;

    m[1][0] = 0.F;
    m[1][1] = 2.F * nfy / _height;
    m[1][2] = -wcy;
    m[1][3] = 0.F;

    m[2][0] = 0.F;
    m[2][1] = 0.F;
    m[2][2] = -(_far + _near) / (_far - _near);
    m[2][3] = -2.F * _far * _near / (_far - _near);

    m[3][0] = 0.F;
    m[3][1] = 0.F;
    m[3][2] = -1.F;
    m[3][3] = 0.F;

    return m;
}

//-----------------------------------------------------------------------------

Ogre::Vector2 camera::convertFromWindowToViewportSpace(
    const Ogre::Camera& _camera,
    int _renderWindowX,
    int _renderWindowY
)
{
    const auto* const vp = _camera.getViewport();
    const int height     = vp->getActualHeight();
    const int width      = vp->getActualWidth();
    const int left       = vp->getActualLeft();
    const int top        = vp->getActualTop();

    return {
        static_cast<float>(_renderWindowX - left) / static_cast<float>(width),
        static_cast<float>(_renderWindowY - top) / static_cast<float>(height)
    };
}

//-----------------------------------------------------------------------------

Ogre::Vector3 camera::convertScreenSpaceToViewSpace(const Ogre::Camera& _camera, const Ogre::Vector3& _screenPos)
{
    const Ogre::Viewport* viewport = _camera.getViewport();

    const Ogre::Vector3 vpSize(static_cast<float>(viewport->getActualWidth()),
                               static_cast<float>(viewport->getActualHeight()), 1.F);
    const Ogre::Vector3 vpPosition(static_cast<float>(viewport->getActualLeft()),
                                   static_cast<float>(viewport->getActualTop()), 0.F);

    Ogre::Vector3 ndcPos = (_screenPos - vpPosition) / vpSize;
    ndcPos    = ndcPos * 2.F - 1.F;
    ndcPos.y *= -1.F;

    return convertNDCToViewSpace(_camera, ndcPos);
}

//-----------------------------------------------------------------------------

Ogre::Vector3 camera::convertNDCToViewSpace(const Ogre::Camera& _camera, const Ogre::Vector3& _ndcPos)
{
    Ogre::Vector4 clippingCoordinatePixel;
    if(_camera.getProjectionType() == Ogre::ProjectionType::PT_PERSPECTIVE)
    {
        const auto near = static_cast<float>(_camera.getNearClipDistance());
        const auto far  = static_cast<float>(_camera.getFarClipDistance());
        clippingCoordinatePixel.w = static_cast<Ogre::Real>(2.0 * near * far)
                                    / (near + far + _ndcPos.z * (near - far));
    }
    else
    {
        clippingCoordinatePixel.w = 1;
    }

    clippingCoordinatePixel.x = _ndcPos.x * clippingCoordinatePixel.w;
    clippingCoordinatePixel.y = _ndcPos.y * clippingCoordinatePixel.w;
    clippingCoordinatePixel.z = _ndcPos.z * clippingCoordinatePixel.w;

    const Ogre::Affine3& viewMat   = _camera.getViewMatrix();
    const Ogre::Matrix4& projMat   = _camera.getProjectionMatrixWithRSDepth();
    const auto invertedCombinedMat = (projMat * viewMat).inverse();

    // We multiply by the inverse since we are performing the usual projection in the other way around.
    const Ogre::Vector4 result = invertedCombinedMat * clippingCoordinatePixel;

    return result.xyz();
}

//------------------------------------------------------------------------------

Ogre::Vector2 camera::convertWorldSpaceToScreenSpace(const Ogre::Camera& _camera, const Ogre::Vector3& _worldPos)
{
    const Ogre::Affine3& viewMat = _camera.getViewMatrix();
    const Ogre::Matrix4& projMat = _camera.getProjectionMatrix();

    const Ogre::Vector4 result = projMat * viewMat * Ogre::Vector4(_worldPos, 1.0);
    if(result.w == 0.F)
    {
        SIGHT_ERROR("Homogenous coordinate is null, this should not happen");
        return {0.F, 0.F};
    }

    Ogre::Vector3 ndcPos = result.xyz() / result.w;

    const Ogre::Viewport* viewport = _camera.getViewport();

    const Ogre::Vector2 vpSize(static_cast<float>(viewport->getActualWidth()),
                               static_cast<float>(viewport->getActualHeight()));
    const Ogre::Vector2 vpPosition(static_cast<float>(viewport->getActualLeft()),
                                   static_cast<float>(viewport->getActualTop()));
    ndcPos.y *= -1.F;
    const Ogre::Vector2 screenPos = ((ndcPos.xy() + 1.F) * 0.5F) * vpSize + vpPosition;

    return screenPos;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::helper
