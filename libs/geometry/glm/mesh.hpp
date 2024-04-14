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

#include <sight/geometry/glm/config.hpp>

#include <boost/unordered_map.hpp>

#include <glm/glm.hpp>

namespace sight::geometry::glm
{

/**
 * @brief Converts an intersection point with a triangle from world coordinates to barycentric coordinates.
 * Assert if the _A_B_C triangle is flat to avoid zero division.
 * Note: no tests are performed in release mode to avoid performance issue.
 * @param _p world coordinates of the point (x, y, z), need to belongs to the ABC triangle.
 * @param _a first vertex of the triangle (_Ax, _Ay, _Az) (can also be called v1).
 * @param _b second vertex of the triangle (_Bx, _By, _Bz) (can also be called v2).
 * @param _c third vertex of the triangle (_Cx, _Cy, _Cz) (can also be called v3).
 * @return barycentric coordinates (u, v, w).
 */
SIGHT_GEOMETRY_GLM_API ::glm::dvec3 to_barycentric_coord(
    const ::glm::dvec3& _p,
    const ::glm::dvec3& _a,
    const ::glm::dvec3& _b,
    const ::glm::dvec3& _c
);

/**
 * @brief Converts from barycentric coordinates to world coordinates, assert if u+v+w isn't equal to 1.
 * Note: no tests are performed in release mode to avoid performance issue.
 * @param _bary_coord barycentric coordinates (u, v, w) that belongs to the _A_B_C triangle.
 * @param _a first vertex of the triangle (_Ax, _Ay, _Az) (can also be called v1).
 * @param _b second vertex of the triangle (_Bx, _By, _Bz) (can also be called v2).
 * @param _c third vertex of the triangle (_Cx, _Cy, _Cz) (can also be called v3).
 * @return world coordinates (x, y, z).
 */
SIGHT_GEOMETRY_GLM_API ::glm::dvec3 from_barycentric_coord(
    const ::glm::dvec3& _bary_coord,
    const ::glm::dvec3& _a,
    const ::glm::dvec3& _b,
    const ::glm::dvec3& _c
);

/**
 * @brief Converts the barycentric coordinates of a point inside a tetrahedron, from the world coordinates.
 * Assert if the _A_B_C_D triangle is flat to avoid zero division.
 * @note no tests are performed in release mode to avoid performance issue.
 * @param _p world coordinates of the point (x, y, z).
 * @param _a first vertex of the tetrahedron (_Ax, _Ay, _Az) (can also be called v1).
 * @param _b second vertex of the tetrahedron (_Bx, _By, _Bz) (can also be called v2).
 * @param _c third vertex of the tetrahedron (_Cx, _Cy, _Cz) (can also be called v3).
 * @param _d fourth vertex of the tetrahedron (_Dx, _Dy, _Dz) (can also be called v4).
 * @return barycentric coordinates (u, v, w, h).
 */
SIGHT_GEOMETRY_GLM_API ::glm::dvec4 to_barycentric_coord(
    const ::glm::dvec3& _p,
    const ::glm::dvec3& _a,
    const ::glm::dvec3& _b,
    const ::glm::dvec3& _c,
    const ::glm::dvec3& _d
);

/**
 * @brief Converts from barycentric coordinates to world coordinates, assert if u+v+w+h isn't equal to 1.
 * @note no tests are performed in release mode to avoid performance issue.
 * @param _bary_coord barycentric coordinates (u, v, w, h) that belongs to the _A_B_C_D tetrahedron.
 * @param _a first vertex of the tetrahedron (_Ax, _Ay, _Az) (can also be called v1).
 * @param _b second vertex of the tetrahedron (_Bx, _By, _Bz) (can also be called v2).
 * @param _c third vertex of the tetrahedron (_Cx, _Cy, _Cz) (can also be called v3).
 * @param _d fourth vertex of the tetrahedron (_Dx, _Dy, _Dz) (can also be called v4).
 * @return world coordinates (x, y, z).
 */
SIGHT_GEOMETRY_GLM_API ::glm::dvec3 from_barycentric_coord(
    const ::glm::dvec4& _bary_coord,
    const ::glm::dvec3& _a,
    const ::glm::dvec3& _b,
    const ::glm::dvec3& _c,
    const ::glm::dvec3& _d
);

/**
 * @brief Determine if the point _P is inside the tetrahedron made of (ABCD).
 * @param _p world coordinates of the point (x, y, z).
 * @param _a first vertex of the tetrahedron (_Ax, _Ay, _Az).
 * @param _b second vertex of the tetrahedron (_Bx, _By, _Bz).
 * @param _c third vertex of the tetrahedron (_Cx, _Cy, _Cz).
 * @param _d fourth vertex of the tetrahedron (_Dx, _Dy, _Dz).
 * @return true if the point _P is inside the tetrahedron (ABCD) false otherwise.
 */
SIGHT_GEOMETRY_GLM_API bool is_inside_tetrahedron(
    const ::glm::dvec3& _p,
    const ::glm::dvec3& _a,
    const ::glm::dvec3& _b,
    const ::glm::dvec3& _c,
    const ::glm::dvec3& _d
);

/**
 * @brief Determine if the point _P is inside the tetrahedron made of (ABCD) based on P barycentric coordinates in
 *(ABCD).
 * @param _barycentric_coord_p_inside_abcd: P coordinates expressed as barycentric coordinates in (ABCD).
 * @return true if the point _P is inside the tetrahedron (ABCD) false otherwise.
 */
SIGHT_GEOMETRY_GLM_API bool is_inside_tetrahedron(::glm::dvec4 _barycentric_coord_p_inside_abcd);
//-----------------------------------------------------------------------------

template<typename T, typename U>
std::pair<T, U> make_ordered_pair(const T _first, const U _second)
{
    if(_first < _second)
    {
        return std::pair<T, U>(_first, _second);
    }

    return std::pair<T, U>(_second, _first);
}

//-----------------------------------------------------------------------------

template<typename T, typename U, typename V>
bool is_borderless_surface(
    T* _cell_data_begin,
    T* _cell_data_end,
    U* _cell_data_offsets_begin,
    U* _cell_data_offsets_end,
    V* _cell_types_begin
)
{
    typedef std::pair<T, T> Edge; // always Edge.first < Edge.second !!
    typedef boost::unordered_map<Edge, int> EdgeHistogram;
    EdgeHistogram edges_histogram;
    bool is_borderless = true;

    U* iter           = _cell_data_offsets_begin;
    U* iter2          = _cell_data_offsets_begin + 1;
    const U* iter_end = _cell_data_offsets_end - 1;
    V* iter_types     = _cell_types_begin;

    std::size_t data_len = *iter2 - *iter;
    for( ;
         iter < iter_end
         || (iter < _cell_data_offsets_end && (data_len = (_cell_data_end - _cell_data_begin) - *iter)) ;
         data_len = *++iter2 - *++iter, ++iter_types)
    {
        if(*iter_types == 0)
        {
            continue;
        }

        T* iter_cell           = _cell_data_begin + *iter;
        T* iter_cell2          = iter_cell + 1;
        T* begin_cell          = iter_cell;
        const T* iter_cell_end = begin_cell + data_len - 1;
        for( ;
             iter_cell < iter_cell_end || (iter_cell < (begin_cell + data_len) && (iter_cell2 = begin_cell)) ;
             ++iter_cell, ++iter_cell2)
        {
            ++edges_histogram[makeOrderedPair(*iter_cell, *(iter_cell2))];
        }
    }

    for(const auto& h : edges_histogram)
    {
        if(h.second != 2)
        {
            is_borderless = false;
            break;
        }
    }

    return is_borderless;
}

} // namespace sight::geometry::glm
