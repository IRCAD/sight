/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#pragma once

#include "geometry/data/config.hpp"

#include <core/base.hpp>

#include <data/matrix4.hpp>
#include <data/mesh.hpp>

#include <glm/vec3.hpp>

namespace sight::geometry::data
{

/**
 * @brief   This helper generates a mesh using specified structure (quad or triangle).
 */
class mesh
{
public:

    using points_map_t   = std::map<glm::vec3, sight::data::mesh::point_t>;
    using position_t     = std::array<double, 3>;
    using organization_t = std::array<std::size_t, 3>;

    /**
     * @brief Generate cell normals for the mesh.
     *
     * @param[out]  _mesh data::mesh structure to fill with cell normals.
     */
    GEOMETRY_DATA_API static void generate_cell_normals(sight::data::mesh::sptr _mesh);

    /**
     * @brief Generate point normals for the mesh.
     *
     * @param[out]  _mesh data::mesh structure to fill with cell normals.
     */
    GEOMETRY_DATA_API static void generate_point_normals(sight::data::mesh::sptr _mesh);

    /**
     * @brief Shake point Normals.
     *
     * @param[out]  _mesh data::mesh structure to shake normals.
     */
    GEOMETRY_DATA_API static void shake_point_normals(sight::data::mesh::sptr _mesh);

    /**
     * @brief Shake cell Normals.
     *
     * @param[out]  _mesh data::mesh structure to shake normals.
     */
    GEOMETRY_DATA_API static void shake_cell_normals(sight::data::mesh::sptr _mesh);

    /**
     * @brief Shake points of the mesh.
     *
     * @param[out]  _mesh data::mesh structure to shake.
     */
    GEOMETRY_DATA_API static void shake_point(sight::data::mesh::sptr _mesh);

    /**
     * @brief Colorize mesh (vertex point color).
     *
     * @param[in]  _mesh data::mesh mesh structure to colorize.
     */
    GEOMETRY_DATA_API static void colorize_mesh_points(sight::data::mesh::sptr _mesh);

    /**
     * @brief Colorize mesh (cell color).
     *
     * @param[in]  _mesh data::mesh mesh structure to colorize.
     */
    GEOMETRY_DATA_API static void colorize_mesh_cells(sight::data::mesh::sptr _mesh);

    /// Apply a transformation 4x4 from an input mesh to an output mesh
    GEOMETRY_DATA_API static void transform(
        sight::data::mesh::csptr _in_mesh,
        sight::data::mesh::sptr _out_mesh,
        const sight::data::matrix4& _t
    );

    /// Apply a transformation 4x4 on a mesh
    GEOMETRY_DATA_API static void transform(sight::data::mesh::sptr _mesh, const sight::data::matrix4& _t);

    /**
     * @brief Colorize the mesh points with the given color
     * @pre point color array must be allocated
     * @pre mesh must only contain triangle
     */
    GEOMETRY_DATA_API static void colorize_mesh_points(
        const sight::data::mesh::sptr& _mesh,
        std::uint8_t _color_r,
        std::uint8_t _color_g,
        std::uint8_t _color_b,
        std::uint8_t _color_a = 255
    );

    /**
     * @brief Colorize the mesh points associated to the triangle in _vectorNumTriangle
     * @pre point color array must be allocated
     * @pre mesh must only contain triangle
     */
    GEOMETRY_DATA_API static void colorize_mesh_points(
        const sight::data::mesh::sptr& _mesh,
        const std::vector<std::size_t>& _vector_num_triangle,
        std::uint8_t _color_r,
        std::uint8_t _color_g,
        std::uint8_t _color_b,
        std::uint8_t _color_a = 255
    );

    /**
     * @brief Colorize the mesh cells with the given rgb color
     * @pre cell color array must be allocated
     * @pre mesh must only contain triangle
     */
    GEOMETRY_DATA_API static void colorize_mesh_cells(
        const sight::data::mesh::sptr& _mesh,
        std::uint8_t _color_r,
        std::uint8_t _color_g,
        std::uint8_t _color_b,
        std::uint8_t _color_a = 255
    );
    /**
     * @brief Colorize the cells in vectorNumTriangle
     * @pre cell color array must be allocated
     * @pre mesh must only contain triangle
     */
    GEOMETRY_DATA_API static void colorize_mesh_cells(
        const sight::data::mesh::sptr& _mesh,
        const std::vector<std::size_t>& _triangle_index_vector,
        std::uint8_t _color_r,
        std::uint8_t _color_g,
        std::uint8_t _color_b,
        std::uint8_t _color_a = 255
    );

    /*!
     * @brief Return true if the mesh is closed.
     * The result is computed for each call.
     */
    GEOMETRY_DATA_API static bool is_closed(const sight::data::mesh::csptr& _mesh);
};

} // namespace sight::geometry::data
