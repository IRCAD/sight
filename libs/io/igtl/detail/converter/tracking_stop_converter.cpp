/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/igtl/detail/converter/tracking_stop_converter.hpp"

#include "io/igtl/detail/data_converter.hpp"

#include <data/boolean.hpp>
#include <data/composite.hpp>

#include <igtlTrackingDataMessage.h>

namespace sight::io::igtl::detail::converter
{

const std::string tracking_stop_converter::IGTL_TYPE          = "STP_TDATA";
const std::string tracking_stop_converter::FWDATA_OBJECT_TYPE = data::composite::classname();
const std::string STATUS_KEY                                  = "Status";

CONVERTER_REGISTER_MACRO(io::igtl::detail::converter::tracking_stop_converter);

tracking_stop_converter::tracking_stop_converter()
= default;

//-----------------------------------------------------------------------------

tracking_stop_converter::~tracking_stop_converter()
= default;

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer tracking_stop_converter::from_fw_data_object(data::object::csptr /*src*/) const
{
    ::igtl::StopTrackingDataMessage::Pointer tracking_msg = ::igtl::StopTrackingDataMessage::New();
    return {tracking_msg.GetPointer()};
}

//-----------------------------------------------------------------------------

data::object::sptr tracking_stop_converter::from_igtl_message(const ::igtl::MessageBase::Pointer /*src*/) const
{
    data::composite::sptr composite = std::make_shared<data::composite>();
    data::boolean::sptr status      = std::make_shared<data::boolean>();
    (*composite)[STATUS_KEY] = status;

    status->set_value(false);

    return composite;
}

//-----------------------------------------------------------------------------

base::sptr tracking_stop_converter::New()
{
    return std::make_shared<tracking_stop_converter>();
}

//-----------------------------------------------------------------------------

std::string const& tracking_stop_converter::get_igtl_type() const
{
    return tracking_stop_converter::IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& tracking_stop_converter::get_fw_data_object_type() const
{
    return tracking_stop_converter::FWDATA_OBJECT_TYPE;
}

} // namespace sight::io::igtl::detail::converter
