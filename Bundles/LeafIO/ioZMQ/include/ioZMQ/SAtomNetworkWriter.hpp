/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOZMQ_SATOMNETWORKWRITER_HPP__
#define __IOZMQ_SATOMNETWORKWRITER_HPP__

#include "ioZMQ/config.hpp"

#include <zmqNetwork/Socket.hpp>

#include <io/IWriter.hpp>

namespace ioZMQ
{

/**
 *
 * @brief writer of atom object using ZMQNetwork library
 */
class IOZMQ_CLASS_API SAtomNetworkWriter : public ::io::IWriter
{

public:

    fwCoreServiceClassDefinitionsMacro( (SAtomNetworkWriter) (::io::IWriter));

    /// Constructor
    IOZMQ_API SAtomNetworkWriter();

    /// Destructor
    IOZMQ_API virtual ~SAtomNetworkWriter() noexcept;

    /// Overrides
    IOZMQ_API ::io::IOPathType getIOPathType() const override;

    /**
     * @brief configure the host(in zeromq format) to send atom to a client
     *
     * @see http://zeromq.org/intro:read-the-manual
     */
    IOZMQ_API virtual void configureWithIHM() override;

protected:
    /// Overrides
    IOZMQ_API virtual void configuring() override;

    /// Overrides
    IOZMQ_API virtual void starting() override;

    /// Overrides
    IOZMQ_API virtual void stopping() override;

    /// Overrides
    IOZMQ_API virtual void updating() override;

    /// Overrides
    IOZMQ_API virtual void swapping() override;

private:

    /// Socket instance
    ::zmqNetwork::Socket::sptr m_socket;

    /// Host in zmq format
    std::string m_host;
};

} // namespace ioZMQ

#endif /*__IOZMQ_SATOMNETWORKWRITER_HPP__*/

