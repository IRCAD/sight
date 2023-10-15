/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include "geometry/data/Vector.hxx"

#include <core/base.hpp>

#include <data/matrix4.hpp>
#include <data/mesh.hpp>

namespace sight::geometry::data
{

/**
 * @brief   This helper generates a mesh using specified structure (quad or triangle).
 */
class mesh
{
public:

    typedef std::map<Point, sight::data::mesh::point_t> PointsMapType;
    typedef std::array<double, 3> PositionType;
    typedef std::array<std::size_t, 3> OrganizationType;

    /**
     * @brief Generate cell normals for the mesh.
     *
     * @param[out]  mesh data::mesh structure to fill with cell normals.
     */
    GEOMETRY_DATA_API static void generateCellNormals(sight::data::mesh::sptr mesh);

    /**
     * @brief Generate point normals for the mesh.
     *
     * @param[out]  mesh data::mesh structure to fill with cell normals.
     */
    GEOMETRY_DATA_API static void generatePointNormals(sight::data::mesh::sptr mesh);

    /**
     * @brief Shake point Normals.
     *
     * @param[out]  mesh data::mesh structure to shake normals.
     */
    GEOMETRY_DATA_API static void shakePointNormals(sight::data::mesh::sptr mesh);

    /**
     * @brief Shake cell Normals.
     *
     * @param[out]  mesh data::mesh structure to shake normals.
     */
    GEOMETRY_DATA_API static void shakeCellNormals(sight::data::mesh::sptr mesh);

    /**
     * @brief Shake points of the mesh.
     *
     * @param[out]  mesh data::mesh structure to shake.
     */
    GEOMETRY_DATA_API static void shakePoint(sight::data::mesh::sptr mesh);

    /**
     * @brief Colorize mesh (vertex point color).
     *
     * @param[in]  mesh data::mesh mesh structure to colorize.
     */
    GEOMETRY_DATA_API static void colorizeMeshPoints(sight::data::mesh::sptr mesh);

    /**
     * @brief Colorize mesh (cell color).
     *
     * @param[in]  mesh data::mesh mesh structure to colorize.
     */
    GEOMETRY_DATA_API static void colorizeMeshCells(sight::data::mesh::sptr mesh);

    /// Apply a transformation 4x4 from an input mesh to an output mesh
    GEOMETRY_DATA_API static void transform(
        sight::data::mesh::csptr inMesh,
        sight::data::mesh::sptr outMesh,
        const sight::data::matrix4& t
    );

    /// Apply a transformation 4x4 on a mesh
    GEOMETRY_DATA_API static void transform(sight::data::mesh::sptr mesh, const sight::data::matrix4& t);

    /**
     * @brief Colorize the mesh points with the given color
     * @pre point color array must be allocated
     * @pre mesh must only contain triangle
     */
    GEOMETRY_DATA_API static void colorizeMeshPoints(
        const sight::data::mesh::sptr& mesh,
        std::uint8_t colorR,
        std::uint8_t colorG,
        std::uint8_t colorB,
        std::uint8_t colorA = 255
    );

    /**
     * @brief Colorize the mesh points associated to the triangle in _vectorNumTriangle
     * @pre point color array must be allocated
     * @pre mesh must only contain triangle
     */
    GEOMETRY_DATA_API static void colorizeMeshPoints(
        const sight::data::mesh::sptr& _mesh,
        const std::vector<std::size_t>& _vectorNumTriangle,
        std::uint8_t _colorR,
        std::uint8_t colorG,
        std::uint8_t colorB,
        std::uint8_t _colorA = 255
    );

    /**
     * @brief Colorize the mesh cells with the given rgb color
     * @pre cell color array must be allocated
     * @pre mesh must only contain triangle
     */
    GEOMETRY_DATA_API static void colorizeMeshCells(
        const sight::data::mesh::sptr& mesh,
        std::uint8_t colorR,
        std::uint8_t colorG,
        std::uint8_t colorB,
        std::uint8_t _colorA = 255
    );
    /**
     * @brief Colorize the cells in vectorNumTriangle
     * @pre cell color array must be allocated
     * @pre mesh must only contain triangle
     */
    GEOMETRY_DATA_API static void colorizeMeshCells(
        const sight::data::mesh::sptr& mesh,
        const std::vector<std::size_t>& triangleIndexVector,
        std::uint8_t colorR,
        std::uint8_t colorG,
        std::uint8_t colorB,
        std::uint8_t _colorA = 255
    );

    /*!
     * @brief Return true if the mesh is closed.
     * The result is computed for each call.
     */
    GEOMETRY_DATA_API static bool isClosed(const sight::data::mesh::csptr& mesh);
};

} // namespace sight::geometry::data
