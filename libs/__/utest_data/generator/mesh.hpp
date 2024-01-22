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

#include "utest_data/config.hpp"

#include <core/base.hpp>

#include <data/mesh.hpp>

namespace sight::utest_data::generator
{

/**
 * @brief   This helper generates a mesh using specified structure (quad or triangle).
 */
class mesh
{
public:

    using points_map_t = std::map<std::array<float, 3>, data::mesh::point_t>;

    /**
     * @brief Generate a mesh.
     * @param _mesh mesh to generate
     */
    UTEST_DATA_API static void generate_mesh(const data::mesh::sptr& _mesh);

    /**
     * @brief Generate a quad mesh.
     *
     * This method generates synthetic mesh with quads (two faces of a cube).
     *
     * @param[out] _mesh data::mesh empty mesh structure to fill with quad cells.
     * @param[in]  _attributes attributes of the mesh (POINT_COLORS, point_normals, ...)
     */
    UTEST_DATA_API static void generate_quad_mesh(
        const data::mesh::sptr& _mesh,
        data::mesh::attribute _attributes = data::mesh::attribute::none
    );

    /**
     * @brief Generate a triangle mesh
     *
     * This method generates synthetic mesh with triangles (two faces of a cube).
     *
     * @param[out]  _mesh data::mesh empty mesh structure to fill with triangle cell.
     * @param[in]   _attributes attributes of the mesh (POINT_COLORS, point_normals, ...)
     */
    UTEST_DATA_API static void generate_triangle_mesh(
        const data::mesh::sptr& _mesh,
        data::mesh::attribute _attributes = data::mesh::attribute::none
    );

    /**
     * @brief Generate a mesh with quad and triangle cells.
     *
     * This method generates synthetic mesh with triangles and quads (four faces of a cube).
     *
     * @param[out]  _mesh data::mesh empty mesh structure to fill with quad and triangle cells.
     * @param[in]   _attributes attributes of the mesh (POINT_COLORS, point_normals, ...)
     */
    UTEST_DATA_API static void generate_triangle_quad_mesh(
        const data::mesh::sptr& _mesh,
        data::mesh::attribute _attributes = data::mesh::attribute::none
    );

private:

    /**
     * @brief Add quad cells in mesh, this method generates synthetic data (two face of a cube).
     *
     * @param[out]  _mesh data::mesh Mesh structure to fill with quad cells.
     * @param[in]   _points points_map_t Point registration map used to not create point which already in mesh buffer.
     * @param[in]   _nb_points_by_edge std::size_t Number of points by edge.
     * @param[in]   _edge_dim float Edge dimension in 3D world.
     */

    static void add_quad_mesh(
        const data::mesh::sptr& _mesh,
        points_map_t& _points,
        std::size_t _nb_points_by_edge = 10,
        float _edge_dim                = 100.F
    );

    /**
     * @brief Add triangle cells in mesh, this method generates synthetic data (two face of a cube).
     *
     * @param[out]  _mesh data::mesh Mesh structure to fill with triangle cells.
     * @param[in]   _points points_map_t Point registration map used to not create point which already in mesh buffer.
     * @param[in]   _nb_points_by_edge std::size_t Number of points by edge.
     * @param[in]   _edge_dim float Edge dimension in 3D world.
     */
    static void add_triangle_mesh(
        const data::mesh::sptr& _mesh,
        points_map_t& _points,
        std::size_t _nb_points_by_edge = 10,
        float _edge_dim                = 100.F
    );

    static data::mesh::point_t add_point(
        const data::mesh::position_t* _pt,
        const data::mesh::sptr& _mesh,
        points_map_t& _points
    );
    /**
     * @brief Shake points of the mesh.
     *
     * @param[out]  _mesh data::mesh structure to shake.
     */
    static void shake_points(const data::mesh::sptr& _mesh);
};

} // namespace sight::utest_data::generator
