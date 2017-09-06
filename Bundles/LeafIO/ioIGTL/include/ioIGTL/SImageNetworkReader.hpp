/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOIGTL_SIMAGENETWORKREADER_HPP__
#define __IOIGTL_SIMAGENETWORKREADER_HPP__

#include "ioIGTL/config.hpp"

#include <io/IReader.hpp>

#include <igtlNetwork/Client.hpp>

#include <boost/type.hpp>

namespace ioIGTL
{
/**
 *
 * @brief image network reader using OpenIGTLinkNetwork
 */
class IOIGTL_CLASS_API SImageNetworkReader : public ::io::IReader
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SImageNetworkReader) (::io::IReader));


    /// Constructor
    IOIGTL_API SImageNetworkReader();

    /// Destructor
    IOIGTL_API virtual ~SImageNetworkReader() noexcept;

    /// Overrides
    IOIGTL_API ::io::IOPathType getIOPathType() const;

    /**
     * @brief configure the server address to receive the image
     */
    IOIGTL_API virtual void configureWithIHM();

protected:
    /// Overrides
    IOIGTL_API virtual void configuring();

    /// Overrides
    IOIGTL_API virtual void starting();

    /// Overrides
    IOIGTL_API virtual void stopping();

    /**
     * @brief connect to the server, receive the image and close the connection
     */
    IOIGTL_API virtual void updating();

    /// Overrides
    IOIGTL_API virtual void swapping();

private:

    /// client instance
    ::igtlNetwork::Client m_client;

    /// hostname
    std::string m_hostname;

    /// port
    boost::uint16_t m_port;

};



} // namespace ioIGTL

#endif /*__IOIGTL_SIMAGENETWORKREADER_HPP__*/

