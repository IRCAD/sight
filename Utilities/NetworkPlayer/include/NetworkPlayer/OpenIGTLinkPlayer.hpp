/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __NETWORKPLAYER_OPENIGTLINKPLAYER_HPP__
#define __NETWORKPLAYER_OPENIGTLINKPLAYER_HPP__

#include "NetworkPlayer/INetworkPlayer.hpp"
#include "NetworkPlayer/types.hpp"

#include <igtlNetwork/Server.hpp>

#include <istream>
#include <boost/function.hpp>

namespace networkPlayer
{
/**
 *
 * @brief a network player use OpenIGTLinkNetwork socket
 */
class OpenIGTLinkPlayer : public INetworkPlayer
{
public:

    /**
     * @typedef SocketType
     *
     * @brief define the OpenIGTLinkNetwork::Client to be the socket type to use
     */
    typedef ::igtlNetwork::Client SocketType;

    /**
     * @typedef FunctionType
     *
     * @brief define the player user-define function type to use
     */
    typedef PlayerFunction<SocketType>::FunctionType FunctionType;

    /// constructor
    OpenIGTLinkPlayer();

    /// destructor
    ~OpenIGTLinkPlayer();

    /**
     * @brief configure the port of the OpenIGTLinkPlayer instance
     *
     * @param[in] argc number of arguments
     * @param[in] argv arguments
     */
    void configure(int argc, char** argv);

    /**
     * @brief play the matrix sequence in the input stream
     *
     */
    void play(std::istream& inputStream);

    /**
     * @brief create a OpenIGTLinkPlayer with a player user-defined function(play is like a incomplete method which user have to complete with the playFunction)
     *
     * @param[in] playFunction play function to read stream and send data to client
     * @return a instance of a OpenIGTLinkPlayer in a smart pointer of INetworkPlayer
     */
    static INetworkPlayer::sptr create(FunctionType const& playFunction);
private:

    /// server socket
    ::igtlNetwork::Server m_server;

    /// port to listen
    ::boost::uint16_t m_port;

    /// user-defined function
    FunctionType m_playFunction;
};
}

#endif // __NETWORKPLAYER_OPENIGTLINKPLAYER_HPP__
