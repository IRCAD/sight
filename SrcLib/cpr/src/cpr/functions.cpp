/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "cpr/functions.hpp"

#include <boost/assign/list_of.hpp>

#include <vtkMath.h>

#include <fwData/Image.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/PointList.hpp>

#include <fwServices/macros.hpp>

#include <navigation/functions.hpp>

#include <math.h>

namespace cpr
{

double getImageMinSpacing(const CSPTR(::fwData::Image)& imageSource)
{
    const ::fwData::Image::SpacingType& spacing = imageSource->getSpacing();
    return *std::min_element(spacing.begin(), spacing.end());
}

//------------------------------------------------------------------------------

double getImageMaxSpacing(const CSPTR(::fwData::Image)& imageSource)
{
    const ::fwData::Image::SpacingType& spacing = imageSource->getSpacing();
    return *std::max_element(spacing.begin(), spacing.end());
}

//------------------------------------------------------------------------------

double getImageMaxSize(const CSPTR(::fwData::Image)& imageSource)
{
    const std::vector< double > mmSizes = ::boost::assign::list_of
                                              (imageSource->getSize()[0] * imageSource->getSpacing()[0])
                                              (imageSource->getSize()[1] * imageSource->getSpacing()[1])
                                              (imageSource->getSize()[2] * imageSource->getSpacing()[2]);

    return *std::max_element(mmSizes.begin(), mmSizes.end());
}

//------------------------------------------------------------------------------

void fillPointGrid(
    double& spacing,
    double& height,
    const CSPTR(::fwData::PointList)& pointList,
    std::vector<double>& pointGrid,
    unsigned int& nbCol,
    unsigned int& nbRow,
    const double angle)
{
    double splineLength                                      = 0.0;
    double previousy[3]                                      = {0, 0, 0};
    vtkSmartPointer < vtkParametricSpline > parametricSpline = vtkSmartPointer < vtkParametricSpline >::New();

    vtkSmartPointer < vtkPoints > points          = vtkSmartPointer < vtkPoints > ::New();
    vtkSmartPointer < vtkPolyData > polyLine      = vtkSmartPointer < vtkPolyData > ::New();
    vtkSmartPointer < vtkPolyData > splineNormals = vtkSmartPointer < vtkPolyData > ::New();

    // Computes spline from point list
    ::navigation::updateSpline(pointList, points, parametricSpline, splineLength);
    ::navigation::initializeVectors(pointList, parametricSpline, &previousy[0], angle);

    // Computes poly line and spline normals from spline
    ::navigation::computePolyData(parametricSpline, polyLine, splineNormals, &previousy[0], spacing);

    // Computes nbCol and nbRow
    nbRow = floor(height / spacing) * 2 + 1;
    nbCol = polyLine->GetNumberOfPoints();

    // Fill the point grid
    double p[3], splineNormal[3];
    const int rangeMax = (nbRow - 1) / 2;
    const int rangeMin = -rangeMax;

    for(int j = rangeMin; j <= rangeMax; ++j)
    {
        const double range = spacing * j;

        for(unsigned int i = 0; i < nbCol; ++i)
        {
            polyLine->GetPoint(i, p);
            splineNormals->GetPoint(i, splineNormal);

            pointGrid.push_back(p[0] + splineNormal[0] * range);
            pointGrid.push_back(p[1] + splineNormal[1] * range);
            pointGrid.push_back(p[2] + splineNormal[2] * range);
        }
    }
}

//------------------------------------------------------------------------------

void computePositionOfPointOnSpline(
    const CSPTR(::fwData::PointList)& pointList,
    const size_t indexPoint,
    double& position)
{
    position = 0;
    double splineLength = 0, error = 0;
    double u[3], du[3], pt[3], findPoint[3], diff[3];

    vtkSmartPointer<vtkMath> math                           = vtkSmartPointer<vtkMath>::New();
    vtkSmartPointer< vtkParametricSpline > parametricSpline = vtkSmartPointer < vtkParametricSpline > ::New();
    vtkSmartPointer< vtkPoints > points                     = vtkSmartPointer < vtkPoints > ::New();

    // Computes spline
    ::navigation::updateSpline(pointList, points, parametricSpline, splineLength);

    // Get the parametric value v of the spline corresponding to the considered point
    ::fwData::Point::sptr point = pointList->getCRefPoints()[indexPoint];
    for (int i = 0; i < 3; ++i)
    {
        findPoint[i] = point->getRefCoord()[i];
    }

    // Initialization
    u[0] = 0;
    parametricSpline->Evaluate(u, pt, du);

    for(int n = 0; n < 3; ++n)
    {
        diff[n] = pt[n] - findPoint[n];
    }

    error = math->Norm(diff);

    // Find the value v which minimizes the distance between the point to find and the current spline point.
    for(double v = 0; v <= 1; v += 1.0 / (splineLength * 10.0))
    {
        u[0] = v;
        parametricSpline->Evaluate(u, pt, du);

        for(int n = 0; n < 3; ++n)
        {
            diff[n] = pt[n] - findPoint[n];
        }

        if(math->Norm(diff) < error)
        {
            error    = math->Norm(diff);
            position = v;
        }
    }

    position *= splineLength;
}

//------------------------------------------------------------------------------

bool computeImageIndexFromSpacePosition(
    const CSPTR(::fwData::Image)& imageSource,
    const double* spacePosition,
    unsigned int* indexPosition)
{
    double imagePosition [3];
    bool isPointInTheImage = true;

    for(int i = 0; i < 3; ++i)
    {
        imagePosition[i] = spacePosition[i] - imageSource->getOrigin()[i];

        // Computes the point index in the image take the nearest point
        indexPosition[i] = floor(imagePosition[i] / (imageSource->getSpacing()[i] * 1.0));
        OSLM_DEBUG("Index position floor => " << indexPosition[i]);

        // Tests if the point is in the image
        if(indexPosition[i] > (imageSource->getSize()[i] - 1) || indexPosition[i] < 0)
        {
            isPointInTheImage = false;
        }
    }

    return isPointInTheImage;
}

} // namespace cpr

