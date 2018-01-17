/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataTools/helper/PointList.hpp"

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include <list>

namespace fwDataTools
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

::fwData::Array::sptr
PointList::computeDistance(::fwData::PointList::sptr pointList1,
                           ::fwData::PointList::sptr pointList2)
{
    SLM_ASSERT("the 2 pointLists must have the same number of points",
               pointList1->getPoints().size() == pointList2->getPoints().size() );

    const ::fwData::PointList::PointListContainer points1 = pointList1->getPoints();
    const ::fwData::PointList::PointListContainer points2 = pointList2->getPoints();
    const size_t size                                     = points1.size();

    ::fwData::Array::sptr outputArray = ::fwData::Array::New();
    ::fwData::Array::SizeType arraySize;
    arraySize.push_back(size);
    outputArray->resize("double", arraySize, 1, true);
    ::fwDataTools::helper::Array arrayHelper(outputArray);
    double* distanceArray = arrayHelper.begin<double>();

    for (size_t i = 0; i < size; ++i)
    {
        const ::fwData::Point::PointCoordArrayType tmp1 = points1[i]->getCoord();
        const ::fwData::Point::PointCoordArrayType tmp2 = points2[i]->getCoord();
        const ::glm::dvec3 pt1                          = ::glm::dvec3(tmp1[0], tmp1[1], tmp1[2]);
        const ::glm::dvec3 pt2                          = ::glm::dvec3(tmp2[0], tmp2[1], tmp2[2]);
        distanceArray[i] = ::glm::distance(pt1, pt2);
    }

    return outputArray;
}

//------------------------------------------------------------------------------

void PointList::transform(::fwData::PointList::sptr& pointList,
                          const ::fwData::TransformationMatrix3D::csptr&  matrix)
{
    ::fwData::PointList::PointListContainer points = pointList->getPoints();
    const size_t size = points.size();

    for(size_t i = 0; i < size; ++i)
    {
        ::fwData::Point::sptr& pt = points[i];

        // Transform the current point with the input matrix
        ::fwDataTools::TransformationMatrix3D::multiply(matrix, pt, pt);
    }
}

//------------------------------------------------------------------------------

void PointList::associate(const ::fwData::PointList::csptr& pointList1,
                          ::fwData::PointList::sptr pointList2)
{
    SLM_ASSERT("the 2 pointLists must have the same number of points",
               pointList1->getPoints().size() == pointList2->getPoints().size() );

    ::fwData::PointList::PointListContainer points1 = pointList1->getPoints();
    ::fwData::PointList::PointListContainer points2 = pointList2->getPoints();

    const size_t size = points1.size();

    // Transform first point list into vector< ::glm::dvec3 > (no erase is performed)
    std::vector< ::glm::dvec3 > vec1;
    vec1.reserve(size);
    //and second one into a list (since we will erase associated points)
    std::list< ::glm::dvec3 > list2;

    for(size_t i = 0; i < size; ++i)
    {
        const ::fwData::Point::PointCoordArrayType tmp1 = points1[i]->getCoord();
        const ::fwData::Point::PointCoordArrayType tmp2 = points2[i]->getCoord();

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

        ::fwData::Point::PointCoordArrayType pointCoord;
        pointCoord[0] = itClosestPoint->x;
        pointCoord[1] = itClosestPoint->y;
        pointCoord[2] = itClosestPoint->z;

        ::fwData::Point::sptr pt = points2[index];
        pt->setCoord(pointCoord);
        ++index;

        // Erase the already matched point
        list2.erase(itClosestPoint);
    }
}

//-----------------------------------------------------------------------------

} // namespace helper
} // namespace fwDataTools
