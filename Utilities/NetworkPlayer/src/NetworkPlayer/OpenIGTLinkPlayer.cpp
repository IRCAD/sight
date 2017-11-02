/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "NetworkPlayer/OpenIGTLinkPlayer.hpp"

#include <boost/lexical_cast.hpp>

namespace networkPlayer
{
OpenIGTLinkPlayer::OpenIGTLinkPlayer()
{
}

OpenIGTLinkPlayer::~OpenIGTLinkPlayer()
{

}

//------------------------------------------------------------------------------

void OpenIGTLinkPlayer::configure(int argc, char** argv)
{
    if (argc == 0)
    {
        throw std::runtime_error("./NetworkPlayer igtl logfile port");
    }
    else
    {
        m_port = ::boost::lexical_cast< std::uint16_t>(argv[0]);
    }
}

//------------------------------------------------------------------------------

INetworkPlayer::sptr OpenIGTLinkPlayer::create(FunctionType const& playFunction)
{
    OpenIGTLinkPlayer* player = new OpenIGTLinkPlayer();

    player->m_playFunction = playFunction;
    return INetworkPlayer::sptr(player);
}

//------------------------------------------------------------------------------

void OpenIGTLinkPlayer::play(std::istream& stream)
{
    ::igtlNetwork::Client::sptr client;

    m_server.start(m_port);
    client = m_server.waitForConnection();
    m_playFunction(stream, *client);
    m_server.stop();
}
}
