/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlSplineNavigation/SMoveAlongSpline.hpp"

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

//Slot
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwThread/Worker.hpp>

#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>

#include <vtkMath.h>

// Library
#include <navigation/functions.hpp>

namespace ctrlSplineNavigation
{
static const std::string S_POINTS_KEY = "points";

const ::fwCom::Signals::SignalKeyType SMoveAlongSpline::s_POINT_CHANGED_SIG  = "PointChanged";
const ::fwCom::Signals::SignalKeyType SMoveAlongSpline::s_LENGTH_CHANGED_SIG = "SplineLengthChanged";

const ::fwCom::Slots::SlotKeyType SMoveAlongSpline::s_CHANGE_SLIDER_VALUE_SLOT   = "changeSliderValue";
const ::fwCom::Slots::SlotKeyType SMoveAlongSpline::s_CHANGE_SELECTED_POINT_SLOT = "changeSelectedPoint";
const ::fwCom::Slots::SlotKeyType SMoveAlongSpline::s_CHANGE_ANGLE_SLOT          = "changeAngle";
const ::fwCom::Slots::SlotKeyType SMoveAlongSpline::s_ADD_POINT_SLOT             = "addPoint";
const ::fwCom::Slots::SlotKeyType SMoveAlongSpline::s_REMOVE_POINT_SLOT          = "removePoint";
const ::fwCom::Slots::SlotKeyType SMoveAlongSpline::s_UPDATE_SPLINE_SLOT         = "updateSpline";

//-------------------------------------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::IController, ::ctrlSplineNavigation::SMoveAlongSpline, ::fwData::PointList );

//-------------------------------------------------------------------------------------------------------------

SMoveAlongSpline::SMoveAlongSpline() throw() :
    m_currentSliderPosition(0.0),
    m_previousSliderPosition(0.0),
    m_nbSplinePoints(0),
    m_splineLength(0.0),
    m_angle(0)
{
    // Create a parametric Spline object
    m_parametricSpline = vtkSmartPointer<vtkParametricSpline>::New();

    // Create a vtkPoints object and store the points in it
    m_vtkPoints = vtkSmartPointer<vtkPoints>::New();

    // Signals
    m_sigPointChanged        = newSignal<PointChangedSignalType>(s_POINT_CHANGED_SIG);
    m_sigSplineLengthChanged = newSignal<SplineLengthChangedSignalType>(s_LENGTH_CHANGED_SIG);

    // Slots
    newSlot(s_CHANGE_SLIDER_VALUE_SLOT, &SMoveAlongSpline::moveToPoint, this);
    newSlot(s_CHANGE_SELECTED_POINT_SLOT, &SMoveAlongSpline::moveToSelectedPoint, this);
    newSlot(s_CHANGE_ANGLE_SLOT, &SMoveAlongSpline::setCameraRotation, this);

    newSlot(s_ADD_POINT_SLOT, &SMoveAlongSpline::addPoint, this);
    newSlot(s_REMOVE_POINT_SLOT, &SMoveAlongSpline::removePoint, this);
    newSlot(s_UPDATE_SPLINE_SLOT, &SMoveAlongSpline::updateSpline, this);

}

//-------------------------------------------------------------------------------------------------------------

SMoveAlongSpline::~SMoveAlongSpline() throw()
{
}

//-------------------------------------------------------------------------------------------------------------

void SMoveAlongSpline::configuring()  throw ( ::fwTools::Failed )
{
}

//-------------------------------------------------------------------------------------------------------------

void SMoveAlongSpline::starting()  throw ( ::fwTools::Failed )
{
    ::fwData::PointList::csptr pointList = this->getInput< ::fwData::PointList>(S_POINTS_KEY);
    m_nbSplinePoints                     = pointList->getCRefPoints().size();
    if (m_nbSplinePoints == 0)
    {
        return;
    }

    ::navigation::computeSpline(pointList, m_nbSplinePoints - 1, m_vtkPoints, m_parametricSpline, m_splineLength);
    m_sigSplineLengthChanged->asyncEmit (m_splineLength);

    if(m_nbSplinePoints>=2)
    {
        ::navigation::initializeVectors(pointList, m_parametricSpline, &m_previousyVector[0], m_angle);
    }
}

//-------------------------------------------------------------------------------------------------------------

void SMoveAlongSpline::stopping()  throw ( ::fwTools::Failed )
{
}

//-------------------------------------------------------------------------------------------------------------

void SMoveAlongSpline::updating() throw ( ::fwTools::Failed )
{
}

//----------------------------------------------------------------------------------------------------------------

void SMoveAlongSpline::addPoint(::fwData::Point::sptr /*point*/)
{
    ::fwData::PointList::csptr pointList = this->getInput< ::fwData::PointList>(S_POINTS_KEY);

    ::navigation::computeSpline(pointList, m_nbSplinePoints, m_vtkPoints, m_parametricSpline, m_splineLength);
    m_sigSplineLengthChanged->asyncEmit(m_splineLength);
    m_nbSplinePoints++;

    if(m_nbSplinePoints>=2)
    {
        ::navigation::initializeVectors(pointList, m_parametricSpline, &m_previousyVector[0], m_angle);
    }

    // Synchronize the position on the spline according to the slider position.
    if(m_nbSplinePoints>1 && m_splineLength>0.0)
    {
        this->moveToPoint(m_currentSliderPosition);
    }

    else if(m_nbSplinePoints==1 || m_nbSplinePoints==0 )
    {
        this->moveToPoint(0);
    }
}

//----------------------------------------------------------------------------------------------------------------

void SMoveAlongSpline::removePoint(::fwData::Point::sptr /*point*/)
{
    ::fwData::PointList::csptr pointList = this->getInput< ::fwData::PointList>(S_POINTS_KEY);

    m_nbSplinePoints--;
    ::navigation::updateSpline(pointList, m_vtkPoints, m_parametricSpline, m_splineLength);
    m_sigSplineLengthChanged->asyncEmit(m_splineLength);

    if(m_nbSplinePoints >= 2)
    {
        ::navigation::initializeVectors(pointList, m_parametricSpline, &m_previousyVector[0], m_angle);
    }

    // Synchronize the position on the spline according to the slider position.
    if(m_nbSplinePoints>1 && m_splineLength>0.0)
    {
        this->moveToPoint(m_currentSliderPosition);
    }

    else if(m_nbSplinePoints==1 || m_nbSplinePoints==0 )
    {
        this->moveToPoint(0);
    }
}

//----------------------------------------------------------------------------------------------------------------

void SMoveAlongSpline::updateSpline()
{
    ::fwData::PointList::csptr pointList = this->getInput< ::fwData::PointList>(S_POINTS_KEY);

    ::navigation::updateSpline(pointList, m_vtkPoints, m_parametricSpline, m_splineLength);
    m_sigSplineLengthChanged->asyncEmit(m_splineLength);

    if (m_nbSplinePoints >= 2)
    {
        ::navigation::initializeVectors(pointList, m_parametricSpline, &m_previousyVector[0], m_angle);
    }

    // Synchronize the position on the spline according to the slider position.
    if(m_nbSplinePoints>1 && m_splineLength>0.0)
    {
        this->moveToPoint(m_currentSliderPosition);
    }

    else if(m_nbSplinePoints==1 || m_nbSplinePoints==0 )
    {
        this->moveToPoint(0);
    }
}

//-------------------------------------------------------------------------------------------------------------

void SMoveAlongSpline::moveToPoint (double sliderPosition)
{
    if(m_nbSplinePoints>1)
    {
        m_currentSliderPosition = sliderPosition;
        const double incr = 1.0 / m_splineLength;
        // Go foward
        if( m_previousSliderPosition <= m_currentSliderPosition)
        {
            for (double position = m_previousSliderPosition;
                 position < m_currentSliderPosition;
                 position = position + incr)
            {
                this->computeTransformationMatrix(position);
            }
        }
        // Go back
        else
        {
            for (double position = m_previousSliderPosition;
                 position > m_currentSliderPosition;
                 position = position - incr)
            {
                this->computeTransformationMatrix(position);
            }
        }

    }

    // if there is no point set the matrix to identity
    else if(m_nbSplinePoints==0)
    {
        // Send the destination matrix to SJumpToPoint
        ::fwData::TransformationMatrix3D::sptr destMatrix = ::fwData::TransformationMatrix3D::New();
        for (int lt = 0; lt < 4; lt++)
        {
            for (int ct = 0; ct < 3; ct++)
            {
                if (lt == ct)
                {
                    destMatrix->setCoefficient(lt, ct, 1);
                }
                else
                {
                    destMatrix->setCoefficient(lt, ct, 0);
                }
            }
        }

        m_sigPointChanged->asyncEmit(destMatrix);
    }

    m_previousSliderPosition = m_currentSliderPosition;
}

//-------------------------------------------------------------------------------------------------------------

void SMoveAlongSpline::computeTransformationMatrix (double position)
{
    vtkSmartPointer<vtkMath> math = vtkSmartPointer<vtkMath>::New();
    ::fwData::TransformationMatrix3D::sptr destMatrix = ::fwData::TransformationMatrix3D::New();
    double u[3], Pt[3], Ptnext[3], Du[3], x[3], y[3], z[3];

    if(position < 1)
    {
        u[0] = position;
        m_parametricSpline->Evaluate(u, Pt, Du);
        u[0] = (position + 0.01);
        m_parametricSpline->Evaluate(u, Ptnext, Du);
    }

    else if(position >=1 )
    {
        u[0] = position - 0.01;
        m_parametricSpline->Evaluate(u, Pt, Du);
        u[0] = position;
        m_parametricSpline->Evaluate(u, Ptnext, Du);
    }

    for (int n = 0; n < 3; n++)
    {
        x[n] = Ptnext[n] - Pt[n];
    }
    math->Normalize(x);

    // Compute the position of camera axes
    math->Cross(x, m_previousyVector, z);
    math->Normalize(z);
    math->Cross(z, x, y);
    math->Normalize(y);
    std::copy(y, y + 3, m_previousyVector);

    // Rotation for the orientation of camera axes
    for (int lt = 0; lt < 4; lt++)
    {
        for (int ct = 0; ct < 3; ct++)
        {
            if (lt < 3)
            {
                if (ct == 0)
                {
                    destMatrix->setCoefficient(lt, ct, x[lt]);
                }
                else if (ct == 1)
                {
                    destMatrix->setCoefficient(lt, ct, y[lt]);
                }
                else
                {
                    destMatrix->setCoefficient(lt, ct, z[lt]);
                }
            }
            else
            {
                destMatrix->setCoefficient(3, ct, 0);
            }
        }
    }

    // Translation for the position of the camera
    for (int lt = 0; lt < 4; lt++)
    {
        if (lt < 3)
        {
            destMatrix->setCoefficient(lt, 3, Pt[lt]);
        }
        else
        {
            destMatrix->setCoefficient(lt, 3, 1);
        }
    }

    // Send the matrix to SJumpToPoint
    m_sigPointChanged->asyncEmit(destMatrix);
}

//------------------------------------------------------------------------------

void SMoveAlongSpline::moveToSelectedPoint (::fwData::Point::sptr point)
{
    ::fwData::TransformationMatrix3D::sptr destMatrix = ::fwData::TransformationMatrix3D::New();

    // Rotation is set to identity
    for (int lt = 0; lt < 4; lt++)
    {
        for (int ct = 0; ct < 3; ct++)
        {
            if (lt == ct )
            {
                destMatrix->setCoefficient(lt, ct, 1);
            }
            else
            {
                destMatrix->setCoefficient(lt, ct, 0);
            }
        }
    }

    // Translation: move to the selected point
    for (int lt = 0; lt < 4; lt++)
    {
        if (lt < 3)
        {
            destMatrix->setCoefficient(lt, 3, point->getRefCoord()[lt]);
        }
        else
        {
            destMatrix->setCoefficient(lt, 3, 1);
        }
    }

    // Send the destination matrix to SJumpToPoint
    m_sigPointChanged->asyncEmit(destMatrix);

}

//------------------------------------------------------------------------------

void SMoveAlongSpline::setCameraRotation (double angle)
{
    ::fwData::PointList::csptr pointList = this->getInput< ::fwData::PointList>(S_POINTS_KEY);

    m_angle = angle;
    ::navigation::initializeVectors(pointList, m_parametricSpline, &m_previousyVector[0], m_angle);
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType SMoveAlongSpline::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::PointList::s_MODIFIED_SIG, s_UPDATE_SPLINE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::PointList::s_POINT_ADDED_SIG, s_ADD_POINT_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::PointList::s_POINT_REMOVED_SIG, s_REMOVE_POINT_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------


} // ctrlSplineNavigation

