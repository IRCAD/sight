/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "igtlProtocol/RawMessage.hpp"
#include "igtlProtocol/MessageFactory.hpp"

#include <igtl/igtlTransformMessage.h>
#include <igtl/igtlImageMessage.h>
#include <igtl/igtlPointMessage.h>
#include <igtl/igtlStringMessage.h>
#include <igtl/igtlPositionMessage.h>
#include <igtl/igtlPolyDataMessage.h>
#include <igtl/igtlTrackingDataMessage.h>

#include <boost/bind.hpp>

namespace igtlProtocol
{

MessageFactory::CreatorContainer MessageFactory::s_creators = MessageFactory::initFactory();

MessageFactory::CreatorContainer MessageFactory::initFactory()
{
    MessageFactory::CreatorContainer container;

    container["TRANSFORM"] = &MessageMaker< ::igtl::TransformMessage, false >::createMessage;
    container["IMAGE"]     = &MessageMaker< ::igtl::ImageMessage, false >::createMessage;
    container["POINT"]     = &MessageMaker< ::igtl::PointMessage, false >::createMessage;
    container["STRING"]    = &MessageMaker< ::igtl::StringMessage, false >::createMessage;
    container["POSITION"]  = &MessageMaker< ::igtl::PositionMessage, false >::createMessage;
    container["POLYDATA"]  = &MessageMaker< ::igtl::PolyDataMessage, false>::createMessage;
    container["TDATA"]     = &MessageMaker< ::igtl::TrackingDataMessage, false>::createMessage;
    container["STT_TDATA"] = &MessageMaker< ::igtl::StartTrackingDataMessage, false>::createMessage;
    container["STP_TDATA"] = &MessageMaker< ::igtl::StopTrackingDataMessage, false>::createMessage;
    container["ATOMS"]     = ::boost::bind(&MessageMaker< RawMessage, true >::createMessage, "ATOMS");
    container["INTEGER"]   = ::boost::bind(&MessageMaker< RawMessage, true >::createMessage, "INTEGER");
    container["FLOAT"]     = ::boost::bind(&MessageMaker< RawMessage, true >::createMessage, "FLOAT");
    return container;
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer MessageFactory::create(std::string const& type) throw (::igtlProtocol::exception::
                                                                                    Conversion)
{
    CreatorContainer::const_iterator it;

    if ((it = MessageFactory::s_creators.find(type)) != MessageFactory::s_creators.end())
    {
        return it->second();
    }
    throw ::igtlProtocol::exception::Conversion("Cannot find a creator for type " + type);
}

}//namespace igtlProtocol
