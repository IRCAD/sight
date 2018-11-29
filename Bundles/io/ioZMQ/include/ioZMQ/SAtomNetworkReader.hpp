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
 * @brief Reader of atom using ZMQNetwork library
 */
class IOZMQ_CLASS_API SAtomNetworkReader : public ::fwIO::IReader
{

public:

    fwCoreServiceClassDefinitionsMacro( (SAtomNetworkReader) (::fwIO::IReader));

    /// Constructor
    IOZMQ_API SAtomNetworkReader();

    /// Destructor
    IOZMQ_API virtual ~SAtomNetworkReader() noexcept;

    /**
     * @brief Show a input message box with you can enter host in zeromq format
     *
     * @see http://zeromq.org/intro:read-the-manual
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

    /**
     * @brief receive atom from an another program
     */
    IOZMQ_API virtual void updating() override;

    /// Overrides
    IOZMQ_API virtual void swapping() override;

private:
    /// Socket to receive image data
    ::zmqNetwork::Socket::sptr m_socket;

    /// hostname in zmq format
    std::string m_host;

};

} // namespace ioZMQ
