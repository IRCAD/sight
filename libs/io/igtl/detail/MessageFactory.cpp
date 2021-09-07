/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

MessageFactory::CreatorContainer MessageFactory::s_creators = MessageFactory::initFactory();

//------------------------------------------------------------------------------

MessageFactory::CreatorContainer MessageFactory::initFactory()
{
    MessageFactory::CreatorContainer container;

    // Create messages without parameters.
    container["TRANSFORM"] = &MessageMaker< ::igtl::TransformMessage, false>::createMessage;
    container["IMAGE"]     = &MessageMaker< ::igtl::ImageMessage, false>::createMessage;
    container["POINT"]     = &MessageMaker< ::igtl::PointMessage, false>::createMessage;
    container["STRING"]    = &MessageMaker< ::igtl::StringMessage, false>::createMessage;
    container["POSITION"]  = &MessageMaker< ::igtl::PositionMessage, false>::createMessage;
    container["POLYDATA"]  = &MessageMaker< ::igtl::PolyDataMessage, false>::createMessage;
    container["TDATA"]     = &MessageMaker< ::igtl::TrackingDataMessage, false>::createMessage;
    container["STT_TDATA"] = &MessageMaker< ::igtl::StartTrackingDataMessage, false>::createMessage;
    container["STP_TDATA"] = &MessageMaker< ::igtl::StopTrackingDataMessage, false>::createMessage;

    // Messages with parameters (custom igtl message from sight).
    container["ATOMS"]   = std::bind(&MessageMaker<RawMessage, true>::createMessage, "ATOMS");
    container["INTEGER"] = std::bind(&MessageMaker<RawMessage, true>::createMessage, "INTEGER");
    container["FLOAT"]   = std::bind(&MessageMaker<RawMessage, true>::createMessage, "FLOAT");
    return container;
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer MessageFactory::create(const std::string& type)
{
    CreatorContainer::const_iterator it;

    if((it = MessageFactory::s_creators.find(type)) != MessageFactory::s_creators.end())
    {
        return it->second();
    }

    throw io::igtl::detail::exception::Conversion("Cannot find a creator for type " + type);
}

} //namespace sight::io::igtl::detail
