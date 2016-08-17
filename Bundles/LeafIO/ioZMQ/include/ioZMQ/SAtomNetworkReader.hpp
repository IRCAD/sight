/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOZMQ_SATOMNETWORKREADER_HPP__
#define __IOZMQ_SATOMNETWORKREADER_HPP__

#include "ioZMQ/config.hpp"

#include <io/IReader.hpp>
#include <zmqNetwork/Socket.hpp>

namespace ioZMQ
{
/**
 *
 * @brief Reader of atom using ZMQNetwork library
 */
class IOZMQ_CLASS_API SAtomNetworkReader : public ::io::IReader
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SAtomNetworkReader) (::io::IReader));

    /// Constructor
    IOZMQ_API SAtomNetworkReader();

    /// Destructor
    IOZMQ_API virtual ~SAtomNetworkReader() throw();

    /**
     * @brief Show a input message box with you can enter host in zeromq format
     *
     * @see http://zeromq.org/intro:read-the-manual
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

    /**
     * @brief receive atom from an another program
     */
    IOZMQ_API virtual void updating() throw (::fwTools::Failed);

    /// Overrides
    IOZMQ_API virtual void swapping() throw (::fwTools::Failed);

private:
    /// Socket to receive image data
    ::zmqNetwork::Socket::sptr m_socket;

    /// hostname in zmq format
    std::string m_host;

};



} // namespace ioZMQ

#endif /*__IOZMQ_SATOMNETWORKREADER_HPP__*/

