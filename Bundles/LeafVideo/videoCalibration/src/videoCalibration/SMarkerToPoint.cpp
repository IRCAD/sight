/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoCalibration/SMarkerToPoint.hpp"

#include <arData/MatrixTL.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>


namespace videoCalibration
{
fwServicesRegisterMacro(::fwServices::IController, ::videoCalibration::SMarkerToPoint, ::fwData::PointList);

const ::fwCom::Slots::SlotKeyType SMarkerToPoint::s_ADD_POINT_SLOT = "addPoint";

// ----------------------------------------------------------------------------

SMarkerToPoint::SMarkerToPoint() throw ()
{
    newSlot( s_ADD_POINT_SLOT, &SMarkerToPoint::addPoint, this );
}

// ----------------------------------------------------------------------------

SMarkerToPoint::~SMarkerToPoint() throw ()
{
}

// ----------------------------------------------------------------------------

void SMarkerToPoint::configuring() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SMarkerToPoint::starting() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SMarkerToPoint::swapping() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SMarkerToPoint::updating() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SMarkerToPoint::stopping() throw (fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SMarkerToPoint::addPoint()
{
    ::arData::MatrixTL::csptr matrixTL = this->getInput< ::arData::MatrixTL >("matrixTL");
    ::fwData::PointList::sptr pl       = this->getInOut< ::fwData::PointList >("pointList");

    ::fwData::TransformationMatrix3D::sptr matrix3D = ::fwData::TransformationMatrix3D::New();

    ::fwCore::HiResClock::HiResClockType currentTimestamp = ::fwCore::HiResClock::getTimeInMilliSec();
    CSPTR(::arData::MatrixTL::BufferType) buffer          = matrixTL->getClosestBuffer(currentTimestamp);
    OSLM_ASSERT("Buffer not found with timestamp " << currentTimestamp, buffer);

    const float* values = buffer->getElement(0);

    for(unsigned int i = 0; i < 4; ++i)
    {
        for(unsigned int j = 0; j < 4; ++j)
        {
            matrix3D->setCoefficient(i,j,values[i*4+j]);
        }
    }

    OSLM_DEBUG("Marker Center Position : "<< matrix3D->getCoefficient(0,3)<<" , "
                                          <<matrix3D->getCoefficient(1,3)<<" , "
                                          <<matrix3D->getCoefficient(2,3));

    //Save the position and drop the orientation
    ::fwData::Point::sptr p = ::fwData::Point::New(matrix3D->getCoefficient(0,3),
                                                   matrix3D->getCoefficient(1,3),
                                                   matrix3D->getCoefficient(2,3));

    pl->pushBack(p);
    auto sig = pl->signal< ::fwData::PointList::PointAddedSignalType >(::fwData::PointList::s_POINT_ADDED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit(p);
    }


}

// ----------------------------------------------------------------------------

} //namespace videoCalibration

