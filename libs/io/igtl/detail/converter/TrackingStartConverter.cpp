/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/igtl/detail/converter/TrackingStartConverter.hpp"

#include "io/igtl/detail/DataConverter.hpp"

#include <data/Boolean.hpp>
#include <data/Composite.hpp>
#include <data/Integer.hpp>
#include <data/String.hpp>

#include <igtlTrackingDataMessage.h>

namespace sight::io::igtl::detail::converter
{

const std::string TrackingStartConverter::s_IGTL_TYPE          = "STT_TDATA";
const std::string TrackingStartConverter::s_FWDATA_OBJECT_TYPE = data::Composite::classname();
const std::string s_statusKey                                  = "Status";
const std::string s_resolutionKey                              = "Resolution";
const std::string s_coordinateNameKey                          = "CoordinateName";

converterRegisterMacro(io::igtl::detail::converter::TrackingStartConverter);

TrackingStartConverter::TrackingStartConverter()
= default;

//-----------------------------------------------------------------------------

TrackingStartConverter::~TrackingStartConverter()
= default;

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer TrackingStartConverter::fromFwDataObject(data::Object::csptr /*src*/) const
{
    ::igtl::StartTrackingDataMessage::Pointer trackingMsg = ::igtl::StartTrackingDataMessage::New();

    trackingMsg->SetResolution(0);
    trackingMsg->SetCoordinateName("Undefined");

    return {trackingMsg.GetPointer()};
}

//-----------------------------------------------------------------------------

data::Object::sptr TrackingStartConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const
{
    auto* msg = dynamic_cast< ::igtl::StartTrackingDataMessage*>(src.GetPointer());

    ::igtl::StartTrackingDataMessage::Pointer trackingMsg = ::igtl::StartTrackingDataMessage::Pointer(msg);

    data::Composite::sptr composite = std::make_shared<data::Composite>();
    {
        data::Boolean::sptr status = std::make_shared<data::Boolean>();
        (*composite)[s_statusKey] = status;

        status->setValue(true);
    }
    {
        data::Integer::sptr resolution = std::make_shared<data::Integer>();
        (*composite)[s_resolutionKey] = resolution;

        resolution->setValue(trackingMsg->GetResolution());
    }
    {
        data::String::sptr coordinateName = std::make_shared<data::String>();
        (*composite)[s_coordinateNameKey] = coordinateName;

        coordinateName->setValue(trackingMsg->GetCoordinateName());
    }

    return composite;
}

//-----------------------------------------------------------------------------

base::sptr TrackingStartConverter::New()
{
    return std::make_shared<TrackingStartConverter>();
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

} // namespace sight::io::igtl::detail::converter
