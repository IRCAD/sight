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

#ifndef __NETWORKLOGGER_INETWORKLOGGER_HPP__
#define __NETWORKLOGGER_INETWORKLOGGER_HPP__

#include <fwCore/macros.hpp>
#include <ostream>
#include <stdexcept>

namespace networkLogger
{
/**
 */
class INetworkLogger
{
public:
    /**
     * @typedef sptr
     *
     * @brief smart pointer of INetworkLogger
     */
    typedef SPTR (INetworkLogger)    sptr;

    /// virtual destructor
    virtual ~INetworkLogger()
    {
    }

    /**
     * @brief log receive object from network in ouput stream
     */
    virtual void log(std::ostream& outputStream) = 0;

    /**
     * @brief configure the network logger
     *
     * @param[in] argc number of arguments
     * @param[in] argv arguments
     */
    virtual void configure(int argc, char** argv) = 0;
};
}

#endif // __NETWORKLOGGER_INETWORKLOGGER_HPP__