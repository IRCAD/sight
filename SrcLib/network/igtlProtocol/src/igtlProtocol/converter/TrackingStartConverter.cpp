/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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

::fwData::Object::sptr TrackingStartConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const
{
    ::igtl::StartTrackingDataMessage* msg = dynamic_cast< ::igtl::StartTrackingDataMessage* >(src.GetPointer());

    ::igtl::StartTrackingDataMessage::Pointer trackingMsg = ::igtl::StartTrackingDataMessage::Pointer(msg);

    ::fwData::Composite::sptr composite = ::fwData::Composite::New();
    {
        ::fwData::Boolean::sptr status = ::fwData::Boolean::New();
        (*composite)[s_statusKey]      = status;

        status->setValue(true);
    }
    {
        ::fwData::Integer::sptr resolution = ::fwData::Integer::New();
        (*composite)[s_resolutionKey]      = resolution;

        resolution->setValue(trackingMsg->GetResolution());
    }
    {
        ::fwData::String::sptr coordinateName = ::fwData::String::New();
        (*composite)[s_coordinateNameKey]     = coordinateName;

        coordinateName->setValue(trackingMsg->GetCoordinateName());
    }

    return composite;
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
