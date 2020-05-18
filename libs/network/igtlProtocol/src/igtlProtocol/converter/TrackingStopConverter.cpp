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

::fwData::Object::sptr TrackingStopConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const
{
    ::fwData::Composite::sptr composite = ::fwData::Composite::New();
    ::fwData::Boolean::sptr status      = ::fwData::Boolean::New();
    (*composite)[s_statusKey]           = status;

    status->setValue(false);

    return composite;
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
