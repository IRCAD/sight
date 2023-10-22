/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include <data/array.hpp>
#include <data/matrix4.hpp>
#include <data/point_list.hpp>

namespace sight::geometry::data
{

/**
 * @brief Defines a helper to modify a sight::data::point_list.
 */
class GEOMETRY_DATA_CLASS_API point_list
{
public:

    /// Constructor. Does nothing.
    GEOMETRY_DATA_API point_list() = default;

    /// Destructor. Does nothing.
    GEOMETRY_DATA_API ~point_list() = default;

    /**
     * @brief Computes the point-to-point distance between 2 pointlists
     * @param[in] _pointList1 first point list
     * @param[in] _pointList2 second point list
     * @return array of the size of one the pointlists (they must have the same size)
     */
    GEOMETRY_DATA_API static sight::data::array::sptr computeDistance(
        sight::data::point_list::sptr _point_list1,
        sight::data::point_list::sptr _point_list2
    );

    /**
     * @brief Transform a pointList with a transformation matrix
     * @param[in] _pointList pointlist to be transformed
     * @param[in] _matrix transformation to apply to each points in pointlist
     */
    GEOMETRY_DATA_API static void transform(
        sight::data::point_list::sptr& _point_list,
        const sight::data::matrix4::csptr& _matrix
    );

    /**
     * @brief Associate 2 pointLists:
     * Take 2 pointLists as input and re-order the second one, so that the points at the
     * same index on both lists are the closest to each other
     * @param[in] _pointList1 first pointlist
     * @param[in] _pointList2 pointlist that will be re-ordered
     */
    GEOMETRY_DATA_API static void associate(
        const sight::data::point_list::csptr& _point_list1,
        sight::data::point_list::sptr _point_list2
    );

    /**
     * @brief removeClosestPoint: removes the closest point from a reference point
     * @param[in] _pointList: the point list
     * @param[in] _point: used to find the closest point in the list of points
     * @param[in] _delta: the maximum tolerance  between the reference point and the point to find
     * @return the removed point or nullptr if no point has been removed
     */
    GEOMETRY_DATA_API static sight::data::point::sptr removeClosestPoint(
        const sight::data::point_list::sptr& _point_list,
        const sight::data::point::csptr& _point,
        float _delta
    );
};

} // namespace sight::geometry::data
