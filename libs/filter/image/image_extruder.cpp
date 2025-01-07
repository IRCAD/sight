/************************************************************************
 *
 * Copyright (C) 2020-2025 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "filter/image/image_extruder.hpp"

#include <core/tools/dispatcher.hpp>

#include <geometry/data/image.hpp>
#include <geometry/data/matrix4.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/matrix.hpp>
#include <glm/vec2.hpp>

#include <cmath>

// Usual nolint comment does not work for an unknown reason (clang 17)
// cspell:ignore Wunknown
#ifdef __clang_analyzer__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#endif

namespace sight::filter::image
{

//------------------------------------------------------------------------------

void image_extruder::extrude(
    const data::image::sptr& _image,
    const data::mesh::csptr& _mesh,
    const data::matrix4::csptr& _transform
)
{
    SIGHT_ASSERT("The image must be in 3 dimensions", _image->num_dimensions() == 3);
    SIGHT_ASSERT("Spacing should be set", _image->spacing() != data::image::spacing_t({0., 0., 0.}));

    parameters param;
    param.m_image     = _image;
    param.m_mesh      = _mesh;
    param.m_transform = _transform;

    // We use a dispatcher because we can't retrieve the image type without a dynamic_t.
    core::type type = _image->type();
    core::tools::dispatcher<core::tools::supported_dispatcher_types, image_extruder>::invoke(type, param);
}

//------------------------------------------------------------------------------

template<typename IMAGE_TYPE>
void image_extruder::operator()(parameters& _param)
{
    auto transform = glm::identity<glm::mat4>();
    if(_param.m_transform)
    {
        // Apply the inverse matrix of the image to each point of the mesh
        const glm::dmat4x4 mat = sight::geometry::data::to_glm_mat(*_param.m_transform);
        transform = glm::inverse(mat);
    }

    // Creates triangles and bounding box of the mesh.
    std::int64_t index_x_beg = std::numeric_limits<std::int64_t>::max();
    std::int64_t index_x_end = std::numeric_limits<std::int64_t>::min();
    std::int64_t index_y_beg = std::numeric_limits<std::int64_t>::max();
    std::int64_t index_y_end = std::numeric_limits<std::int64_t>::min();
    std::int64_t index_z_beg = std::numeric_limits<std::int64_t>::max();
    std::int64_t index_z_end = std::numeric_limits<std::int64_t>::min();

    std::list<triangle> triangles;

    const auto add_triangle =
        [&](const data::iterator::point::xyz& _pa,
            const data::iterator::point::xyz& _pb,
            const data::iterator::point::xyz& _pc,
            const glm::mat4 _transform)
        {
            const float ax = _pa.x;
            const float ay = _pa.y;
            const float az = _pa.z;

            const float bx = _pb.x;
            const float by = _pb.y;
            const float bz = _pb.z;

            const float cx = _pc.x;
            const float cy = _pc.y;
            const float cz = _pc.z;

            const glm::vec4 tri_a = _transform * glm::vec4(ax, ay, az, 1.0);
            const glm::vec4 tri_b = _transform * glm::vec4(bx, by, bz, 1.0);
            const glm::vec4 tri_c = _transform * glm::vec4(cx, cy, cz, 1.0);

            triangles.push_back(triangle {tri_a, tri_b, tri_c});

            using sight::geometry::data::world_to_image;
            const auto a = world_to_image<std::array<std::int64_t, 3> >(*_param.m_image, tri_a, false, false);
            const auto b = world_to_image<std::array<std::int64_t, 3> >(*_param.m_image, tri_b, false, false);
            const auto c = world_to_image<std::array<std::int64_t, 3> >(*_param.m_image, tri_c, false, false);

            index_x_beg = std::min(index_x_beg, std::min(a[0], std::min(b[0], c[0])));
            index_y_beg = std::min(index_y_beg, std::min(a[1], std::min(b[1], c[1])));
            index_z_beg = std::min(index_z_beg, std::min(a[2], std::min(b[2], c[2])));

            index_x_end = std::max(index_x_end, std::max(a[0], std::max(b[0], c[0])));
            index_y_end = std::max(index_y_end, std::max(a[1], std::max(b[1], c[1])));
            index_z_end = std::max(index_z_end, std::max(a[2], std::max(b[2], c[2])));
        };

    auto it_point = _param.m_mesh->cbegin<data::iterator::point::xyz>();

    const auto cell_size = _param.m_mesh->cell_size();
    if(cell_size < 3)
    {
        SIGHT_FATAL("The extrusion works only with meshes of at least three points per cells");
    }
    else if(cell_size == 3)
    {
        for(const auto& cell : _param.m_mesh->crange<data::iterator::cell::triangle>())
        {
            const auto& point_a = it_point + cell.pt[0];
            const auto& point_b = it_point + cell.pt[1];
            const auto& point_c = it_point + cell.pt[2];
            add_triangle(*point_a, *point_b, *point_c, transform);
        }
    }
    else if(cell_size == 4)
    {
        for(const auto& cell : _param.m_mesh->crange<data::iterator::cell::quad>())
        {
            const auto& point_a = it_point + cell.pt[0];
            const auto& point_b = it_point + cell.pt[1];
            const auto& point_c = it_point + cell.pt[2];
            const auto& point_d = it_point + cell.pt[3];

            add_triangle(*point_a, *point_b, *point_c, transform);
            add_triangle(*point_c, *point_d, *point_a, transform);
        }
    }
    else
    {
        SIGHT_FATAL("The extrusion works only with meshes of at most four points per cells");
    }

    // Get images.
    const auto dump_lock = _param.m_image->dump_lock();

    const auto& size = _param.m_image->size();
    index_x_beg = std::clamp(index_x_beg, std::int64_t(0), std::int64_t(size[0]));
    index_x_end = std::clamp(index_x_end, std::int64_t(0), std::int64_t(size[0]));
    index_y_beg = std::clamp(index_y_beg, std::int64_t(0), std::int64_t(size[1]));
    index_y_end = std::clamp(index_y_end, std::int64_t(0), std::int64_t(size[1]));
    index_z_beg = std::clamp(index_z_beg, std::int64_t(0), std::int64_t(size[2]));
    index_z_end = std::clamp(index_z_end, std::int64_t(0), std::int64_t(size[2]));

    // Check if the ray origin is inside or outside of the mesh and return all found intersections.
    const auto get_intersections =
        [&triangles](const glm::vec3& _ray_orig, const glm::vec3& _ray_dir,
                     std::vector<glm::vec3>& _intersections) -> bool
        {
            bool inside = false;
            for(const triangle& tri : triangles)
            {
                glm::vec2 pos;
                float distance = NAN;
                if(glm::intersectRayTriangle(
                       _ray_orig,
                       _ray_dir,
                       tri.a,
                       tri.b,
                       tri.c,
                       pos,
                       distance
                ))
                {
                    if(distance >= 0.F)
                    {
                        const glm::vec3 cross = _ray_orig + _ray_dir * distance;
                        // Sometime, the ray it the edge of a triangle, we need to take it into account only one time.
                        if(std::find(_intersections.begin(), _intersections.end(), cross) == _intersections.end())
                        {
                            _intersections.push_back(cross);
                            inside = !inside;
                        }
                    }
                }
            }

            // Sort all intersections from nearest to farthest from the origin.
            std::sort(
                _intersections.begin(),
                _intersections.end(),
                [&](const glm::vec3& _a,
                    const glm::vec3& _b)
            {
                return glm::distance(_ray_orig, _a) < glm::distance(_ray_orig, _b);
            });

            return inside;
        };

    // Check if each voxel are in the mesh and sets the mask to zero.
    const IMAGE_TYPE empty_value = 0;

    using index_t = typename data::image::index_t;

    const auto orientation = _param.m_image->orientation();
    const glm::vec3 orientation_x(orientation[0], orientation[3], orientation[6]);
    const glm::vec3 orientation_y(orientation[1], orientation[4], orientation[7]);
    const glm::vec3 orientation_z(orientation[2], orientation[5], orientation[8]);

    const auto image_to_world_trf = sight::geometry::data::image_to_world_transform<glm::mat4>(*_param.m_image);
    const auto half               = glm::vec4(0.5, 0.5, 0.5, 0.);

    // We loop over two dimensions out of three, for each voxel, we launch a ray on the third dimension and get a
    // list of intersections. After that, we iterate over the voxel line on the third dimension and with the
    // intersections list, we know if the voxel is inside or outside of the mesh. So to improve performance, we need
    // to launch the minimum number of rays. The better way is to create three loops and call the right one.
    const auto z_loop =
        [&]()
        {
            // NOLINTNEXTLINE(clang-diagnostic-unknown-pragmas)
        #pragma omp parallel for
            for(std::int64_t x = index_x_beg ; x < index_x_end ; ++x)
            {
                for(std::int64_t y = index_y_beg ; y < index_y_end ; ++y)
                {
                    // For each voxel of the slice, launch a ray to the third axis.
                    const auto ray_orig = glm::xyz(image_to_world_trf * (glm::vec4(x, y, index_z_beg, 1.0) + half));

                    // Check if the first voxel is inside or not, and stores all intersections.
                    std::vector<glm::vec3> intersections;
                    bool inside = get_intersections(ray_orig, orientation_z, intersections);

                    // If there is no intersection, the entire line is visible.
                    if(!intersections.empty())
                    {
                        // Iterate over the "ray" and check intersections to know if the voxel is inside
                        // or outside of the mesh.
                        auto next_intersection      = intersections.begin();
                        const auto intersection_end = intersections.end();
                        for(std::int64_t z = index_z_beg ; z < index_z_end ; ++z)
                        {
                            const auto voxel = glm::xyz(image_to_world_trf * (glm::vec4(x, y, z, 1.0) + half));

                            // While the current ray position is near to the next intersection, set the
                            // voxel to the value if
                            // it's needed.
                            if(glm::distance(ray_orig, voxel) < glm::distance(ray_orig, *next_intersection))
                            {
                                if(inside)
                                {
                                    _param.m_image->at<IMAGE_TYPE>(
                                        index_t(x),
                                        index_t(y),
                                        index_t(z)
                                    ) = empty_value;
                                }
                            }
                            // Once the intersection reach, get the next one.
                            else
                            {
                                inside = !inside;
                                ++next_intersection;
                                // Once we found the last intersection, finish the image line.
                                if(next_intersection == intersection_end)
                                {
                                    if(inside)
                                    {
                                        for(std::int64_t zp = z ; zp < index_z_end ; ++zp)
                                        {
                                            _param.m_image->at<IMAGE_TYPE>(
                                                index_t(x),
                                                index_t(y),
                                                index_t(zp)
                                            ) = empty_value;
                                        }
                                    }

                                    break;
                                }
                            }
                        }
                    }
                }
            }
        };

    const auto y_loop =
        [&]()
        {
            // NOLINTNEXTLINE(clang-diagnostic-unknown-pragmas)
        #pragma omp parallel for
            for(std::int64_t x = index_x_beg ; x < index_x_end ; ++x)
            {
                for(std::int64_t z = index_z_beg ; z < index_z_end ; ++z)
                {
                    const auto ray_orig = glm::xyz(image_to_world_trf * (glm::vec4(x, index_y_beg, z, 1.0) + half));

                    std::vector<glm::vec3> intersections;
                    bool inside = get_intersections(ray_orig, orientation_y, intersections);

                    if(!intersections.empty())
                    {
                        auto next_intersection      = intersections.begin();
                        const auto intersection_end = intersections.end();
                        for(std::int64_t y = index_y_beg ; y < index_y_end ; ++y)
                        {
                            const auto voxel = glm::xyz(image_to_world_trf * (glm::vec4(x, y, z, 1.0) + half));

                            if(glm::distance(ray_orig, voxel) < glm::distance(ray_orig, *next_intersection))
                            {
                                if(inside)
                                {
                                    _param.m_image->at<IMAGE_TYPE>(
                                        index_t(x),
                                        index_t(y),
                                        index_t(z)
                                    ) = empty_value;
                                }
                            }
                            else
                            {
                                inside = !inside;
                                ++next_intersection;
                                if(next_intersection == intersection_end)
                                {
                                    if(inside)
                                    {
                                        for(std::int64_t yp = y ; yp < index_y_end ; ++yp)
                                        {
                                            _param.m_image->at<IMAGE_TYPE>(
                                                index_t(x),
                                                index_t(yp),
                                                index_t(z)
                                            ) = empty_value;
                                        }
                                    }

                                    break;
                                }
                            }
                        }
                    }
                }
            }
        };

    const auto x_loop =
        [&]()
        {
            // NOLINTNEXTLINE(clang-diagnostic-unknown-pragmas)
        #pragma omp parallel for
            for(std::int64_t y = index_y_beg ; y < index_y_end ; ++y)
            {
                for(std::int64_t z = index_z_beg ; z < index_z_end ; ++z)
                {
                    const auto ray_orig = glm::xyz(image_to_world_trf * (glm::vec4(index_x_beg, y, z, 1.0) + half));

                    std::vector<glm::vec3> intersections;
                    bool inside = get_intersections(ray_orig, orientation_x, intersections);

                    if(!intersections.empty())
                    {
                        auto next_intersection      = intersections.begin();
                        const auto intersection_end = intersections.end();
                        for(std::int64_t x = index_x_beg ; x < index_x_end ; ++x)
                        {
                            const auto voxel = glm::xyz(image_to_world_trf * (glm::vec4(x, y, z, 1.0) + half));

                            if(glm::distance(ray_orig, voxel) < glm::distance(ray_orig, *next_intersection))
                            {
                                if(inside)
                                {
                                    _param.m_image->at<IMAGE_TYPE>(
                                        index_t(x),
                                        index_t(y),
                                        index_t(z)
                                    ) = empty_value;
                                }
                            }
                            else
                            {
                                inside = !inside;
                                ++next_intersection;
                                if(next_intersection == intersection_end)
                                {
                                    if(inside)
                                    {
                                        for(std::int64_t xp = x ; xp < index_x_end ; ++xp)
                                        {
                                            _param.m_image->at<IMAGE_TYPE>(
                                                index_t(xp),
                                                index_t(y),
                                                index_t(z)
                                            ) = empty_value;
                                        }
                                    }

                                    break;
                                }
                            }
                        }
                    }
                }
            }
        };

    // Get the smallest dimension in terms of voxel to loop over the minimum of voxel.
    std::uint8_t axis = 2;
    auto voxel        = std::size_t((index_x_end - index_x_beg) * (index_y_end - index_y_beg));

    auto voxel_xz = std::size_t((index_x_end - index_x_beg) * (index_z_end - index_z_beg));
    auto voxel_yz = std::size_t((index_y_end - index_y_beg) * (index_z_end - index_z_beg));
    if(voxel_xz < voxel)
    {
        axis  = 1;
        voxel = voxel_xz;
    }

    if(voxel_yz < voxel)
    {
        axis  = 0;
        voxel = voxel_yz;
    }

    // Call the right loop.
    switch(axis)
    {
        case 2:
            z_loop();
            break;

        case 1:
            y_loop();
            break;

        case 0:
            x_loop();
            break;

        default:
            SIGHT_ASSERT("Unreachable code", false);
    }
}

} // namespace sight::filter::image

#ifdef __clang_analyzer__
#pragma clang diagnostic pop
#endif
