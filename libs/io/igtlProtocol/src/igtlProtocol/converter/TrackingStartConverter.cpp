/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <data/Boolean.hpp>
#include <data/Composite.hpp>
#include <data/Integer.hpp>
#include <data/String.hpp>

#include <igtlTrackingDataMessage.h>

namespace igtlProtocol
{
namespace converter
{

const std::string TrackingStartConverter::s_IGTL_TYPE          = "STT_TDATA";
const std::string TrackingStartConverter::s_FWDATA_OBJECT_TYPE = data::Composite::classname();
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

::igtl::MessageBase::Pointer TrackingStartConverter::fromFwDataObject(data::Object::csptr src) const
{
    ::igtl::StartTrackingDataMessage::Pointer trackingMsg = ::igtl::StartTrackingDataMessage::New();

    trackingMsg->SetResolution(0);
    trackingMsg->SetCoordinateName("Undefined");

    return ::igtl::MessageBase::Pointer(trackingMsg.GetPointer());
}

//-----------------------------------------------------------------------------

data::Object::sptr TrackingStartConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const
{
    ::igtl::StartTrackingDataMessage* msg = dynamic_cast< ::igtl::StartTrackingDataMessage* >(src.GetPointer());

    ::igtl::StartTrackingDataMessage::Pointer trackingMsg = ::igtl::StartTrackingDataMessage::Pointer(msg);

    data::Composite::sptr composite = data::Composite::New();
    {
        data::Boolean::sptr status = data::Boolean::New();
        (*composite)[s_statusKey] = status;

        status->setValue(true);
    }
    {
        data::Integer::sptr resolution = data::Integer::New();
        (*composite)[s_resolutionKey] = resolution;

        resolution->setValue(trackingMsg->GetResolution());
    }
    {
        data::String::sptr coordinateName = data::String::New();
        (*composite)[s_coordinateNameKey] = coordinateName;

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
