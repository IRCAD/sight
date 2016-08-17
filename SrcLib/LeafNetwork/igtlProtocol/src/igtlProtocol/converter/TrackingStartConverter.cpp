/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "igtlProtocol/converter/TrackingStartConverter.hpp"
#include "igtlProtocol/DataConverter.hpp"

#include <fwData/Boolean.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Integer.hpp>
#include <fwData/String.hpp>

#include <igtl/igtlTrackingDataMessage.h>

namespace igtlProtocol
{
namespace converter
{

const std::string TrackingStartConverter::s_IGTL_TYPE          = "STT_TDATA";
const std::string TrackingStartConverter::s_FWDATA_OBJECT_TYPE = ::fwData::Composite::classname();
const std::string s_statusKey                                  = "Status";
const std::string s_resolutionKey                              = "Resolution";
const std::string s_coordinateNameKey                          = "CoordinateName";

converterRegisterMacro(::igtlProtocol::converter::TrackingStartConverter);

TrackingStartConverter::TrackingStartConverter()
{
}

//-----------------------------------------------------------------------------

TrackingStartConverter::~TrackingStartConverter()
{
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer TrackingStartConverter::fromFwDataObject(::fwData::Object::csptr src) const
{
    ::igtl::StartTrackingDataMessage::Pointer trackingMsg = ::igtl::StartTrackingDataMessage::New();

    trackingMsg->SetResolution(0);
    trackingMsg->SetCoordinateName("Undefined");

    return ::igtl::MessageBase::Pointer(trackingMsg.GetPointer());
}

//-----------------------------------------------------------------------------

void TrackingStartConverter::fromIgtlMessage(::igtl::MessageBase::Pointer const src,
                                             ::fwData::Object::sptr& destObj) const
throw (::igtlProtocol::exception::Conversion)
{
    FW_RAISE_EXCEPTION_IF(exception::Conversion("Incompatible destination object type must be a "
                                                "::fwData::Composite"),
                          destObj->getClassname() != TrackingStartConverter::s_FWDATA_OBJECT_TYPE);

    ::igtl::StartTrackingDataMessage::Pointer trackingMsg;
    trackingMsg =
        ::igtl::StartTrackingDataMessage::Pointer(dynamic_cast< ::igtl::StartTrackingDataMessage* >(src.GetPointer()));


    ::fwData::Composite::sptr composite = ::fwData::Composite::dynamicCast(destObj);
    {
        ::fwData::Boolean::sptr status;
        ::fwData::Composite::iterator iter = composite->find(s_statusKey);
        if( iter != composite->end())
        {
            status = ::fwData::Boolean::dynamicCast(iter->second);
            SLM_ASSERT("Object '"+s_statusKey+"' already exist, but it's not a Boolean.", status);
        }
        else
        {
            status                    = ::fwData::Boolean::New();
            (*composite)[s_statusKey] = status;
        }
        status->setValue(true);
    }
    {
        ::fwData::Integer::sptr resolution;
        ::fwData::Composite::iterator iter = composite->find(s_resolutionKey);
        if( iter != composite->end())
        {
            resolution = ::fwData::Integer::dynamicCast(iter->second);
            SLM_ASSERT("Object '"+s_resolutionKey+"' already exist, but it's not an Integer.", resolution);
        }
        else
        {
            resolution                    = ::fwData::Integer::New();
            (*composite)[s_resolutionKey] = resolution;
        }
        resolution->setValue(trackingMsg->GetResolution());
    }
    {
        ::fwData::String::sptr coordinateName;
        ::fwData::Composite::iterator iter = composite->find(s_coordinateNameKey);
        if( iter != composite->end())
        {
            coordinateName = ::fwData::String::dynamicCast(iter->second);
            SLM_ASSERT("Object '"+s_coordinateNameKey+"' already exist, but it's not a String.", coordinateName);
        }
        else
        {
            coordinateName                    = ::fwData::String::New();
            (*composite)[s_coordinateNameKey] = coordinateName;
        }
        coordinateName->setValue(trackingMsg->GetCoordinateName());
    }
}

//-----------------------------------------------------------------------------

IConverter::sptr TrackingStartConverter::New()
{
    return std::make_shared< TrackingStartConverter >();
}

//-----------------------------------------------------------------------------

std::string const& TrackingStartConverter::getIgtlType() const
{
    return TrackingStartConverter::s_IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& TrackingStartConverter::getFwDataObjectType() const
{
    return TrackingStartConverter::s_FWDATA_OBJECT_TYPE;
}

} // namespace converter
} // namespace igtlProtocol
