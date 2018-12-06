/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "NetworkPlayer/ZeroMQPlayer.hpp"

namespace networkPlayer
{
ZeroMQPlayer::ZeroMQPlayer() : m_socket(::zmqNetwork::Socket(::zmqNetwork::Socket::Server,
                                                             ::zmqNetwork::Socket::Publish))
{

}

ZeroMQPlayer::~ZeroMQPlayer()
{

}

void ZeroMQPlayer::configure(int argc, char** argv)
{
    if (argc == 0)
    {
        throw std::runtime_error("./NetworkPlayer zmq <host>");
    }
    else
    {
        m_host = std::string(argv[0]);
    }
}

INetworkPlayer::sptr ZeroMQPlayer::create(FunctionType const& playFunction)
{
    ZeroMQPlayer* player = new ZeroMQPlayer();

    player->m_playFunction = playFunction;
    return INetworkPlayer::sptr(player);
}

void ZeroMQPlayer::play(std::istream& stream)
{
    m_socket.start (m_host);
    m_playFunction(stream, m_socket);
    m_socket.stop();
}
}
