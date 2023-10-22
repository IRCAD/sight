/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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
    SIGHT_ASSERT("The image must be in 3 dimensions", _image->numDimensions() == 3);
    SIGHT_ASSERT("Spacing should be set", _image->getSpacing() != data::image::Spacing({0., 0., 0.}));

    Parameters param;
    param.m_image     = _image;
    param.m_mesh      = _mesh;
    param.m_mesh      = _mesh;
    param.m_transform = _transform;

    // We use a dispatcher because we can't retrieve the image type without a dynamic_t.
    core::type type = _image->getType();
    core::tools::dispatcher<core::tools::supported_dispatcher_types, image_extruder>::invoke(type, param);
}

//------------------------------------------------------------------------------

template<typename IMAGE_TYPE>
void image_extruder::operator()(Parameters& _param)
{
    auto transform = glm::identity<glm::mat4>();
    if(_param.m_transform)
    {
        // Apply the inverse matrix of the image to each point of the mesh
        const glm::dmat4x4 mat = sight::geometry::data::to_glm_mat(*_param.m_transform);
        transform = glm::inverse(mat);
    }

    // Creates triangles and bounding box of the mesh.
    const float min = std::numeric_limits<float>::lowest();
    const float max = std::numeric_limits<float>::max();

    std::list<Triangle> triangles;
    glm::vec3 min_bound(max, max, max);
    glm::vec3 max_bound(min, min, min);

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

            triangles.push_back(Triangle {tri_a, tri_b, tri_c});

            min_bound.x = std::min(std::min(std::min(min_bound.x, tri_a.x), tri_b.x), tri_c.x);
            min_bound.y = std::min(std::min(std::min(min_bound.y, tri_a.y), tri_b.y), tri_c.y);
            min_bound.z = std::min(std::min(std::min(min_bound.z, tri_a.z), tri_b.z), tri_c.z);

            max_bound.x = std::max(std::max(std::max(max_bound.x, tri_a.x), tri_b.x), tri_c.x);
            max_bound.y = std::max(std::max(std::max(max_bound.y, tri_a.y), tri_b.y), tri_c.y);
            max_bound.z = std::max(std::max(std::max(max_bound.z, tri_a.z), tri_b.z), tri_c.z);
        };

    auto it_point = _param.m_mesh->cbegin<data::iterator::point::xyz>();

    const auto cell_size = _param.m_mesh->getCellSize();
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

    // Get image informations.
    const data::image::Origin& origin   = _param.m_image->getOrigin();
    const data::image::Size& size       = _param.m_image->size();
    const data::image::Spacing& spacing = _param.m_image->getSpacing();

    // Loop over the bounding box intersection of the mesh and the image to increase performance.
    std::int64_t index_x_beg = 0;
    if(origin[0] < min_bound.x)
    {
        index_x_beg = static_cast<std::int64_t>((min_bound.x - origin[0]) / spacing[0]);
    }

    auto index_x_end = static_cast<std::int64_t>(size[0]);
    if(origin[0] + double(index_x_end) * spacing[0] > max_bound.x)
    {
        index_x_end = static_cast<std::int64_t>((max_bound.x - origin[0]) / spacing[0]);
    }

    std::int64_t index_y_beg = 0;
    if(origin[1] < min_bound.y)
    {
        index_y_beg = static_cast<std::int64_t>((min_bound.y - origin[1]) / spacing[1]);
    }

    auto index_y_end = std::int64_t(size[1]);
    if(origin[1] + double(index_y_end) * spacing[1] > max_bound.y)
    {
        index_y_end = static_cast<std::int64_t>((max_bound.y - origin[1]) / spacing[1]);
    }

    std::int64_t index_z_beg = 0;
    if(origin[2] < min_bound.z)
    {
        index_z_beg = static_cast<std::int64_t>((min_bound.z - origin[2]) / spacing[2]);
    }

    auto index_z_end = std::int64_t(size[2]);
    if(origin[2] + double(index_z_end) * spacing[2] > max_bound.z)
    {
        index_z_end = static_cast<std::int64_t>((max_bound.z - origin[2]) / spacing[2]);
    }

    // Check if the ray origin is inside or outside of the mesh and return all found intersections.
    const auto get_intersections =
        [&](const glm::vec3& _ray_orig, const glm::vec3& _ray_dir,
            std::vector<glm::vec3>& _intersections) -> bool
        {
            bool inside = false;
            for(const Triangle& tri : triangles)
            {
                glm::vec2 pos;
                float distance = NAN;
                if(glm::intersectRayTriangle(
                       _ray_orig,
                       _ray_dir,
                       tri.A,
                       tri.B,
                       tri.C,
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
                    const glm::vec3 ray_orig(origin[0] + double(x) * spacing[0] + spacing[0] / 2.F,
                                             origin[1] + double(y) * spacing[1] + spacing[1] / 2.F,
                                             origin[2] + double(index_z_beg) * spacing[2] + spacing[2] / 2.F);
                    const glm::vec3 ray_dir_pos(ray_orig.x, ray_orig.y, ray_orig.z + 1);
                    const glm::vec3 ray_dir = glm::normalize(ray_dir_pos - ray_orig);

                    // Check if the first voxel is inside or not, and stores all intersections.
                    std::vector<glm::vec3> intersections;
                    bool inside = get_intersections(ray_orig, ray_dir, intersections);

                    // If there is no intersection, the entire line is visible.
                    if(!intersections.empty())
                    {
                        // Iterate over the "ray" and check intersections to know if the voxel is inside
                        // or outside of the mesh.
                        auto next_intersection      = intersections.begin();
                        const auto intersection_end = intersections.end();
                        for(std::int64_t z = index_z_beg ; z < index_z_end ; ++z)
                        {
                            const glm::vec3 current_voxel_pos(origin[0] + double(x) * spacing[0] + spacing[0] / 2.F,
                                                              origin[1] + double(y) * spacing[1] + spacing[1] / 2.F,
                                                              origin[2] + double(z) * spacing[2] + spacing[2] / 2.F);
                            // While the current ray position is near to the next intersection, set the
                            // voxel to the value if
                            // it's needed.
                            if(glm::distance(ray_orig, current_voxel_pos) < glm::distance(ray_orig, *next_intersection))
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
                    const glm::vec3 ray_orig(origin[0] + double(x) * spacing[0] + spacing[0] / 2.F,
                                             origin[1] + double(index_y_beg) * spacing[1] + spacing[1] / 2.F,
                                             origin[2] + double(z) * spacing[2] + spacing[2] / 2.F);
                    const glm::vec3 ray_dir_pos(ray_orig.x, ray_orig.y + 1, ray_orig.z);
                    const glm::vec3 ray_dir = glm::normalize(ray_dir_pos - ray_orig);

                    std::vector<glm::vec3> intersections;
                    bool inside = get_intersections(ray_orig, ray_dir, intersections);

                    if(!intersections.empty())
                    {
                        auto next_intersection      = intersections.begin();
                        const auto intersection_end = intersections.end();
                        for(std::int64_t y = index_y_beg ; y < index_y_end ; ++y)
                        {
                            const glm::vec3 current_voxel_pos(origin[0] + double(x) * spacing[0] + spacing[0] / 2.F,
                                                              origin[1] + double(y) * spacing[1] + spacing[1] / 2.F,
                                                              origin[2] + double(z) * spacing[2] + spacing[2] / 2.F);
                            if(glm::distance(ray_orig, current_voxel_pos) < glm::distance(ray_orig, *next_intersection))
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
                    const glm::vec3 ray_orig(origin[0] + double(index_x_beg) * spacing[0] + spacing[0] / 2.F,
                                             origin[1] + double(y) * spacing[1] + spacing[1] / 2.F,
                                             origin[2] + double(z) * spacing[2] + spacing[2] / 2.F);
                    const glm::vec3 ray_dir_pos(ray_orig.x + 1, ray_orig.y, ray_orig.z);
                    const glm::vec3 ray_dir = glm::normalize(ray_dir_pos - ray_orig);

                    std::vector<glm::vec3> intersections;
                    bool inside = get_intersections(ray_orig, ray_dir, intersections);

                    if(!intersections.empty())
                    {
                        auto next_intersection      = intersections.begin();
                        const auto intersection_end = intersections.end();
                        for(std::int64_t x = index_x_beg ; x < index_x_end ; ++x)
                        {
                            const glm::vec3 current_voxel_pos(origin[0] + double(x) * spacing[0] + spacing[0] / 2.F,
                                                              origin[1] + double(y) * spacing[1] + spacing[1] / 2.F,
                                                              origin[2] + double(z) * spacing[2] + spacing[2] / 2.F);
                            if(glm::distance(ray_orig, current_voxel_pos) < glm::distance(ray_orig, *next_intersection))
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
