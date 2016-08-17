/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

void ZeroMQPlayer::configure(int argc, char** argv) throw (std::runtime_error)
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

void ZeroMQPlayer::play(std::istream& stream) throw (std::runtime_error)
{
    m_socket.start (m_host);
    m_playFunction(stream, m_socket);
    m_socket.stop();
}
}
