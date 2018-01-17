/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#include "navigation/functions.hpp"

#include <fwData/Point.hpp>

#include <vtkCardinalSpline.h>
#include <vtkMath.h>
#include <vtkPlane.h>
#include <vtkPoints.h>
#include <vtkSpline.h>
#include <vtkTransform.h>

#include <cmath>

namespace navigation
{

//------------------------------------------------------------------------------

void computeSpline(
    const ::fwData::PointList::csptr& pointList,
    const int pointIndex,
    const vtkSmartPointer<vtkPoints>& points,
    const vtkSmartPointer<vtkParametricSpline>& computedSpline,
    double& length)
{
    SLM_ASSERT("No valid point list", pointList);
    OSLM_ASSERT("Requested point of index '" << pointIndex << "' whereas point list size is '"
                                             << pointList->getPoints().size() << "'",
                pointIndex < pointList->getPoints().size());

    // Get the last point of PointList
    ::fwData::Point::sptr point = (pointList->getPoints())[pointIndex];

    // Insert the point in points.
    points->InsertNextPoint(&point->getCoord()[0]);
    computedSpline->SetNumberOfPoints(pointIndex + 1);
    computedSpline->SetPoints(points);

    // Update spline length
    if (pointIndex > 0)
    {
        ::navigation::computeSplineLength(computedSpline, length);
    }
    else if (pointIndex == 0)
    {
        length = 0.0;
    }
}

//------------------------------------------------------------------------------

void updateSpline(
    const ::fwData::PointList::csptr& pointList,
    const vtkSmartPointer<vtkPoints>& points,
    const vtkSmartPointer<vtkParametricSpline>& computedSpline,
    double& length)
{
    vtkSmartPointer<vtkPoints> tempPoints = vtkSmartPointer<vtkPoints>::New();
    int nbofPoints                        = 0;
    SLM_ASSERT("No valid point list", pointList);

    for(::fwData::Point::sptr point :  pointList->getPoints())
    {
        tempPoints->InsertNextPoint(&point->getCoord()[0]);
        nbofPoints++;
    }

    if(nbofPoints > 0)
    {
        computedSpline->SetNumberOfPoints(nbofPoints);
        computedSpline->SetPoints(tempPoints);
        ::navigation::computeSplineLength(computedSpline, length);
    }
    else if(nbofPoints == 0)
    {
        computedSpline->GetXSpline()->RemoveAllPoints();
        computedSpline->GetYSpline()->RemoveAllPoints();
        computedSpline->GetZSpline()->RemoveAllPoints();
        length = 0.0;
    }

    points->DeepCopy(tempPoints);
}

//------------------------------------------------------------------------------

void computeSplineLength(const vtkSmartPointer<vtkParametricSpline>& computedSpline, double& length)
{
    length = 0;
    vtkSmartPointer<vtkMath> math = vtkSmartPointer<vtkMath>::New();
    double u[3], du[3], ptFirst[3], ptNext[3], splineVector[3];

    for (double v = 0; v <= 1; v += 0.001)
    {
        u[0] = v;
        computedSpline->Evaluate(u, ptFirst, du);

        u[0] = v + 0.001;
        computedSpline->Evaluate(u, ptNext, du);

        for(int n = 0; n < 3; ++n)
        {
            splineVector[n] = ptNext[n] - ptFirst[n];
        }

        length += math->Norm(splineVector);
    }
}

//------------------------------------------------------------------------------

void computeViewUp(const double x[3], double* viewUp)
{
    vtkSmartPointer<vtkMath> math = vtkSmartPointer<vtkMath>::New();

    double res[3];
    double i[3] = {1, 0, 0};

    math->Cross(x, i, res);
    std::copy(i, i + 3, viewUp);

    double min = 1 - math->Norm(res);

    for(int n = 1; n < 3; ++n)
    {
        double candViewUp[3] = {0, 0, 0};
        candViewUp[n] = 1;
        math->Cross(x, candViewUp, res);

        if (1 - math->Norm(res) < min)
        {
            std::copy(candViewUp, candViewUp + 3, viewUp);
            min = 1 - math->Norm(res);
        }
    }
}

//------------------------------------------------------------------------------

bool arePointsCoplanar(const ::fwData::PointList::csptr& pointList, double* normal)
{
    const size_t numberOfPoints = pointList->getPoints().size();
    bool arePointsCoplanar      = true;
    bool isNormalComputed       = false;

    if (numberOfPoints >= 3)
    {
        ::fwData::Point::sptr point1, point2, point3, pointnext;
        double col0[3] = {0, 0, 0}, col1[3] = {0, 0, 0}, col2[3] = {0, 0, 0};
        double vector1[3] = {0, 0, 0}, vector2[3] = {0, 0, 0}, res[3] = {0, 0, 0};
        vtkSmartPointer<vtkMath> math = vtkSmartPointer<vtkMath>::New();

        // Define the plane build by the 3 first points of the spline
        point1 = (pointList->getPoints())[0];
        point2 = (pointList->getPoints())[1];
        point3 = (pointList->getPoints())[2];

        // Define the two first vectors
        for (int i = 0; i < 3; ++i)
        {
            vector1[i] = point2->getCoord()[i] - point1->getCoord()[i];
            vector2[i] = point3->getCoord()[i] - point1->getCoord()[i];
        }

        math->Cross(vector1, vector2, res);

        // Check if the points are not aligned
        if (math->Norm(res) > 0)
        {
            math->Normalize(res);
            std::copy(res, res + 3, normal);
            isNormalComputed = true;

            if(numberOfPoints == 3)
            {
                return arePointsCoplanar;
            }
        }

        if (numberOfPoints > 3)
        {
            // Check if the next points of the pointList are on the plane define by point1, point2, point3
            for (int n = 3; n < numberOfPoints; ++n)
            {
                pointnext = (pointList->getPoints())[n];

                for (int i = 0; i < 3; ++i)
                {
                    col0[i]    = pointnext->getCoord()[0] - point1->getCoord()[i];
                    col1[i]    = pointnext->getCoord()[1] - point2->getCoord()[i];
                    col2[i]    = pointnext->getCoord()[2] - point3->getCoord()[i];
                    vector2[i] = pointnext->getCoord()[i] - point1->getCoord()[i];
                }
            }

            // if the points are not coplanar return false
            if (math->Determinant3x3(col0, col1, col2) != 0)
            {
                arePointsCoplanar = false;
                return arePointsCoplanar;
            }
            else if (math->Determinant3x3(col0, col1, col2) == 0 && !isNormalComputed)
            {
                // if the points are coplanar: computes the normal if it is not already computed
                math->Cross(vector1, vector2, res);

                if (math->Norm(res) > 0)
                {
                    math->Normalize(res);
                    std::copy(res, res + 3, normal);
                    isNormalComputed = true;
                }
            }
        }
    }

    // if the normal is not computed the points are aligned
    return (arePointsCoplanar && isNormalComputed);
}

//------------------------------------------------------------------------------

void initializeVectors(
    const ::fwData::PointList::csptr& pointList,
    const vtkSmartPointer<vtkParametricSpline>& computedSpline,
    double* yfirst,
    double angle)
{
    double u[3], du[3], ptFirst[3], ptNext[3], viewUp[3], splineNormal[3], xfirst[3], zfirst[3];
    size_t numberOfPoints                   = pointList->getPoints().size();
    vtkSmartPointer<vtkMath> math           = vtkSmartPointer<vtkMath>::New();
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

    // Compute xfirst:
    u[0] = 0;
    computedSpline->Evaluate(u, ptFirst, du);

    u[0] = 0.01;
    computedSpline->Evaluate(u, ptNext, du);

    for (int n = 0; n < 3; ++n)
    {
        xfirst[n] = ptNext[n] - ptFirst[n];
    }

    math->Normalize(xfirst);

    // Case 1: numberOfPoints==2 of numberOfPoints>=3 and points are not coplanar
    // -> initialize y and z vectors by default
    if (numberOfPoints == 2 || (numberOfPoints >= 3 && !arePointsCoplanar(pointList, splineNormal)) )
    {
        // Get the first y and z
        ::navigation::computeViewUp(xfirst, &viewUp[0]);

        // Compute the y vector the first time
        math->Cross(viewUp, xfirst, yfirst);
        math->Normalize(yfirst);
        math->Cross(xfirst, yfirst, zfirst);
        math->Normalize(zfirst);
    }

    // Case 2: the points are coplanar: z = normal of the spline
    if(arePointsCoplanar(pointList, splineNormal))
    {
        std::copy(splineNormal, splineNormal + 3, zfirst);
        math->Cross(zfirst, xfirst, yfirst);
        math->Normalize(yfirst);
    }

    if (angle > 0 && angle < 360)
    {
        transform->RotateWXYZ(angle, xfirst[0], xfirst[1], xfirst[2]);
        transform->MultiplyPoint(yfirst, yfirst);
    }
}

//------------------------------------------------------------------------------

void computePolyData(
    const vtkSmartPointer<vtkParametricSpline>& computedSpline,
    vtkSmartPointer<vtkPolyData>& polyLine,
    vtkSmartPointer<vtkPolyData>& splineNormals,
    double* yprevious,
    const double& step,
    std::string axis)
{
    double length = 0.0;
    double u[3], du[3], pt[3], x[3], y[3], z[3], direction[3], ptNext[3], yPreviouscurrent[3];

    vtkSmartPointer<vtkMath> math = vtkSmartPointer<vtkMath>::New();
    std::copy(yprevious, yprevious + 3, yPreviouscurrent);
    vtkSmartPointer<vtkPoints> tempPoints     = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkPoints> tempDirections = vtkSmartPointer<vtkPoints>::New();
    ::navigation::computeSplineLength(computedSpline, length);

    const float vStep = step / length;

    if (length > 0.0)
    {
        for (double v = 0.; v < 1; v += vStep)
        {
            u[0] = v;
            computedSpline->Evaluate(u, pt, du);

            u[0] = v + (1.0 / length);
            computedSpline->Evaluate(u, ptNext, du);

            for (int n = 0; n < 3; ++n)
            {
                x[n] = ptNext[n] - pt[n];
            }

            math->Normalize(x);

            // Compute the next vectors
            math->Cross(x, yPreviouscurrent, z);
            math->Normalize(z);
            math->Cross(z, x, y);
            math->Normalize(y);

            std::copy(y, y + 3, yPreviouscurrent);
            tempPoints->InsertNextPoint(pt);

            if(axis.compare("y") == 0)
            {
                std::copy(y, y + 3, direction);
            }
            else
            {
                std::copy(z, z + 3, direction);
            }

            tempDirections->InsertNextPoint(direction);
        }

        // Insert the last point of the spline
        u[0] = 1.;
        computedSpline->Evaluate(u, pt, du);
        tempPoints->InsertNextPoint(pt);

        // Insert the last spline normal: same as the last
        tempDirections->InsertNextPoint(direction);

        // Fill the polyLine and the splineNormals polydata
        polyLine->SetPoints(tempPoints);
        splineNormals->SetPoints(tempDirections);
    }
}

} // namespace navigation
