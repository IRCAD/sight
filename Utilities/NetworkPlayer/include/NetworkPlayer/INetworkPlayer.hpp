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