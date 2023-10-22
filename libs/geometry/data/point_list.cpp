/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "geometry/data/point_list.hpp"

#include <data/point.hpp>
#include <data/point_list.hpp>

#include <geometry/data/matrix4.hpp>

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include <cmath>
#include <list>

namespace sight::geometry::data
{

//-----------------------------------------------------------------------------

sight::data::array::sptr point_list::computeDistance(
    sight::data::point_list::sptr _point_list1,
    sight::data::point_list::sptr _point_list2
)
{
    SIGHT_ASSERT(
        "the 2 pointLists must have the same number of points",
        _point_list1->getPoints().size() == _point_list2->getPoints().size()
    );

    const sight::data::point_list::PointListContainer points1 = _point_list1->getPoints();
    const sight::data::point_list::PointListContainer points2 = _point_list2->getPoints();
    const std::size_t size                                    = points1.size();

    sight::data::array::sptr output_array = std::make_shared<sight::data::array>();
    output_array->resize({size}, sight::core::type::DOUBLE);
    const auto dump_lock    = output_array->dump_lock();
    auto distance_array_itr = output_array->begin<double>();

    for(std::size_t i = 0 ; i < size ; ++i)
    {
        const sight::data::point::point_coord_array_t tmp1 = points1[i]->getCoord();
        const sight::data::point::point_coord_array_t tmp2 = points2[i]->getCoord();
        const glm::dvec3 pt1                               = glm::dvec3(tmp1[0], tmp1[1], tmp1[2]);
        const glm::dvec3 pt2                               = glm::dvec3(tmp2[0], tmp2[1], tmp2[2]);
        *distance_array_itr = glm::distance(pt1, pt2);
        ++distance_array_itr;
    }

    return output_array;
}

//------------------------------------------------------------------------------

void point_list::transform(
    sight::data::point_list::sptr& _point_list,
    const sight::data::matrix4::csptr& _matrix
)
{
    sight::data::point_list::PointListContainer points = _point_list->getPoints();
    const std::size_t size                             = points.size();

    for(std::size_t i = 0 ; i < size ; ++i)
    {
        sight::data::point& pt = *points[i];

        // Transform the current point with the input matrix
        sight::geometry::data::multiply(*_matrix, pt, pt);
    }
}

//------------------------------------------------------------------------------

void point_list::associate(
    const sight::data::point_list::csptr& _point_list1,
    sight::data::point_list::sptr _point_list2
)
{
    SIGHT_ASSERT(
        "the 2 pointLists must have the same number of points",
        _point_list1->getPoints().size() == _point_list2->getPoints().size()
    );

    sight::data::point_list::PointListContainer points1 = _point_list1->getPoints();
    sight::data::point_list::PointListContainer points2 = _point_list2->getPoints();

    const std::size_t size = points1.size();

    // Transform first point list into vector< glm::dvec3 > (no erase is performed)
    std::vector<glm::dvec3> vec1;
    vec1.reserve(size);
    //and second one into a list (since we will erase associated points)
    std::list<glm::dvec3> list2;

    for(std::size_t i = 0 ; i < size ; ++i)
    {
        const sight::data::point::point_coord_array_t tmp1 = points1[i]->getCoord();
        const sight::data::point::point_coord_array_t tmp2 = points2[i]->getCoord();

        // Add the point to vector/list
        vec1.emplace_back(tmp1[0], tmp1[1], tmp1[2]);
        list2.emplace_back(tmp2[0], tmp2[1], tmp2[2]);
    }

    std::size_t index = 0;
    for(auto point1 : vec1)
    {
        // Identify the closest point
        double distance_min = std::numeric_limits<double>::max();
        std::list<glm::dvec3>::iterator it_closest_point;

        for(auto it2 = list2.begin() ; it2 != list2.end() ; ++it2)
        {
            const glm::dvec3 point2 = *it2;
            const double distance   = glm::distance(point1, point2);
            if(distance < distance_min)
            {
                distance_min     = distance;
                it_closest_point = it2;
            }
        }

        sight::data::point::point_coord_array_t point_coord;
        point_coord[0] = it_closest_point->x;
        point_coord[1] = it_closest_point->y;
        point_coord[2] = it_closest_point->z;

        sight::data::point::sptr pt = points2[index];
        pt->setCoord(point_coord);
        ++index;

        // Erase the already matched point
        list2.erase(it_closest_point);
    }
}

//------------------------------------------------------------------------------

sight::data::point::sptr point_list::removeClosestPoint(
    const sight::data::point_list::sptr& _point_list,
    const sight::data::point::csptr& _point,
    float _delta
)
{
    // Initial data
    const auto& list = _point_list->getPoints();
    if(!list.empty())
    {
        const auto& coord1 = _point->getCoord();
        const glm::vec3 p1 {coord1[0], coord1[1], coord1[2]};

        // Data to find the closest point
        float closest                  = std::numeric_limits<float>::max();
        sight::data::point::sptr point = nullptr;
        std::size_t index              = 0;
        bool point_is_found            = false;

        // Find the closest one
        for(std::size_t i = 0 ; i < list.size() ; ++i)
        {
            const auto& coord2 = list[i]->getCoord();
            const glm::vec3 p2 {coord2[0], coord2[1], coord2[2]};

            float temp_closest = NAN;
            if((temp_closest = glm::distance(p1, p2)) < _delta && temp_closest < closest)
            {
                closest        = temp_closest;
                point          = list[i];
                index          = i;
                point_is_found = true;
            }
        }

        // Remove the closest point if it has been found
        if(point_is_found)
        {
            _point_list->remove(index);
        }

        return point;
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

} // namespace sight::geometry::data
