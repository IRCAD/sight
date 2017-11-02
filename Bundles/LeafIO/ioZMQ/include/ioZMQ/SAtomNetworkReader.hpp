/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOZMQ_SATOMNETWORKREADER_HPP__
#define __IOZMQ_SATOMNETWORKREADER_HPP__

#include "ioZMQ/config.hpp"

#include <zmqNetwork/Socket.hpp>

#include <io/IReader.hpp>

namespace ioZMQ
{
/**
 *
 * @brief Reader of atom using ZMQNetwork library
 */
class IOZMQ_CLASS_API SAtomNetworkReader : public ::io::IReader
{

public:

    fwCoreServiceClassDefinitionsMacro( (SAtomNetworkReader) (::io::IReader));

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
    IOZMQ_API ::io::IOPathType getIOPathType() const override;

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

#endif /*__IOZMQ_SATOMNETWORKREADER_HPP__*/

