/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOZMQ_SIMAGENETWORKREADER_HPP__
#define __IOZMQ_SIMAGENETWORKREADER_HPP__

#include "ioZMQ/config.hpp"

#include <io/IReader.hpp>
#include <zmqNetwork/Socket.hpp>

namespace ioZMQ
{
/**
 *
 * @brief image reader using ZMQNetwork library
 */
class IOZMQ_CLASS_API SImageNetworkReader : public ::io::IReader
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SImageNetworkReader) (::io::IReader));

    /// Constructor
    IOZMQ_API SImageNetworkReader();

    /// Destructor
    IOZMQ_API virtual ~SImageNetworkReader() throw();

    /**
     * @brief Show a input dialog box which you can enter a host in zeromq format
     */
    IOZMQ_API virtual void configureWithIHM();

    /// Overrides
    IOZMQ_API ::io::IOPathType getIOPathType() const;

protected:

    /// Overrides
    IOZMQ_API virtual void configuring() throw (::fwTools::Failed);

    /// Overrides
    IOZMQ_API virtual void starting() throw (::fwTools::Failed);

    /// Overrides
    IOZMQ_API virtual void stopping() throw (::fwTools::Failed);

    /// Overrides
    IOZMQ_API virtual void updating() throw (::fwTools::Failed);


    /// Overrides
    IOZMQ_API virtual void swapping() throw (::fwTools::Failed);
private:

    /// Socket to receive image data
    ::zmqNetwork::Socket::sptr m_socket;

    /// hostname in format zmq
    std::string m_host;


};

} // namespace ioZMQ

#endif /*__IOZMQ_SIMAGENETWORKREADER_HPP__*/

