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
    IOIGTL_API virtual ~SImageNetworkReader() throw();

    /// Overrides
    IOIGTL_API ::io::IOPathType getIOPathType() const;

    /**
     * @brief configure the server address to receive the image
     */
    IOIGTL_API virtual void configureWithIHM() throw (::fwTools::Failed);

protected:
    /// Overrides
    IOIGTL_API virtual void configuring() throw (::fwTools::Failed);

    /// Overrides
    IOIGTL_API virtual void starting() throw (::fwTools::Failed);

    /// Overrides
    IOIGTL_API virtual void stopping() throw (::fwTools::Failed);

    /**
     * @brief connect to the server, receive the image and close the connection
     */
    IOIGTL_API virtual void updating() throw (::fwTools::Failed);

    /// Overrides
    IOIGTL_API virtual void swapping() throw (::fwTools::Failed);

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

