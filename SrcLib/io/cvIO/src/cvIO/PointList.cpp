/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "cvIO/PointList.hpp"

namespace cvIO
{

//------------------------------------------------------------------------------

void PointList::copyToCv(const ::fwData::Point::csptr& _src, ::cv::Point2d& _dst)
{
    _dst.x = _src->getCoord()[0];
    _dst.y = _src->getCoord()[1];
}

//------------------------------------------------------------------------------

void PointList::copyToCv(const ::fwData::PointList::csptr& _src, std::vector< ::cv::Point2d >& _dst)
{
    _dst.clear();
    for(const auto& point : _src->getPoints())
    {
        ::cv::Point2d cvPoint;
        copyToCv(point, cvPoint);
        _dst.push_back(cvPoint);
    }
}

//------------------------------------------------------------------------------

void PointList::copyFromCv(const std::vector< ::cv::Point2d >& _src, const ::fwData::PointList::sptr& _dst)
{
    _dst->getPoints().clear();
    for(const auto& cvPoint : _src)
    {
        _dst->getPoints().push_back(::fwData::Point::New(cvPoint.x, cvPoint.y, 0.));
    }
}

//------------------------------------------------------------------------------

} // namespace cvIO
