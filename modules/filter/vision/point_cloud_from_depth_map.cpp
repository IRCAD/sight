/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "point_cloud_from_depth_map.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/profiling.hpp>

#include <filter/vision/projection.hpp>

#include <geometry/data/matrix4.hpp>

#include <service/macros.hpp>

#include <glm/glm.hpp>

#include <cmath>

namespace sight::module::filter::vision
{

const core::com::slots::key_t point_cloud_from_depth_map::SET_DEPTH_RANGE = "setDepthRange";

//------------------------------------------------------------------------------

point_cloud_from_depth_map::point_cloud_from_depth_map() noexcept
{
    new_slot(SET_DEPTH_RANGE, &point_cloud_from_depth_map::setDepthRange, this);
}

//------------------------------------------------------------------------------

point_cloud_from_depth_map::~point_cloud_from_depth_map() noexcept =
    default;

//------------------------------------------------------------------------------

void point_cloud_from_depth_map::starting()
{
}

//------------------------------------------------------------------------------

void point_cloud_from_depth_map::stopping()
{
}

//------------------------------------------------------------------------------

void point_cloud_from_depth_map::configuring()
{
}

//------------------------------------------------------------------------------

void point_cloud_from_depth_map::updating()
{
    const auto calibration = m_calibration.lock();
    const auto depth_map   = m_depthMap.lock();
    const auto point_cloud = m_pointCloud.lock();

    SIGHT_ASSERT("Missing 'pointCloud' inout", point_cloud);
    SIGHT_ASSERT("Missing 'calibration' input", calibration);
    SIGHT_ASSERT("Missing 'depthMap' input", depth_map);

    const auto depth_calibration = calibration->get_camera(0);

    const auto rgb_map = m_rgbMap.lock();

    data::camera::csptr color_calibration;
    data::matrix4::csptr extrinsic_matrix;

    if(rgb_map)
    {
        color_calibration = calibration->get_camera(1);
        extrinsic_matrix  = calibration->get_extrinsic_matrix(1);
        SIGHT_ASSERT("Missing extrinsic matrix", extrinsic_matrix);
    }

    // Initialize mesh points memory one time in order to increase performances
    if(point_cloud->numPoints() == 0)
    {
        const auto size             = depth_map->size();
        const std::size_t width     = size[0];
        const std::size_t height    = size[1];
        const std::size_t nb_points = width * height;

        // allocate mesh
        data::mesh::Attributes attribute = data::mesh::Attributes::NONE;
        if(rgb_map)
        {
            attribute = data::mesh::Attributes::POINT_COLORS;
        }

        point_cloud->resize(
            data::mesh::size_t(nb_points),
            data::mesh::size_t(nb_points),
            data::mesh::cell_type_t::POINT,
            attribute
        );

        const auto dump_lock = point_cloud->dump_lock();

        auto itr = point_cloud->begin<data::iterator::cell::point>();

        // to display the mesh, we need to create cells with one point.
        for(std::size_t i = 0 ; i < nb_points ; ++i, ++itr)
        {
            itr->pt = data::mesh::cell_t(i);
        }

        auto sig = point_cloud->signal<data::mesh::modified_signal_t>(data::mesh::MODIFIED_SIG);
        sig->async_emit();
    }

    if(rgb_map)
    {
        this->depthMapToPointCloudRGB(
            depth_calibration,
            color_calibration,
            depth_map.get_shared(),
            rgb_map.get_shared(),
            extrinsic_matrix,
            point_cloud.get_shared()
        );

        auto sig =
            point_cloud->signal<data::mesh::signal_t>(data::mesh::VERTEX_MODIFIED_SIG);
        sig->async_emit();

        auto sig2 = point_cloud->signal<data::mesh::signal_t>(data::mesh::POINT_COLORS_MODIFIED_SIG);
        sig2->async_emit();
    }
    else
    {
        this->depthMapToPointCloud(depth_calibration, depth_map.get_shared(), point_cloud.get_shared());
        auto sig =
            point_cloud->signal<data::mesh::signal_t>(data::mesh::VERTEX_MODIFIED_SIG);
        sig->async_emit();
    }

    m_sigComputed->async_emit();
}

//------------------------------------------------------------------------------

void point_cloud_from_depth_map::setDepthRange(int _val, std::string _key)
{
    if(_key == "minDepth")
    {
        if(_val >= 0 && _val <= UINT16_MAX)
        {
            m_minDepth = static_cast<std::uint16_t>(_val);
        }
        else
        {
            SIGHT_ERROR("min Depth should be between [0; 65536]");
        }
    }
    else if(_key == "maxDepth")
    {
        if(_val >= 0 && _val <= UINT16_MAX)
        {
            m_maxDepth = static_cast<std::uint16_t>(_val);
        }
        else
        {
            SIGHT_ERROR("min Depth should be between [0; 65536]");
        }
    }
    else
    {
        SIGHT_ERROR(
            std::string("unknown key '") + _key + "' in slot '" + SET_DEPTH_RANGE + "'"
        );
    }
}

//------------------------------------------------------------------------------

void point_cloud_from_depth_map::depthMapToPointCloud(
    const data::camera::csptr& _depth_camera,
    const data::image::csptr& _depth_map,
    const data::mesh::sptr& _point_cloud
) const
{
    SIGHT_INFO("Input RGB map was empty, skipping colors");

    const auto type = _depth_map->getType();
    if(type != core::type::UINT16)
    {
        SIGHT_ERROR("Wrong input depth map format: " << type << ", uint16 is expected.");
        return;
    }

    const auto size          = _depth_map->size();
    const std::size_t width  = size[0];
    const std::size_t height = size[1];

    const auto depth_dump_lock = _depth_map->dump_lock();

    auto depth_itr = _depth_map->begin<std::uint16_t>();

    const double cx = _depth_camera->getCx();
    const double cy = _depth_camera->getCy();
    const double fx = _depth_camera->getFx();
    const double fy = _depth_camera->getFy();

    const auto mesh_dump_lock = _point_cloud->dump_lock();

    auto points_itr = _point_cloud->begin<data::iterator::point::xyz>();

    for(std::size_t y = 0 ; y != height ; ++y)
    {
        for(std::size_t x = 0 ; x != width ; ++x)
        {
            const uint16_t depth = *depth_itr;
            if(depth >= m_minDepth && depth <= m_maxDepth)
            {
                double px = NAN;
                double py = NAN;
                double pz = NAN;
                sight::filter::vision::projection::projectPixel<double>(x, y, depth, cx, cy, fx, fy, px, py, pz);
                points_itr->x = static_cast<float>(px);
                points_itr->y = static_cast<float>(py);
                points_itr->z = static_cast<float>(pz);
                ++points_itr;
            }

            ++depth_itr;
        }
    }
}

//------------------------------------------------------------------------------

void point_cloud_from_depth_map::depthMapToPointCloudRGB(
    const data::camera::csptr& _depth_camera,
    const data::camera::csptr& _color_camera,
    const data::image::csptr& _depth_map,
    const data::image::csptr& _color_map,
    const data::matrix4::csptr& _extrinsic,
    const data::mesh::sptr& _point_cloud
) const
{
    SIGHT_INFO("Input RGB map was supplied, including colors");

    const auto type = _depth_map->getType();
    if(type != core::type::UINT16)
    {
        SIGHT_ERROR("Wrong input depth map format: " << type << ", uint16 is expected.");
        return;
    }

    // Make sure RGB and depth maps are the same size
    const auto size          = _depth_map->size();
    const std::size_t width  = size[0];
    const std::size_t height = size[1];

    const auto rgb_type = _color_map->getType();
    if(rgb_type != core::type::UINT8)
    {
        SIGHT_ERROR("Wrong input rgb format: " << rgb_type << ", uint8 is expected.");
        return;
    }

    if(4 != _color_map->numComponents())
    {
        SIGHT_ERROR("Wrong number of components in rgb : " << _color_map->numComponents() << ", 4 is expected.");
        return;
    }

    const auto rgb_size          = _color_map->size();
    const std::size_t rgb_width  = rgb_size[0];
    const std::size_t rgb_height = rgb_size[1];

    if(rgb_width != width || rgb_height != height)
    {
        SIGHT_ERROR("RGB and depth maps must have the same size");
        return;
    }

    const auto depth_dump_lock = _depth_map->dump_lock();
    auto depth_itr             = _depth_map->begin<std::uint16_t>();

    const auto rgb_dump_lock = _color_map->dump_lock();
    const auto rgb_begin     = _color_map->begin<data::iterator::rgba>();

    const double cx = _depth_camera->getCx();
    const double cy = _depth_camera->getCy();
    const double fx = _depth_camera->getFx();
    const double fy = _depth_camera->getFy();

    const double rgb_cx = _color_camera->getCx();
    const double rgb_cy = _color_camera->getCy();
    const double rgb_fx = _color_camera->getFx();
    const double rgb_fy = _color_camera->getFy();

    const auto mesh_dump_lock = _point_cloud->dump_lock();

    auto points_itr = _point_cloud->zip_range<data::iterator::point::xyz, data::iterator::point::rgba>().begin();

    const data::iterator::rgba default_color = {255, 255, 255, 255};

    unsigned int nb_real_points = 0;
    auto glm_extrinsic_matrix   = geometry::data::to_glm_mat(*_extrinsic);

    const auto image_size = height * width;
    for(std::size_t y = 0 ; y != height ; ++y)
    {
        for(std::size_t x = 0 ; x != width ; ++x)
        {
            const uint16_t depth = *depth_itr;
            if(depth >= m_minDepth && depth <= m_maxDepth)
            {
                auto&& [p, c] = *points_itr;

                // get the 3D coordinates in the depth world
                double px = NAN;
                double py = NAN;
                double pz = NAN;
                sight::filter::vision::projection::projectPixel<double>(x, y, depth, cx, cy, fx, fy, px, py, pz);
                p.x = static_cast<float>(px);
                p.y = static_cast<float>(py);
                p.z = static_cast<float>(pz);

                // Transform point to the rgb sensor world
                const glm::dvec4 point(px, py, pz, 1.0);
                const glm::dvec4 rgb_point = glm_extrinsic_matrix * point;

                // project point to the rgb image
                std::size_t rgb_px      = 0;
                std::size_t rgb_py      = 0;
                const bool is_projected = sight::filter::vision::projection::projectPoint(
                    rgb_point.x,
                    rgb_point.y,
                    rgb_point.z,
                    rgb_cx,
                    rgb_cy,
                    rgb_fx,
                    rgb_fy,
                    rgb_width,
                    rgb_height,
                    rgb_px,
                    rgb_py
                );

                if(is_projected)
                {
                    const std::size_t rgb_idx = rgb_py * rgb_width + rgb_px;
                    if(rgb_idx < image_size)
                    {
                        const auto color = rgb_begin + std::int64_t(rgb_idx);
                        c = *color;
                    }
                    else
                    {
                        c = default_color;
                    }
                }
                else
                {
                    c = default_color;
                }

                ++points_itr;
                ++nb_real_points;
            }

            ++depth_itr;
        }
    }

    // Since we discard points for which the depth map is zero, the mesh buffers are not full
    _point_cloud->truncate(nb_real_points, nb_real_points);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::filter::vision
