/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoCalibration/SMarkerToPoint.hpp"

#include <fwData/Composite.hpp>
#include <fwData/Array.hpp>

#include <arData/CalibrationInfo.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwComEd/helper/Array.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>


namespace videoCalibration
{
fwServicesRegisterMacro(::fwServices::IController, ::videoCalibration::SMarkerToPoint, ::fwData::PointList);

const ::fwCom::Slots::SlotKeyType SMarkerToPoint::s_ADD_MARKER_SLOT = "addMarker";

// ----------------------------------------------------------------------------

SMarkerToPoint::SMarkerToPoint() throw () :
    m_matrixTLUid("")
{
    newSlot( s_ADD_MARKER_SLOT, &SMarkerToPoint::findPosition, this );
}

// ----------------------------------------------------------------------------

SMarkerToPoint::~SMarkerToPoint() throw ()
{
}

// ----------------------------------------------------------------------------

void SMarkerToPoint::configuring() throw (fwTools::Failed)
{
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    const ConfigurationType TLConfig = m_configuration->findConfigurationElement("matrixTL");
    SLM_ASSERT("element 'matrixTL' is missing.", TLConfig);
    m_matrixTLUid = TLConfig->getValue();
}

// ----------------------------------------------------------------------------

void SMarkerToPoint::starting() throw (fwTools::Failed)
{
    ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(m_matrixTLUid);
    SLM_ASSERT("Object '" + m_matrixTLUid + "' is not found", obj);
    m_matrixTL = ::extData::MatrixTL::dynamicCast(obj);
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

void SMarkerToPoint::findPosition()
{
    ::fwData::PointList::sptr pl = this->getObject< ::fwData::PointList >();

    ::fwData::TransformationMatrix3D::sptr matrix3D = ::fwData::TransformationMatrix3D::New();

    ::fwCore::HiResClock::HiResClockType currentTimestamp = ::fwCore::HiResClock::getTimeInMilliSec();
    CSPTR(::extData::MatrixTL::BufferType) buffer         = m_matrixTL->getClosestBuffer(currentTimestamp);
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

