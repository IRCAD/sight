/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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
#include "geometry/data/types.hpp"

#include <core/base.hpp>

#include <boost/unordered_map.hpp>

#include <cmath>
#include <utility>
#include <vector>

namespace sight::geometry::data
{

/**
 * @brief
 */
GEOMETRY_DATA_API bool is_inclosed_volume(
    const fw_vertex_position& _vertex,
    const fw_vertex_index& _vertex_index,
    const fw_vec3d& _p
);

//cspell: disable
/**
 * @brief Compute the intersection between triangle(defined by three vertices vert1, vert2, vert3) and the Oz parallel
 * line and cross by the point P.
 * The function return true if intersection is found (false otherwise).
 * t is the oriented distance of P to the intersection point.
 * u and v is the coordinate of the intersection point in the (O, u, v) triangle coordinate system with :
 * O = vert1. u =  vert2 - O. v =  vert3 - O.
 * "Fast, Minimum Storage Ray/Triangle Intersection", Tomas Muller, Ben Trumbore.
 *
 */
//cspell: enable
GEOMETRY_DATA_API bool intersect_triangle(
    fw_vec3d _orig,
    fw_vec3d _dir,
    fw_vec3d _vert0,
    fw_vec3d _vert1,
    fw_vec3d _vert2,
    double& _t,
    double& _u,
    double& _v
);

/// test whatever a vertex is duplicated or not
GEOMETRY_DATA_API bool is_borderless_surface(const fw_vertex_index& _vertex_index);

GEOMETRY_DATA_API void find_border_edges(
    const fw_vertex_index& _vertex_index,
    std::vector<std::vector<std::pair<int,
                                      int> > >& _contours
);

/**
 * @brief Closes the surface if necessary
 * @return True if container mesh is changed
 */
GEOMETRY_DATA_API bool close_surface(fw_vertex_position& _vertex, fw_vertex_index& _vertex_index);

/**
 * remove orphan vertices (i.e not used in _vertexIndex), _vertexIndex is reordered according to vertex suppressions
 */
GEOMETRY_DATA_API bool remove_orphan_vertices(fw_vertex_position& _vertex, fw_vertex_index& _vertex_index);

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

    std::size_t data_len = 0;
    U* iter              = _cell_data_offsets_begin;
    U* iter2             = _cell_data_offsets_begin + 1;
    const U* iter_end    = _cell_data_offsets_end - 1;
    V* iter_types        = _cell_types_begin;

    data_len = *iter2 - *iter;
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

} // namespace sight::geometry::data
