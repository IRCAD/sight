/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include "data/tools/helper/PointList.hpp"

#include <data/Point.hpp>
#include <data/PointList.hpp>
#include <data/tools/TransformationMatrix3D.hpp>

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include <list>

namespace sight::data::tools
{
namespace helper
{

//-----------------------------------------------------------------------------

PointList::PointList()
{
}

//-----------------------------------------------------------------------------

PointList::~PointList()
{
}

//-----------------------------------------------------------------------------

data::Array::sptr
PointList::computeDistance(data::PointList::sptr _pointList1,
                           data::PointList::sptr _pointList2)
{
    SLM_ASSERT("the 2 pointLists must have the same number of points",
               _pointList1->getPoints().size() == _pointList2->getPoints().size() );

    const data::PointList::PointListContainer points1 = _pointList1->getPoints();
    const data::PointList::PointListContainer points2 = _pointList2->getPoints();
    const size_t size                                 = points1.size();

    data::Array::sptr outputArray = data::Array::New();
    outputArray->resize({size}, core::tools::Type::s_DOUBLE);
    const auto dumpLock   = outputArray->lock();
    auto distanceArrayItr = outputArray->begin<double>();

    for (size_t i = 0; i < size; ++i)
    {
        const data::Point::PointCoordArrayType tmp1 = points1[i]->getCoord();
        const data::Point::PointCoordArrayType tmp2 = points2[i]->getCoord();
        const ::glm::dvec3 pt1                      = ::glm::dvec3(tmp1[0], tmp1[1], tmp1[2]);
        const ::glm::dvec3 pt2                      = ::glm::dvec3(tmp2[0], tmp2[1], tmp2[2]);
        *distanceArrayItr = ::glm::distance(pt1, pt2);
        ++distanceArrayItr;
    }

    return outputArray;
}

//------------------------------------------------------------------------------

void PointList::transform(data::PointList::sptr& _pointList,
                          const data::TransformationMatrix3D::csptr& _matrix)
{
    data::PointList::PointListContainer points = _pointList->getPoints();
    const size_t size                          = points.size();

    for(size_t i = 0; i < size; ++i)
    {
        data::Point::sptr& pt = points[i];

        // Transform the current point with the input matrix
        data::tools::TransformationMatrix3D::multiply(_matrix, pt, pt);
    }
}

//------------------------------------------------------------------------------

void PointList::associate(const data::PointList::csptr& _pointList1,
                          data::PointList::sptr _pointList2)
{
    SLM_ASSERT("the 2 pointLists must have the same number of points",
               _pointList1->getPoints().size() == _pointList2->getPoints().size() );

    data::PointList::PointListContainer points1 = _pointList1->getPoints();
    data::PointList::PointListContainer points2 = _pointList2->getPoints();

    const size_t size = points1.size();

    // Transform first point list into vector< ::glm::dvec3 > (no erase is performed)
    std::vector< ::glm::dvec3 > vec1;
    vec1.reserve(size);
    //and second one into a list (since we will erase associated points)
    std::list< ::glm::dvec3 > list2;

    for(size_t i = 0; i < size; ++i)
    {
        const data::Point::PointCoordArrayType tmp1 = points1[i]->getCoord();
        const data::Point::PointCoordArrayType tmp2 = points2[i]->getCoord();

        // Add the point to vector/list
        vec1.push_back(::glm::dvec3( tmp1[0], tmp1[1], tmp1[2]));
        list2.push_back(::glm::dvec3( tmp2[0], tmp2[1], tmp2[2]));
    }

    size_t index = 0;
    for(auto point1 : vec1)
    {
        // Identify the closest point
        double distanceMin = std::numeric_limits<double>::max();
        std::list< ::glm::dvec3 >::iterator itClosestPoint;

        std::list< ::glm::dvec3 >::iterator it2 = list2.begin();
        for(; it2 != list2.end(); it2++)
        {
            const ::glm::dvec3 point2 = *it2;
            const double distance     = ::glm::distance(point1, point2);
            if(distance < distanceMin)
            {
                distanceMin    = distance;
                itClosestPoint = it2;
            }
        }

        data::Point::PointCoordArrayType pointCoord;
        pointCoord[0] = itClosestPoint->x;
        pointCoord[1] = itClosestPoint->y;
        pointCoord[2] = itClosestPoint->z;

        data::Point::sptr pt = points2[index];
        pt->setCoord(pointCoord);
        ++index;

        // Erase the already matched point
        list2.erase(itClosestPoint);
    }
}

//------------------------------------------------------------------------------

const data::Point::sptr PointList::removeClosestPoint(const data::PointList::sptr& _pointList,
                                                      const data::Point::csptr& _point, float _delta)
{
    // Initial data
    const auto& list = _pointList->getPoints();
    if(list.size() > 0)
    {
        const auto& coord1 = _point->getCoord();
        const ::glm::vec3 p1{coord1[0], coord1[1], coord1[2]};

        // Data to find the closest point
        float closest           = std::numeric_limits<float>::max();
        data::Point::sptr point = nullptr;
        size_t index            = 0;
        bool pointIsFound       = false;

        // Find the closest one
        for(size_t i = 0; i < list.size(); ++i)
        {
            const auto& coord2 = list[i]->getCoord();
            const ::glm::vec3 p2{coord2[0], coord2[1], coord2[2]};

            float tempClosest;
            if((tempClosest = ::glm::distance(p1, p2)) < _delta  && tempClosest < closest)
            {
                closest      = tempClosest;
                point        = list[i];
                index        = i;
                pointIsFound = true;
            }
        }

        // Remove the closest point if it has been found
        if(pointIsFound)
        {
            _pointList->remove(index);
        }
        return point;
    }
    return nullptr;
}

//-----------------------------------------------------------------------------

} // namespace helper
} // namespace sight::data::tools
