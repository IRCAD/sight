/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "utestData/config.hpp"

#include <core/base.hpp>

#include <data/Mesh.hpp>

namespace sight::utestData
{
namespace generator
{

/**
 * @brief   This helper generates a mesh using specified structure (quad or triangle).
 */
class Mesh
{

public:

    typedef std::map< std::array<float, 3>, data::Mesh::PointId> PointsMapType;

    /**
     * @brief Initialize 'rand' seed
     */
    UTESTDATA_API static void initRand();

    /**
     * @brief Generate a mesh.
     * @param mesh mesh to generate
     */
    UTESTDATA_API static void generateMesh( const data::Mesh::sptr& mesh );

    /**
     * @brief Generate a quad mesh.
     *
     * This method generates synthetic mesh with quads (two faces of a cube).
     *
     * @param[out]  mesh data::Mesh empty mesh structure to fill with quad cells.
     * @param[in]  attributes attributes of the mesh (POINT_COLORS, POINT_NORMALS, ...)
     */
    UTESTDATA_API static void generateQuadMesh(
        const data::Mesh::sptr& mesh,
        data::Mesh::Attributes attributes = data::Mesh::Attributes::NONE);

    /**
     * @brief Generate a triangle mesh
     *
     * This method generates synthetic mesh with triangles (two faces of a cube).
     *
     * @param[out]  mesh data::Mesh empty mesh structure to fill with triangle cell.
     * @param[in]  attributes attributes of the mesh (POINT_COLORS, POINT_NORMALS, ...)
     */
    UTESTDATA_API static void generateTriangleMesh(
        const data::Mesh::sptr& mesh,
        data::Mesh::Attributes attributes = data::Mesh::Attributes::NONE);

    /**
     * @brief Generate a mesh with quad and triangle cells.
     *
     * This method generates synthetic mesh with triangles and quads (four faces of a cube).
     *
     * @param[out]  mesh data::Mesh empty mesh structure to fill with quad and triangle cells.
     * @param[in]  attributes attributes of the mesh (POINT_COLORS, POINT_NORMALS, ...)
     */
    UTESTDATA_API static void generateTriangleQuadMesh(
        const data::Mesh::sptr& mesh,
        data::Mesh::Attributes attributes = data::Mesh::Attributes::NONE);

private:

    /**
     * @brief Add quad cells in mesh, this method generates synthetic data (two face of a cube).
     *
     * @param[out]  mesh data::Mesh Mesh structure to fill with quad cells.
     * @param[in]   points PointsMapType Point registration map used to not create point which already in mesh buffer.
     * @param[in]   nbPointsByEdge size_t Number of points by edge.
     * @param[in]   edgeDim float Edge dimension in 3D world.
     */

    static void addQuadMesh(const data::Mesh::sptr& mesh,
                            PointsMapType& points,
                            size_t nbPointsByEdge = 10,
                            float edgeDim         = 100.f);

    /**
     * @brief Add triangle cells in mesh, this method generates synthetic data (two face of a cube).
     *
     * @param[out]  mesh data::Mesh Mesh structure to fill with triangle cells.
     * @param[in]   points PointsMapType Point registration map used to not create point which already in mesh buffer.
     * @param[in]   nbPointsByEdge size_t Number of points by edge.
     * @param[in]   edgeDim float Edge dimension in 3D world.
     */
    static void addTriangleMesh(const data::Mesh::sptr& mesh,
                                PointsMapType& points,
                                size_t nbPointsByEdge = 10,
                                float edgeDim         = 100.f);

    static data::Mesh::PointId addPoint(const data::Mesh::PointValueType* pt,
                                        const data::Mesh::sptr& mesh,
                                        PointsMapType& points);
    /**
     * @brief Shake points of the mesh.
     *
     * @param[out]  mesh data::Mesh structure to shake.
     */
    static void shakePoints(const data::Mesh::sptr& mesh);
};

} // namespace generator
} // namespace sight::utestData
