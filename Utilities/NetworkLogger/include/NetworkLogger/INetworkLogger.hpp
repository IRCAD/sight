/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
    virtual void log(std::ostream& outputStream) throw (std::runtime_error) = 0;

    /**
     * @brief configure the network logger
     *
     * @param[in] argc number of arguments
     * @param[in] argv arguments
     */
    virtual void configure(int argc, char** argv) throw (std::runtime_error) = 0;
};
}

#endif // __NETWORKLOGGER_INETWORKLOGGER_HPP__