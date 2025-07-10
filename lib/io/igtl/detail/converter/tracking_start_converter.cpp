/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "io/igtl/detail/converter/tracking_start_converter.hpp"

#include "io/igtl/detail/data_converter.hpp"

#include <data/boolean.hpp>
#include <data/integer.hpp>
#include <data/map.hpp>
#include <data/string.hpp>

#include <igtlTrackingDataMessage.h>

namespace sight::io::igtl::detail::converter
{

const std::string tracking_start_converter::IGTL_TYPE          = "STT_TDATA";
const std::string tracking_start_converter::FWDATA_OBJECT_TYPE = data::map::classname();
const std::string STATUS_KEY                                   = "Status";
const std::string RESOLUTION_KEY                               = "Resolution";
const std::string COORDINATE_NAME_KEY                          = "CoordinateName";

CONVERTER_REGISTER_MACRO(io::igtl::detail::converter::tracking_start_converter);

tracking_start_converter::tracking_start_converter()
= default;

//-----------------------------------------------------------------------------

tracking_start_converter::~tracking_start_converter()
= default;

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer tracking_start_converter::from_fw_data_object(data::object::csptr /*src*/) const
{
    ::igtl::StartTrackingDataMessage::Pointer tracking_msg = ::igtl::StartTrackingDataMessage::New();

    tracking_msg->SetResolution(0);
    tracking_msg->SetCoordinateName("Undefined");

    return {tracking_msg.GetPointer()};
}

//-----------------------------------------------------------------------------

data::object::sptr tracking_start_converter::from_igtl_message(const ::igtl::MessageBase::Pointer _src) const
{
    auto* msg = dynamic_cast< ::igtl::StartTrackingDataMessage*>(_src.GetPointer());

    ::igtl::StartTrackingDataMessage::Pointer tracking_msg = ::igtl::StartTrackingDataMessage::Pointer(msg);

    data::map::sptr map = std::make_shared<data::map>();
    {
        data::boolean::sptr status = std::make_shared<data::boolean>();
        (*map)[STATUS_KEY] = status;

        status->set_value(true);
    }
    {
        data::integer::sptr resolution = std::make_shared<data::integer>();
        (*map)[RESOLUTION_KEY] = resolution;

        resolution->set_value(tracking_msg->GetResolution());
    }
    {
        data::string::sptr coordinate_name = std::make_shared<data::string>();
        (*map)[COORDINATE_NAME_KEY] = coordinate_name;

        coordinate_name->set_value(tracking_msg->GetCoordinateName());
    }

    return map;
}

//-----------------------------------------------------------------------------

base::sptr tracking_start_converter::New()
{
    return std::make_shared<tracking_start_converter>();
}

//-----------------------------------------------------------------------------

std::string const& tracking_start_converter::get_igtl_type() const
{
    return tracking_start_converter::IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& tracking_start_converter::get_fw_data_object_type() const
{
    return tracking_start_converter::FWDATA_OBJECT_TYPE;
}

} // namespace sight::io::igtl::detail::converter
