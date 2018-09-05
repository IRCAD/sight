/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __NETWORKPLAYER_INETWORKPLAYER_HPP__
#define __NETWORKPLAYER_INETWORKPLAYER_HPP__

#include <fwCore/macros.hpp>
#include <istream>
#include <stdexcept>

namespace networkPlayer
{
/**
 *
 * @brief Interface define functionality a network player have
 */
class INetworkPlayer
{
public:

    /**
     * @typedef sptr
     *
     * @brief smart pointer of a INetworkPlayer
     */
    typedef SPTR (INetworkPlayer)    sptr;

    /// destructor
    virtual ~INetworkPlayer()
    {
    }

    /**
     * @brief configure the network player
     *
     * @param[in] argc number of arguments
     * @param[in] argv arguments contain like port to bind
     */
    virtual void configure(int argc, char** argv) = 0;

    /**
     * @brief play a sequence of matrix in the inputStream
     */
    virtual void play(std::istream& inputStream) = 0;
};
}

#endif // __NETWORKPLAYER_INETWORKPLAYER_HPP__