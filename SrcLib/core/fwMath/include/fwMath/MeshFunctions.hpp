/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWMATH_MESHFUNCTIONS_HPP__
#define __FWMATH_MESHFUNCTIONS_HPP__

#include "fwMath/IntrasecTypes.hpp"
#include "fwMath/config.hpp"

#include <fwCore/base.hpp>

#include <boost/unordered_map.hpp>
#include <cmath>
#include <utility>
#include <vector>

namespace fwMath
{

/**
 * @brief
 */
FWMATH_API bool IsInclosedVolume(const fwVertexPosition &_vertex, const fwVertexIndex &_vertexIndex,
                                 const fwVec3d &_p );

/**
 * @brief Compute the intersection between triangle(define by threes vertex vert1, vert2, vert3) and the Oz parallel line and cross by the point P.
 * The function return true if intersection is found (false otherwise).
 * t is the oriented distance of P to the intersection point.
 * u and v is the coordinate of the intersection point in the (O, u, v) triangle coordinate system with :
 * O = vert1. u =  vert2 - O. v =  vert3 - O.
 * "Fast, Minimum Storage Ray/Triangle Intersection", Tomas Muller, Ben Trumbore.
 *
 */
FWMATH_API bool intersect_triangle(fwVec3d _orig, fwVec3d _dir, fwVec3d _vert0, fwVec3d _vert1, fwVec3d _vert2,
                                   double &_t, double &_u, double &_v);

/// test whatever a vertex is duplicated or not
FWMATH_API bool isBorderlessSurface( const fwVertexIndex &_vertexIndex);

FWMATH_API void findBorderEdges( const fwVertexIndex &_vertexIndex, std::vector< std::vector<  std::pair< int,
                                                                                                          int  > > > &contours);

/**
 * @brief Closes the surface if necessary
 * @return True if container mesh is changed
 */
FWMATH_API bool closeSurface( fwVertexPosition &_vertex, fwVertexIndex &_vertexIndex );

/**
 * remove orphan vertices (i.e not used in _vertexIndex), _vertexIndex is reordered according to vertex suppressions
 */
FWMATH_API bool removeOrphanVertices( fwVertexPosition &_vertex, fwVertexIndex &_vertexIndex );

//-----------------------------------------------------------------------------

template <typename T, typename U>
std::pair< T, U > makeOrderedPair(const T first, const U second)
{
    if (first < second)
    {
        return std::pair< T, U >(first, second);
    }
    else
    {
        return std::pair< T, U >(second, first);
    }
}

//-----------------------------------------------------------------------------

template <typename T, typename U, typename V>
bool isBorderlessSurface(T* cellDataBegin, T* cellDataEnd, U* cellDataOffsetsBegin, U* cellDataOffsetsEnd,
                         V* cellTypesBegin)
{
    typedef std::pair< T, T >  Edge; // always Edge.first < Edge.second !!
    typedef ::boost::unordered_map< Edge, int >  EdgeHistogram;
    EdgeHistogram edgesHistogram;
    bool isBorderless = true;

    size_t dataLen   = 0;
    U* iter          = cellDataOffsetsBegin;
    U* iter2         = cellDataOffsetsBegin + 1;
    const U* iterEnd = cellDataOffsetsEnd - 1;
    V* iterTypes     = cellTypesBegin;

    dataLen = *iter2 - *iter;
    for (
        ;
        iter < iterEnd || ( iter < cellDataOffsetsEnd && (dataLen = (cellDataEnd - cellDataBegin) - *iter) );
        dataLen = *++iter2 - *++iter, ++iterTypes
        )
    {
        if(*iterTypes == 0)
        {
            continue;
        }
        T* iterCell          = cellDataBegin + *iter;
        T* iterCell2         = iterCell + 1;
        T* beginCell         = iterCell;
        const T* iterCellEnd = beginCell + dataLen - 1;
        for (
            ;
            iterCell < iterCellEnd || ( iterCell < (beginCell + dataLen) && (iterCell2 = beginCell) );
            ++iterCell, ++iterCell2
            )
        {
            ++edgesHistogram[makeOrderedPair(*iterCell, *(iterCell2))];
        }
    }

    for(const typename EdgeHistogram::value_type &histo : edgesHistogram)
    {
        if (histo.second != 2)
        {
            isBorderless = false;
            break;
        }
    }

    return isBorderless;
}

}

#endif /* __FWMATH_MESHFUNCTIONS_HPP__ */
