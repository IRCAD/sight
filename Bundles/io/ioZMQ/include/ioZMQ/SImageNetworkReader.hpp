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
