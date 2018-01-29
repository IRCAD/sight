/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioZMQ/config.hpp"

#include <fwIO/IReader.hpp>

#include <zmqNetwork/Socket.hpp>

namespace ioZMQ
{
/**
 *
 * @brief image reader using ZMQNetwork library
 */
class IOZMQ_CLASS_API SImageNetworkReader : public ::fwIO::IReader
{

public:

    fwCoreServiceClassDefinitionsMacro( (SImageNetworkReader) (::fwIO::IReader));

    /// Constructor
    IOZMQ_API SImageNetworkReader();

    /// Destructor
    IOZMQ_API virtual ~SImageNetworkReader() noexcept;

    /**
     * @brief Show a input dialog box which you can enter a host in zeromq format
     */
    IOZMQ_API virtual void configureWithIHM() override;

    /// Overrides
    IOZMQ_API ::fwIO::IOPathType getIOPathType() const override;

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

    /// Socket to receive image data
    ::zmqNetwork::Socket::sptr m_socket;

    /// hostname in format zmq
    std::string m_host;

};

} // namespace ioZMQ
