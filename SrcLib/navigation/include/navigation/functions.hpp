/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __NAVIGATION_FUNCTIONS_HPP__
#define __NAVIGATION_FUNCTIONS_HPP__

#include <fwData/PointList.hpp>

#include <vtkParametricSpline.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

#include <navigation/config.hpp>

/**
 * @namespace navigation
 * @brief Contains tools to compute 3D splines for virtual navigation purpose.
 */
namespace navigation
{

/**
 * @brief Computes a 3D spline, when a point is added in the source points.
 * The new point is added to given vtkPoints and set the points to the spline.
 *
 * @param[in] pointList source points
 * @param[in] pointIndex index of source point
 * @param[in] points spline points
 * @param[out] computedSpline output spline
 * @param[out] length length of computed spline
 */
NAVIGATION_API void computeSpline(
    const ::fwData::PointList::csptr& pointList,
    const int pointIndex,
    const vtkSmartPointer<vtkPoints>& points,
    const vtkSmartPointer<vtkParametricSpline>& computedSpline,
    double& length);

/**
 * @brief Compute spline when a point is removed: recompute the whole spline
 *
 * @param[in] pointList source points
 * @param[out] points output points
 * @param[out] computedSpline output spline
 * @param[out] length length of computed spline
 */
NAVIGATION_API void updateSpline(
    const ::fwData::PointList::csptr& pointList,
    const vtkSmartPointer<vtkPoints>& points,
    const vtkSmartPointer<vtkParametricSpline>& computedSpline,
    double& length);

/**
 * @brief Computes view up position relative to given point.
 *
 * @param[in] x point to compute view up
 * @param[out] viewUp view up
 */
NAVIGATION_API void computeViewUp(const double x[3], double* viewUp);

/**
 * @brief Computes the length of the spline.
 *
 * @param[in] computedSpline input spline
 * @param[out] length length of computed spline
 */
NAVIGATION_API void computeSplineLength(const vtkSmartPointer<vtkParametricSpline>& computedSpline, double& length);

/**
 * @brief Initializes the orientation of camera axes.
 *
 * @param[in] pointList source points
 * @param[in] computedSpline
 * @param[out] yfirst
 * @param[in] angle angle to initialize the vectors
 */
NAVIGATION_API void initializeVectors(
    const ::fwData::PointList::csptr& pointList,
    const vtkSmartPointer<vtkParametricSpline>& computedSpline,
    double* yfirst,
    double angle = 0);

/**
 * @brief Checks if given points are coplanar.
 *
 * @param[in] pointList source points
 * @param[out] normal normal of the points if they are coplanar
 *
 * @return if given points are coplanar
 */
NAVIGATION_API bool arePointsCoplanar(const ::fwData::PointList::csptr& pointList, double* normal);

/**
 * @brief Compute a polyline and the normal from the spline.
 *
 * @param[in] computedSpline source spline
 * @param[out] polyLine output polyline
 * @param[out] splineNormals output normals
 * @param[in] yprevious
 * @param[in] step
 * @param[in] axis axis (x, y, z)
 */
NAVIGATION_API void computePolyData(
    const vtkSmartPointer<vtkParametricSpline>& computedSpline,
    vtkSmartPointer<vtkPolyData>& polyLine,
    vtkSmartPointer<vtkPolyData>& splineNormals,
    double* yprevious,
    const double& step,
    std::string axis = "z");

}   // namespace navigation

#endif // __NAVIGATION_FUNCTIONS_HPP__

