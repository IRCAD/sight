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
