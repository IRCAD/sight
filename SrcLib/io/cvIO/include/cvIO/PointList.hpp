/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "cvIO/config.hpp"

#include <fwData/PointList.hpp>

#include <opencv2/core.hpp>

namespace cvIO
{
/**
 * @brief Point and point list conversion methods.
 */
class CVIO_CLASS_API PointList
{
public:

    /// Copy the first two coordinates of the f4s point to the OpenCV point.
    CVIO_API static void copyToCv(const ::fwData::Point::csptr& _src, ::cv::Point2d& _dst);

    /// Copy a f4s 2D point list to OpenCV.
    CVIO_API static void copyToCv(const ::fwData::PointList::csptr& _src, std::vector< ::cv::Point2d >& _dst);

    /// Convert OpenCV points to a f4s point list.
    CVIO_API static void copyFromCv(const std::vector< ::cv::Point2d >& _src, const ::fwData::PointList::sptr& _dst);

};

} //namespace cvIO
