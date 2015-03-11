/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOCALIBRATION_HELPER_CHESSBOARDANALYZER_HPP__
#define __VIDEOCALIBRATION_HELPER_CHESSBOARDANALYZER_HPP__

#include "videoCalibration/config.hpp"

#include <fwCore/base.hpp>

namespace fwData
{
class PointList;
class Image;
}

namespace videoCalibration
{
namespace helper
{

/// Helper to analyze chessboard
class VIDEOCALIBRATION_CLASS_API ChessboardAnalyzer
{
public:

    /**
     * @brief Detect chessboard points
     * @param image the frame displaying the chessboard
     * @param xDim the number of chessboard squares horizontally
     * @param yDim the number of chessboard squares vertically
     * @return The list of chessboard points or NULL if no points are detected
     */
    VIDEOCALIBRATION_API static SPTR(::fwData::PointList) detectChessboard(SPTR(::fwData::Image) image,
                                                                           size_t xDim, size_t yDim);
};

} // namespace helper

} //namespace videoCalibration


#endif // __VIDEOCALIBRATION_HELPER_CHESSBOARDANALYZER_HPP__
