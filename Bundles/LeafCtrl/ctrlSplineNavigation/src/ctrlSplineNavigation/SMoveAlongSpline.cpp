/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

//Slot
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwThread/Worker.hpp>

#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>

#include <fwComEd/PointListMsg.hpp>

#include <vtkMath.h>

// Library
#include <navigation/functions.hpp>

#include "ctrlSplineNavigation/SMoveAlongSpline.hpp"

namespace ctrlSplineNavigation
{
const ::fwCom::Signals::SignalKeyType SMoveAlongSpline::s_POINT_CHANGED_SIG = "PointChanged";
const ::fwCom::Signals::SignalKeyType SMoveAlongSpline::s_LENGTH_CHANGED_SIG = "SplineLengthChanged";
const ::fwCom::Slots::SlotKeyType SMoveAlongSpline::s_CHANGE_SLIDER_VALUE_SLOT = "changeSliderValue";
const ::fwCom::Slots::SlotKeyType SMoveAlongSpline::s_CHANGE_SELECTED_POINT_SLOT = "changeSelectedPoint";
const ::fwCom::Slots::SlotKeyType SMoveAlongSpline::s_CHANGE_ANGLE_SLOT = "changeAngle";

//-------------------------------------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::IController, ::ctrlSplineNavigation::SMoveAlongSpline, ::fwData::PointList ) ;

//-------------------------------------------------------------------------------------------------------------

SMoveAlongSpline::SMoveAlongSpline() throw():
    m_currentSliderPosition(0.0), m_previousSliderPosition(0.0), m_nbSplinePoints(0), m_splineLength(0.0), m_angle(0)
{
    // Create a parametric Spline object
    m_parametricSpline = vtkSmartPointer<vtkParametricSpline>::New();

    // Create a vtkPoints object and store the points in it
    m_vtkPoints = vtkSmartPointer<vtkPoints>::New();

    // Init signals
    m_sigPointChanged = PointChangedSignalType::New();
    m_sigSplineLengthChanged = SplineLengthChangedSignalType::New();

    // Register
    ::fwCom::HasSignals::m_signals(s_POINT_CHANGED_SIG, m_sigPointChanged);
    ::fwCom::HasSignals::m_signals(s_LENGTH_CHANGED_SIG,m_sigSplineLengthChanged);

#ifdef COM_LOG
    m_sigPointChanged->setID( s_POINT_CHANGED_SIG );
    m_sigSplineLengthChanged->setID( s_LENGTH_CHANGED_SIG );
#endif

    // Init slots
    m_slotChangeSliderValue = ::fwCom::newSlot(&SMoveAlongSpline::moveToPoint,this);
    ::fwCom::HasSlots::m_slots(s_CHANGE_SLIDER_VALUE_SLOT, m_slotChangeSliderValue);

    m_slotChangeSelectedPoint = ::fwCom::newSlot(&SMoveAlongSpline::moveToSelectedPoint,this);
    ::fwCom::HasSlots::m_slots(s_CHANGE_SELECTED_POINT_SLOT, m_slotChangeSelectedPoint);

    m_slotChangeAngle = ::fwCom::newSlot( &SMoveAlongSpline::setCameraRotation, this ) ;
          ::fwCom::HasSlots::m_slots(s_CHANGE_ANGLE_SLOT, m_slotChangeAngle);

    // Set default worker to new slot
    this->setWorker(::fwServices::registry::ActiveWorkers::getDefault()->
            getWorker(::fwServices::registry::ActiveWorkers::s_DEFAULT_WORKER));

}

//-------------------------------------------------------------------------------------------------------------

SMoveAlongSpline::~SMoveAlongSpline() throw()
{}

//-------------------------------------------------------------------------------------------------------------

void SMoveAlongSpline::configuring()  throw ( ::fwTools::Failed )
{}

//-------------------------------------------------------------------------------------------------------------

void SMoveAlongSpline::starting()  throw ( ::fwTools::Failed )
{
    ::fwData::PointList::sptr pointList = this->getObject< ::fwData::PointList>();
    m_nbSplinePoints = pointList->getRefPoints().size();
    if (m_nbSplinePoints == 0)
    {
        return;
    }

    ::navigation::computeSpline(pointList, m_nbSplinePoints - 1, m_vtkPoints, m_parametricSpline, m_splineLength);
    fwServicesNotifyMacro(this->getLightID(), m_sigSplineLengthChanged,(m_splineLength));

    if(m_nbSplinePoints>=2)
    {
        ::navigation::initializeVectors(pointList,m_parametricSpline,&m_previousyVector[0],m_angle);
    }

}

//-------------------------------------------------------------------------------------------------------------

void SMoveAlongSpline::stopping()  throw ( ::fwTools::Failed )
{}

//-------------------------------------------------------------------------------------------------------------

void SMoveAlongSpline::updating() throw ( ::fwTools::Failed )
{}

//-------------------------------------------------------------------------------------------------------------

void SMoveAlongSpline::receiving( ::fwServices::ObjectMsg::csptr msg ) throw ( ::fwTools::Failed )
{
    // if the pointList is modified update the spline and initialize the x, y and z axes of the camera
    if( msg->hasEvent(::fwComEd::PointListMsg::ELEMENT_ADDED) )
    {
        ::fwData::PointList::sptr pointList = this->getObject< ::fwData::PointList>();
        ::navigation::computeSpline(pointList,m_nbSplinePoints,m_vtkPoints,m_parametricSpline,m_splineLength);
        fwServicesNotifyMacro(this->getLightID(), m_sigSplineLengthChanged,(m_splineLength));
        m_nbSplinePoints++;

        if(m_nbSplinePoints>=2)
        {
            ::navigation::initializeVectors(pointList,m_parametricSpline,&m_previousyVector[0],m_angle);
        }
    }

    if( msg->hasEvent(::fwComEd::PointListMsg::ELEMENT_REMOVED) )
    {
        ::fwData::PointList::sptr pointList = this->getObject< ::fwData::PointList>();
        m_nbSplinePoints--;
        ::navigation::updateSpline(pointList, m_vtkPoints,m_parametricSpline,m_splineLength);
        fwServicesNotifyMacro(this->getLightID(), m_sigSplineLengthChanged,(m_splineLength));

        if(m_nbSplinePoints >= 2)
        {
            ::navigation::initializeVectors(pointList,m_parametricSpline,&m_previousyVector[0],m_angle);
        }

    }

    if( msg->hasEvent(::fwComEd::PointListMsg::ELEMENT_MODIFIED) )
    {
        ::fwData::PointList::sptr pointList = this->getObject< ::fwData::PointList>();

        ::navigation::updateSpline(pointList, m_vtkPoints, m_parametricSpline, m_splineLength);
        fwServicesNotifyMacro(this->getLightID(), m_sigSplineLengthChanged,(m_splineLength));

        if (m_nbSplinePoints >= 2)
        {
            ::navigation::initializeVectors(pointList, m_parametricSpline, &m_previousyVector[0], m_angle);
        }
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

        // Go foward
        if( m_previousSliderPosition<=m_currentSliderPosition)
        {
            for (double position = m_previousSliderPosition;
                    position < m_currentSliderPosition;
                    position = position + 1.0 / (m_splineLength))
            {
                this->computeTransformationMatrix(position);
            }
        }

        // Go back
        else
        {
            for (double position = m_previousSliderPosition;
                    position > m_currentSliderPosition;
                    position = position - 1.0 / (m_splineLength))
            {
                this->computeTransformationMatrix(position);
            }
        }

    }

    // if there is no point set the matrix to identity
    else if(m_nbSplinePoints==0)
    {
        // Send the destination matrix to SJumpToPoint
        ::fwData::TransformationMatrix3D::sptr destMatrix =
                            ::fwData::TransformationMatrix3D::sptr(::fwData::TransformationMatrix3D::New());
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

        fwServicesNotifyMacro(this->getLightID(), m_sigPointChanged,(destMatrix));
    }

    m_previousSliderPosition = m_currentSliderPosition;
}

//-------------------------------------------------------------------------------------------------------------

void SMoveAlongSpline::computeTransformationMatrix (double position)
{
    vtkSmartPointer<vtkMath> math = vtkSmartPointer<vtkMath>::New();
    ::fwData::TransformationMatrix3D::sptr destMatrix =
               ::fwData::TransformationMatrix3D::sptr(::fwData::TransformationMatrix3D::New());
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
    fwServicesNotifyMacro(this->getLightID(), m_sigPointChanged, (destMatrix));

}
void SMoveAlongSpline::moveToSelectedPoint (::fwData::Point::sptr point)
{
    ::fwData::TransformationMatrix3D::sptr destMatrix =
            ::fwData::TransformationMatrix3D::sptr(::fwData::TransformationMatrix3D::New());


    // Rotation is set to identity
    for (int lt = 0; lt < 4; lt++)
    {
        for (int ct = 0; ct < 3; ct++)
        {
            if (lt == ct )
            {
                destMatrix->setCoefficient(lt, ct,1);
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
            destMatrix->setCoefficient(lt, 3,point->getRefCoord()[lt]);
        }
        else
        {
            destMatrix->setCoefficient(lt, 3, 1);
        }
    }

    // Send the destination matrix to SJumpToPoint
    fwServicesNotifyMacro(this->getLightID(), m_sigPointChanged,(destMatrix));

}

void SMoveAlongSpline::setCameraRotation (double angle)
{
    m_angle = angle;
    ::fwData::PointList::sptr pointList = this->getObject< ::fwData::PointList>();
    ::navigation::initializeVectors(pointList,m_parametricSpline,&m_previousyVector[0],m_angle);
}

} // ctrlSplineNavigation

