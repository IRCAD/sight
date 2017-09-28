/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IGTLNETWORK_CLIENT_HPP__
#define __IGTLNETWORK_CLIENT_HPP__

#include "igtlNetwork/config.hpp"
#include "igtlNetwork/INetwork.hpp"

#include <fwCore/Exception.hpp>

#include <fwData/Object.hpp>

#include <igtlProtocol/DataConverter.hpp>

#include <boost/type.hpp>

#include <igtl/igtlClientSocket.h>
#include <igtl/igtlMessageHeader.h>

#include <string>

namespace igtlNetwork
{

/**
 *
 * @brief A Network client using igtl::ClientSocket can send fwData primitive
 */
class IGTLNETWORK_CLASS_API Client : public ::igtlNetwork::INetwork
{
public:
    typedef SPTR (Client) sptr;

    /**
     * @brief default constructor
     */
    IGTLNETWORK_API Client();

    /**
     * @brief Constructor take a igtl::ClientSocket used only by server class.
     *        You should not use this constructor, use default constructor instead
     */
    IGTLNETWORK_API Client (::igtl::ClientSocket::Pointer socket);

    /**
     * @brief Destructor if a connection is opened the destructor close it
     */
    IGTLNETWORK_API ~Client();

    /**
     * @brief method to connect client to a server by using hostname and port
     *
     * @param[in] addr ip or hostname
     * @param[in] port port of server
     */
    IGTLNETWORK_API void connect (const std::string& addr,
                                  std::uint16_t port);

    /**
     * @brief method to disconnect client
     */
    IGTLNETWORK_API void disconnect();

    /**
     * @brief method to check if the client is connected
     *
     * @return a boolean to say if the client is connected or not
     */
    IGTLNETWORK_API bool isConnected() const;

private:

    /**
     * @brief utility method to avoid code duplication
     * @param[in] msg exception message
     * @param[in] result throw the exception if result=true
     */
    void throwExceptionIfFailed (const std::string& msg, bool result);

};

} // namespace igtlNetwork

#endif // __IGTLNETWORK_CLIENT_HPP__

