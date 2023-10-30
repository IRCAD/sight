/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "MessageFactory.hpp"

#include "io/igtl/detail/RawMessage.hpp"

#include <igtlImageMessage.h>
#include <igtlPointMessage.h>
#include <igtlPolyDataMessage.h>
#include <igtlPositionMessage.h>
#include <igtlStringMessage.h>
#include <igtlTrackingDataMessage.h>
#include <igtlTransformMessage.h>

namespace sight::io::igtl::detail
{

message_factory::creator_container_t message_factory::s_creators = message_factory::init_factory();

//------------------------------------------------------------------------------

message_factory::creator_container_t message_factory::init_factory()
{
    creator_container_t container;

    // Create messages without parameters.
    container["TRANSFORM"] = &MessageMaker< ::igtl::TransformMessage, false>::create_message;
    container["IMAGE"]     = &MessageMaker< ::igtl::ImageMessage, false>::create_message;
    container["POINT"]     = &MessageMaker< ::igtl::PointMessage, false>::create_message;
    container["STRING"]    = &MessageMaker< ::igtl::StringMessage, false>::create_message;
    container["POSITION"]  = &MessageMaker< ::igtl::PositionMessage, false>::create_message;
    container["POLYDATA"]  = &MessageMaker< ::igtl::PolyDataMessage, false>::create_message;
    container["TDATA"]     = &MessageMaker< ::igtl::TrackingDataMessage, false>::create_message;
    container["STT_TDATA"] = &MessageMaker< ::igtl::StartTrackingDataMessage, false>::create_message;
    container["STP_TDATA"] = &MessageMaker< ::igtl::StopTrackingDataMessage, false>::create_message;

    return container;
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer message_factory::create(const std::string& _type)
{
    creator_container_t::const_iterator it;

    if((it = message_factory::s_creators.find(_type)) != message_factory::s_creators.end())
    {
        return it->second();
    }

    throw io::igtl::detail::exception::conversion("Cannot find a creator for type " + _type);
}

} //namespace sight::io::igtl::detail
