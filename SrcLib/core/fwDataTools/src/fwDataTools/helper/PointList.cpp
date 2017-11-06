/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataTools/helper/PointList.hpp"

#include <fwDataTools/helper/Array.hpp>

#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

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
        ::fwData::Point::PointCoordArrayType tmp1 = points1[i]->getCoord();
        ::fwData::Point::PointCoordArrayType tmp2 = points2[i]->getCoord();
        ::glm::dvec3 pt1                          = ::glm::dvec3(tmp1[0], tmp1[1], tmp1[2]);
        ::glm::dvec3 pt2                          = ::glm::dvec3(tmp2[0], tmp2[1], tmp2[2]);
        distanceArray[i]                          = ::glm::distance(pt1, pt2);
    }

    return outputArray;
}

//-----------------------------------------------------------------------------

} // namespace helper
} // namespace fwDataTools
