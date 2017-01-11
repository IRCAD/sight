/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "igtlProtocol/converter/TrackingStopConverter.hpp"
#include "igtlProtocol/DataConverter.hpp"

#include <fwData/Boolean.hpp>
#include <fwData/Composite.hpp>

#include <igtlTrackingDataMessage.h>

namespace igtlProtocol
{
namespace converter
{

const std::string TrackingStopConverter::s_IGTL_TYPE          = "STP_TDATA";
const std::string TrackingStopConverter::s_FWDATA_OBJECT_TYPE = ::fwData::Composite::classname();
const std::string s_statusKey                                 = "Status";

converterRegisterMacro(::igtlProtocol::converter::TrackingStopConverter);

TrackingStopConverter::TrackingStopConverter()
{
}

//-----------------------------------------------------------------------------

TrackingStopConverter::~TrackingStopConverter()
{
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer TrackingStopConverter::fromFwDataObject(::fwData::Object::csptr src) const
{
    ::igtl::StopTrackingDataMessage::Pointer trackingMsg = ::igtl::StopTrackingDataMessage::New();
    return ::igtl::MessageBase::Pointer(trackingMsg.GetPointer());
}

//-----------------------------------------------------------------------------

void TrackingStopConverter::fromIgtlMessage(::igtl::MessageBase::Pointer const src,
                                            ::fwData::Object::sptr& destObj) const
throw (::igtlProtocol::exception::Conversion)
{
    FW_RAISE_EXCEPTION_IF(exception::Conversion("Incompatible destination object type must be a "
                                                "::fwData::Composite"),
                          destObj->getClassname() != TrackingStopConverter::s_FWDATA_OBJECT_TYPE);


    ::fwData::Boolean::sptr status;
    ::fwData::Composite::sptr composite = ::fwData::Composite::dynamicCast(destObj);
    ::fwData::Composite::iterator iter  = composite->find(s_statusKey);
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
    status->setValue(false);
}

//-----------------------------------------------------------------------------

IConverter::sptr TrackingStopConverter::New()
{
    return std::make_shared< TrackingStopConverter >();
}

//-----------------------------------------------------------------------------

std::string const& TrackingStopConverter::getIgtlType() const
{
    return TrackingStopConverter::s_IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& TrackingStopConverter::getFwDataObjectType() const
{
    return TrackingStopConverter::s_FWDATA_OBJECT_TYPE;
}

} // namespace converter
} // namespace igtlProtocol
