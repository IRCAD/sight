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

#include "geometry/glm/config.hpp"

#include <glm/glm.hpp>

#include <boost/unordered_map.hpp>

namespace sight::geometry::glm
{

/**
 * @brief Converts an intersection point with a triangle from world coordinates to barycentric coordinates.
 * Assert if the _A_B_C triangle is flat to avoid zero division.
 * Note: no tests are performed in release mode to avoid performance issue.
 * @param _P world coordinates of the point (x, y, z), need to belongs to the ABC triangle.
 * @param _A first vertex of the triangle (_Ax, _Ay, _Az) (can also be called v1).
 * @param _B second vertex of the triangle (_Bx, _By, _Bz) (can also be called v2).
 * @param _C third vertex of the triangle (_Cx, _Cy, _Cz) (can also be called v3).
 * @return barycentric coordinates (u, v, w).
 */
GEOMETRY_GLM_API ::glm::dvec3 toBarycentricCoord(
    const ::glm::dvec3& _P,
    const ::glm::dvec3& _A,
    const ::glm::dvec3& _B,
    const ::glm::dvec3& _C
);

/**
 * @brief Converts from barycentric coordinates to world coordinates, assert if u+v+w isn't equal to 1.
 * Note: no tests are performed in release mode to avoid performance issue.
 * @param _baryCoord barycentric coordinates (u, v, w) that belongs to the _A_B_C triangle.
 * @param _A first vertex of the triangle (_Ax, _Ay, _Az) (can also be called v1).
 * @param _B second vertex of the triangle (_Bx, _By, _Bz) (can also be called v2).
 * @param _C third vertex of the triangle (_Cx, _Cy, _Cz) (can also be called v3).
 * @return world coordinates (x, y, z).
 */
GEOMETRY_GLM_API ::glm::dvec3 fromBarycentricCoord(
    const ::glm::dvec3& _baryCoord,
    const ::glm::dvec3& _A,
    const ::glm::dvec3& _B,
    const ::glm::dvec3& _C
);

/**
 * @brief Converts the barycentric coordinates of a point inside a tetrahedron, from the world coordinates.
 * Assert if the _A_B_C_D triangle is flat to avoid zero division.
 * @note no tests are performed in release mode to avoid performance issue.
 * @param _P world coordinates of the point (x, y, z).
 * @param _A first vertex of the tetrahedron (_Ax, _Ay, _Az) (can also be called v1).
 * @param _B second vertex of the tetrahedron (_Bx, _By, _Bz) (can also be called v2).
 * @param _C third vertex of the tetrahedron (_Cx, _Cy, _Cz) (can also be called v3).
 * @param _D fourth vertex of the tetrahedron (_Dx, _Dy, _Dz) (can also be called v4).
 * @return barycentric coordinates (u, v, w, h).
 */
GEOMETRY_GLM_API ::glm::dvec4 toBarycentricCoord(
    const ::glm::dvec3& _P,
    const ::glm::dvec3& _A,
    const ::glm::dvec3& _B,
    const ::glm::dvec3& _C,
    const ::glm::dvec3& _D
);

/**
 * @brief Converts from barycentric coordinates to world coordinates, assert if u+v+w+h isn't equal to 1.
 * @note no tests are performed in release mode to avoid performance issue.
 * @param _baryCoord barycentric coordinates (u, v, w, h) that belongs to the _A_B_C_D tetrahedron.
 * @param _A first vertex of the tetrahedron (_Ax, _Ay, _Az) (can also be called v1).
 * @param _B second vertex of the tetrahedron (_Bx, _By, _Bz) (can also be called v2).
 * @param _C third vertex of the tetrahedron (_Cx, _Cy, _Cz) (can also be called v3).
 * @param _C fourth vertex of the tetrahedron (_Dx, _Dy, _Dz) (can also be called v4).
 * @return world coordinates (x, y, z).
 */
GEOMETRY_GLM_API ::glm::dvec3 fromBarycentricCoord(
    const ::glm::dvec4& _baryCoord,
    const ::glm::dvec3& _A,
    const ::glm::dvec3& _B,
    const ::glm::dvec3& _C,
    const ::glm::dvec3& _D
);

/**
 * @brief Determine if the point _P is inside the tetrahedron made of (ABCD).
 * @param _P world coordinates of the point (x, y, z).
 * @param _A first vertex of the tetrahedron (_Ax, _Ay, _Az).
 * @param _B second vertex of the tetrahedron (_Bx, _By, _Bz).
 * @param _C third vertex of the tetrahedron (_Cx, _Cy, _Cz).
 * @param _D fourth vertex of the tetrahedron (_Dx, _Dy, _Dz).
 * @return true if the point _P is inside the tetrahedron (ABCD) false otherwise.
 */
GEOMETRY_GLM_API bool isInsideTetrahedron(
    const ::glm::dvec3& _P,
    const ::glm::dvec3& _A,
    const ::glm::dvec3& _B,
    const ::glm::dvec3& _C,
    const ::glm::dvec3& _D
);

/**
 * @brief Determine if the point _P is inside the tetrahedron made of (ABCD) based on P barycentric coordinates in
 *(ABCD).
 * @param barycentricCoordPInsideABCD: P coordinates expressed as barycentric coordinates in (ABCD).
 * @return true if the point _P is inside the tetrahedron (ABCD) false otherwise.
 */
GEOMETRY_GLM_API bool isInsideTetrahedron(const ::glm::dvec4 barycentricCoordPInsideABCD);
//-----------------------------------------------------------------------------

template<typename T, typename U>
std::pair<T, U> makeOrderedPair(const T first, const U second)
{
    if(first < second)
    {
        return std::pair<T, U>(first, second);
    }
    else
    {
        return std::pair<T, U>(second, first);
    }
}

//-----------------------------------------------------------------------------

template<typename T, typename U, typename V>
bool isBorderlessSurface(
    T* cellDataBegin,
    T* cellDataEnd,
    U* cellDataOffsetsBegin,
    U* cellDataOffsetsEnd,
    V* cellTypesBegin
)
{
    typedef std::pair<T, T> Edge; // always Edge.first < Edge.second !!
    typedef ::boost::unordered_map<Edge, int> EdgeHistogram;
    EdgeHistogram edgesHistogram;
    bool isBorderless = true;

    size_t dataLen   = 0;
    U* iter          = cellDataOffsetsBegin;
    U* iter2         = cellDataOffsetsBegin + 1;
    const U* iterEnd = cellDataOffsetsEnd - 1;
    V* iterTypes     = cellTypesBegin;

    dataLen = *iter2 - *iter;
    for( ;
         iter < iterEnd || (iter < cellDataOffsetsEnd && (dataLen = (cellDataEnd - cellDataBegin) - *iter)) ;
         dataLen = *++iter2 - *++iter, ++iterTypes)
    {
        if(*iterTypes == 0)
        {
            continue;
        }

        T* iterCell          = cellDataBegin + *iter;
        T* iterCell2         = iterCell + 1;
        T* beginCell         = iterCell;
        const T* iterCellEnd = beginCell + dataLen - 1;
        for( ;
             iterCell < iterCellEnd || (iterCell < (beginCell + dataLen) && (iterCell2 = beginCell)) ;
             ++iterCell, ++iterCell2)
        {
            ++edgesHistogram[makeOrderedPair(*iterCell, *(iterCell2))];
        }
    }

    for(const typename EdgeHistogram::value_type& histo : edgesHistogram)
    {
        if(histo.second != 2)
        {
            isBorderless = false;
            break;
        }
    }

    return isBorderless;
}

} // namespace sight::geometry
