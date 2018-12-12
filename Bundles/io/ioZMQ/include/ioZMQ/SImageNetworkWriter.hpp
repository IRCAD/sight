/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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
