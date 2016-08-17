/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOIGTL_SIMAGENETWORKWRITER_HPP__
#define __IOIGTL_SIMAGENETWORKWRITER_HPP__

#include "ioIGTL/config.hpp"

#include <io/IWriter.hpp>

#include <igtlNetwork/Server.hpp>

#include <boost/type.hpp>

namespace ioIGTL
{
/**
 *
 * @brief image network writer using OpenIGTLinkNetwork
 */
class IOIGTL_CLASS_API SImageNetworkWriter : public ::io::IWriter
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SImageNetworkWriter) (::io::IWriter));


    /// Constructor
    IOIGTL_API SImageNetworkWriter();

    /// Destructor
    IOIGTL_API virtual ~SImageNetworkWriter() throw();

    /// Overrides
    IOIGTL_API ::io::IOPathType getIOPathType() const;
    /**
     * @brief configure the port to listen for send image to a client
     */
    IOIGTL_API virtual void configureWithIHM();

protected:

    /// Overrides
    IOIGTL_API virtual void configuring() throw (::fwTools::Failed);

    /// Overrides
    IOIGTL_API virtual void starting() throw (::fwTools::Failed);

    /// Overrides
    IOIGTL_API virtual void stopping() throw (::fwTools::Failed);

    /**
     * @brief when a client is connected it send image and close the connection
     */
    IOIGTL_API virtual void updating() throw (::fwTools::Failed);

    /// Overrides
    IOIGTL_API virtual void swapping() throw (::fwTools::Failed);

private:

    /// Server instannce
    ::igtlNetwork::Server m_server;

    /// port to listen
    ::boost::uint16_t m_port;

    /// booelan to check if the port is valid when updating is called
    bool m_isValidPort;

};



} // namespace ioIGTL

#endif /*__IOIGTL_SIMAGENETWORKWRITER_HPP__*/

