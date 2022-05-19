/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "io/igtl/detail/converter/TrackingStopConverter.hpp"

#include "io/igtl/detail/DataConverter.hpp"

#include <data/Boolean.hpp>
#include <data/Composite.hpp>

#include <igtlTrackingDataMessage.h>

namespace sight::io::igtl::detail
{

namespace converter
{

const std::string TrackingStopConverter::s_IGTL_TYPE          = "STP_TDATA";
const std::string TrackingStopConverter::s_FWDATA_OBJECT_TYPE = data::Composite::classname();
const std::string s_statusKey                                 = "Status";

converterRegisterMacro(io::igtl::detail::converter::TrackingStopConverter);

TrackingStopConverter::TrackingStopConverter()
{
}

//-----------------------------------------------------------------------------

TrackingStopConverter::~TrackingStopConverter()
{
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer TrackingStopConverter::fromFwDataObject(data::Object::csptr /*src*/) const
{
    ::igtl::StopTrackingDataMessage::Pointer trackingMsg = ::igtl::StopTrackingDataMessage::New();
    return ::igtl::MessageBase::Pointer(trackingMsg.GetPointer());
}

//-----------------------------------------------------------------------------

data::Object::sptr TrackingStopConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer /*src*/) const
{
    data::Composite::sptr composite = data::Composite::New();
    data::Boolean::sptr status      = data::Boolean::New();
    (*composite)[s_statusKey] = status;

    status->setValue(false);

    return composite;
}

//-----------------------------------------------------------------------------

IConverter::sptr TrackingStopConverter::New()
{
    return std::make_shared<TrackingStopConverter>();
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

} // namespace sight::io::igtl::detail
