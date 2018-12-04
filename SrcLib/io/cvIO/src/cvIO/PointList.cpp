/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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
