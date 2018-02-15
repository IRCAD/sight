/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioZMQ/config.hpp"

#include <fwIO/IWriter.hpp>

#include <zmqNetwork/Socket.hpp>

namespace ioZMQ
{
/**
 *
 * @brief writer of image using ZMQNetwork
 */
class IOZMQ_CLASS_API SImageNetworkWriter : public ::fwIO::IWriter
{

public:

    fwCoreServiceClassDefinitionsMacro( (SImageNetworkWriter) (::fwIO::IWriter));

    /// Constructor
    IOZMQ_API SImageNetworkWriter();

    /// Destructor
    IOZMQ_API virtual ~SImageNetworkWriter() noexcept;

    /// Overrides
    IOZMQ_API ::fwIO::IOPathType getIOPathType() const override;

    /**
     * @brief configure the host(in zeromq format) to send image to a client
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
