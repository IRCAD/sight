/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _NETWORKPLAYER_INETWORKPLAYER_HPP_
#define _NETWORKPLAYER_INETWORKPLAYER_HPP_

#include <fwCore/macros.hpp>
#include <istream>
#include <stdexcept>

namespace networkPlayer
{
    /**
     * @class INetworkPlayer
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
        typedef SPTR(INetworkPlayer)    sptr;

        /// destructor
        virtual ~INetworkPlayer() {}

        /**
         * @brief configure the network player
         *
         * @param[in] argc number of arguments
         * @param[in] argv arguments contain like port to bind
         */
        virtual void configure(int argc, char **argv) throw(std::runtime_error) = 0;

        /**
         * @brief play a sequence of matrix in the inputStream
         */
        virtual void play(std::istream &inputStream) throw(std::runtime_error) = 0;
    };
}

#endif // _NETWORKPLAYER_INETWORKPLAYER_HPP_