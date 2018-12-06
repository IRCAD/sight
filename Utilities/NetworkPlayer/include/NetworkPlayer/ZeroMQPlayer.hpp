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

#ifndef __NETWORKPLAYER_ZEROMQPLAYER_HPP__
#define __NETWORKPLAYER_ZEROMQPLAYER_HPP__

#include <istream>
#include <boost/function.hpp>
#include <zmqNetwork/Socket.hpp>
#include "NetworkPlayer/types.hpp"
#include "NetworkPlayer/INetworkPlayer.hpp"

namespace networkPlayer
{
/**
 *
 * @brief a network player use ZeroMQ sockets
 */
class ZeroMQPlayer : public INetworkPlayer
{
public:

    /**
     * @typedef SocketType
     *
     * @brief define the ZMQNetwork::Socket to be the socket type to use
     */
    typedef ::zmqNetwork::Socket SocketType;

    /**
     * @typedef FunctionType
     *
     * @brief define the player user-define function type to use
     */
    typedef PlayerFunction<SocketType>::FunctionType FunctionType;

    /// constructor
    ZeroMQPlayer();

    /// destructor
    ~ZeroMQPlayer();

    /**
     * @brief configure the string in zmq format of the ZMQNetwork::Socket instance
     *
     * @param[in] argc number of arguments
     * @param[in] argv arguments
     */
    void configure(int argc, char** argv);

    /**
     * @brief play the matrix sequence in the input stream
     *
     * @param[in] stream input stream
     */
    void play(std::istream& stream);


    /**
     * @brief create a ZeroMQPlayer with a player user-defined function(play is like a incomplete method which user have to complete with the playFunction)
     *
     * @param[in] playFunction
     * @return playOperation a instance of a ZeroMQPlayer in a smart pointer of INetworkPlayer
     */
    static INetworkPlayer::sptr create(FunctionType const& playOperation);

private:

    /// server socket
    ::zmqNetwork::Socket m_socket;

    /// host
    std::string m_host;

    /// play function to call when play is called
    FunctionType m_playFunction;
};
}

#endif // __NETWORKPLAYER_ZEROMQPLAYER_HPP__
