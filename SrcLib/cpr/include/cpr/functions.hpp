/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CPR_FUNCTIONS_HPP__
#define __CPR_FUNCTIONS_HPP__

#include "cpr/config.hpp"

#include <fwCore/macros.hpp>

#include <vector>

namespace fwData
{
class Image;
class Mesh;
class PointList;
}

namespace cpr
{
/**
 * @brief Get the minimum image spacing.
 *
 * @param imageSource ::fwData::Image source image
 *
 * @return lowest spacing value
 */
CPR_API double getImageMinSpacing(const CSPTR(::fwData::Image)& imageSource);

/**
 * @brief Get the maximum image spacing
 *
 * @param imageSource ::fwData::Image source image
 *
 * @return highest spacing value
 */
CPR_API double getImageMaxSpacing(const CSPTR(::fwData::Image)& imageSource);

/**
 * @brief Get the maximum image size (size in millimeters : spacing value by pixel size).
 *
 * @param imageSource ::fwData::Image source image
 *
 * @return highest size in millimeters
 */
CPR_API double getImageMaxSize(const CSPTR(::fwData::Image)& imageSource);

/**
 * @brief Fills the point grid for the CPR.
 *
 * @param[in] spacing CPR spacing
 * @param[in] height CPR height
 * @param[in] pointList spline points
 * @param[out] pointGrid output point grid
 * @param[out] nbCol number of columns in the point grid
 * @param[out] nbRow number of rows in the point grid
 * @param[in] angle rotation angle
 */
CPR_API void fillPointGrid (
    double& spacing,
    double& height,
    const CSPTR(::fwData::PointList)& pointList,
    std::vector<double>& pointGrid,
    unsigned int& nbCol,
    unsigned int& nbRow,
    const double angle = 0);

/**
 * @brief Computes the parametric position u [0,1] of the considered point.
 *
 * @param[in] pointList source points
 * @param[in] indexPoint index of source point
 * @param[out] position computed position
 */
CPR_API void computePositionOfPointOnSpline(
    const CSPTR(::fwData::PointList)& pointList,
    const size_t indexPoint,
    double& position);

/**
 * @brief Computes the image index of the given position.
 * Returns true if the point is in the image.
 *
 * @param[in] imageSource source image
 * @param[in] spacePosition position to compute image index
 * @param[out] indexPosition image index of the given space position
 *
 * @return if given space position is in the image
 */
CPR_API bool computeImageIndexFromSpacePosition(
    const CSPTR(::fwData::Image)& imageSource,
    const double* spacePosition,
    unsigned int* indexPosition);

} // namespace cpr

#endif // __CPR_FUNCTIONS_HPP__

