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

Ogre::Matrix4 camera::compute_frustum_shear_transform(const Ogre::Camera& _camera, float _angle)
{
    Ogre::Matrix4 shear_transform = Ogre::Matrix4::IDENTITY;

    const float focal_length   = _camera.getFocalLength();
    const float x_shear_factor = std::tan(_angle);

    shear_transform[0][2] = -x_shear_factor;
    shear_transform[0][3] = -focal_length * x_shear_factor;

    return shear_transform;
}

//-----------------------------------------------------------------------------

Ogre::Matrix4 camera::compute_projection_matrix(
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
    const auto fx = static_cast<float>(_calibration.get_fx());
    const auto fy = static_cast<float>(_calibration.get_fy());

    const auto cx = static_cast<float>(_calibration.get_cx());
    const auto cy = static_cast<float>(_calibration.get_cy());

    //calibration images size
    const auto im_w = static_cast<float>(_calibration.get_width());
    const auto im_h = static_cast<float>(_calibration.get_height());

    //compute ratio between calibration image height & displayed image height
    const float ratio_h = _height / im_h;

    //compute new fx, fy
    const float nfx = fx * ratio_h;
    const float nfy = fy * ratio_h;

    // compute principle point offset according to size of displayed image
    float px       = ratio_h * cx;
    const float py = ratio_h * cy;

    const std::int64_t expected_window_size = std::lround(ratio_h * im_w);

    if(expected_window_size != static_cast<std::int64_t>(_width))
    {
        const std::int64_t diff_x = (static_cast<std::int64_t>(_width) - expected_window_size) / 2;
        px += static_cast<float>(diff_x);
    }

    const float cx1 = _width - px;
    const float cy1 = _height - py;

    // avoid divide by zero below
    const float inv_win_w = std::max(1e-5F, _width - 1.F);
    const float inv_win_h = std::max(1e-5F, _height - 1.F);

    // compute the offset according to current size
    const float wcx = cx1 / (inv_win_w / 2.F) - 1.F;
    const float wcy = cy1 / (inv_win_h / 2.F) - 1.F;

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

Ogre::Vector2 camera::convert_from_window_to_viewport_space(
    const Ogre::Camera& _camera,
    int _render_window_x,
    int _render_window_y
)
{
    const auto* const vp = _camera.getViewport();
    const int height     = vp->getActualHeight();
    const int width      = vp->getActualWidth();
    const int left       = vp->getActualLeft();
    const int top        = vp->getActualTop();

    return {
        static_cast<float>(_render_window_x - left) / static_cast<float>(width),
        static_cast<float>(_render_window_y - top) / static_cast<float>(height)
    };
}

//-----------------------------------------------------------------------------

Ogre::Vector3 camera::convert_screen_space_to_view_space(const Ogre::Camera& _camera, const Ogre::Vector3& _screen_pos)
{
    const Ogre::Viewport* viewport = _camera.getViewport();

    const Ogre::Vector3 vp_size(static_cast<float>(viewport->getActualWidth()),
                                static_cast<float>(viewport->getActualHeight()), 1.F);
    const Ogre::Vector3 vp_position(static_cast<float>(viewport->getActualLeft()),
                                    static_cast<float>(viewport->getActualTop()), 0.F);

    Ogre::Vector3 ndc_pos = (_screen_pos - vp_position) / vp_size;
    ndc_pos    = ndc_pos * 2.F - 1.F;
    ndc_pos.y *= -1.F;

    return convert_ndc_to_view_space(_camera, ndc_pos);
}

//-----------------------------------------------------------------------------

Ogre::Vector3 camera::convert_ndc_to_view_space(const Ogre::Camera& _camera, const Ogre::Vector3& _ndc_pos)
{
    Ogre::Vector4 clipping_coordinate_pixel;
    if(_camera.getProjectionType() == Ogre::ProjectionType::PT_PERSPECTIVE)
    {
        const auto near = static_cast<float>(_camera.getNearClipDistance());
        const auto far  = static_cast<float>(_camera.getFarClipDistance());
        clipping_coordinate_pixel.w = static_cast<Ogre::Real>(2.0 * near * far)
                                      / (near + far + _ndc_pos.z * (near - far));
    }
    else
    {
        clipping_coordinate_pixel.w = 1;
    }

    clipping_coordinate_pixel.x = _ndc_pos.x * clipping_coordinate_pixel.w;
    clipping_coordinate_pixel.y = _ndc_pos.y * clipping_coordinate_pixel.w;
    clipping_coordinate_pixel.z = _ndc_pos.z * clipping_coordinate_pixel.w;

    const Ogre::Affine3& view_mat    = _camera.getViewMatrix();
    const Ogre::Matrix4& proj_mat    = _camera.getProjectionMatrixWithRSDepth();
    const auto inverted_combined_mat = (proj_mat * view_mat).inverse();

    // We multiply by the inverse since we are performing the usual projection in the other way around.
    const Ogre::Vector4 result = inverted_combined_mat * clipping_coordinate_pixel;

    return result.xyz();
}

//------------------------------------------------------------------------------

Ogre::Vector2 camera::convert_world_space_to_screen_space(const Ogre::Camera& _camera, const Ogre::Vector3& _world_pos)
{
    const Ogre::Affine3& view_mat = _camera.getViewMatrix();
    const Ogre::Matrix4& proj_mat = _camera.getProjectionMatrix();

    const Ogre::Vector4 result = proj_mat * view_mat * Ogre::Vector4(_world_pos, 1.0);
    if(result.w == 0.F)
    {
        SIGHT_ERROR("Homogenous coordinate is null, this should not happen");
        return {0.F, 0.F};
    }

    Ogre::Vector3 ndc_pos = result.xyz() / result.w;

    const Ogre::Viewport* viewport = _camera.getViewport();

    const Ogre::Vector2 vp_size(static_cast<float>(viewport->getActualWidth()),
                                static_cast<float>(viewport->getActualHeight()));
    const Ogre::Vector2 vp_position(static_cast<float>(viewport->getActualLeft()),
                                    static_cast<float>(viewport->getActualTop()));
    ndc_pos.y *= -1.F;
    const Ogre::Vector2 screen_pos = ((ndc_pos.xy() + 1.F) * 0.5F) * vp_size + vp_position;

    return screen_pos;
}

//------------------------------------------------------------------------------

Ogre::Vector3 camera::get_cam_direction(const Ogre::Camera* const _cam)
{
    const Ogre::Matrix4 view = _cam->getViewMatrix();
    Ogre::Vector3 direction(view[2][0], view[2][1], view[2][2]);
    direction.normalise();
    return -direction;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::helper
