/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <fwData/PointList.hpp>
#include <fwData/Image.hpp>

#include <fwComEd/helper/Image.hpp>

#include "videoCalibration/helper/ChessboardAnalyzer.hpp"

namespace videoCalibration
{
namespace helper
{

SPTR(::fwData::PointList) ChessboardAnalyzer::detectChessboard(SPTR(::fwData::Image) image, size_t xDim, size_t yDim)
{
    ::fwData::PointList::sptr pointlist;

    ::fwComEd::helper::Image imgHelper(image);

    int height = static_cast<int>(image->getSize()[0]);
    int width  = static_cast<int>(image->getSize()[1]);

    cv::Mat img(width, height, CV_8UC4, imgHelper.getBuffer());
    cv::Mat grayImg;
    cv::cvtColor(img, grayImg, CV_RGBA2GRAY);
    cv::Size boardSize(static_cast<int>(xDim) - 1, static_cast<int>(yDim) - 1);
    std::vector< cv::Point2f > corners;

    int flags = CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE | CV_CALIB_CB_FILTER_QUADS
                | CV_CALIB_CB_FAST_CHECK;

    if (cv::findChessboardCorners(grayImg, boardSize, corners, flags))
    {
        cv::TermCriteria term(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 30, 0.1);
        cv::cornerSubPix(grayImg, corners, cv::Size(5, 5), cv::Size(-1, -1), term);

        pointlist                                       = ::fwData::PointList::New();
        ::fwData::PointList::PointListContainer &points = pointlist->getRefPoints();
        points.reserve(corners.size());

        for(cv::Point2f& p : corners)
        {
            ::fwData::Point::sptr point = ::fwData::Point::New(p.x, p.y);
            points.push_back(point);
        }
    }

    return pointlist;
}

} // namespace helper

} //namespace videoCalibration
