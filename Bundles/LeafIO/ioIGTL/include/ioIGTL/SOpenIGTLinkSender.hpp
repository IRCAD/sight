/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOIGTL_SOPENIGTLINKSENDER_HPP__
#define __IOIGTL_SOPENIGTLINKSENDER_HPP__

#include "ioIGTL/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Object.hpp>

#include <fwTools/Failed.hpp>

#include <igtlNetwork/Client.hpp>
#include <igtlNetwork/Server.hpp>

#include <ioNetwork/INetworkSender.hpp>

#include <cstdint>
#include <future>
#include <string>

namespace ioIGTL
{

/**
 * @brief    Network server that uses OpenIGTLink protocol
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service uid="..." type="::ioIGTL::SOpenIGTLinkSender">
            <in key="object" uid="..." />
            <deviceName>...</deviceName>
       </service>
   @endcode
 * @subsection Input Input:
 * - \b target [::fwData::Object]: .
 * @subsection Configuration Configuration:
 * - \b deviceName(optional) : device to set in the message.
 */

class IOIGTL_CLASS_API SOpenIGTLinkSender : public ::ioNetwork::INetworkSender
{

public:

    fwCoreServiceClassDefinitionsMacro( (SOpenIGTLinkSender)( ::ioNetwork::INetworkSender ) );

    /// Constructor
    IOIGTL_API SOpenIGTLinkSender();

    /// Destructor
    IOIGTL_API virtual ~SOpenIGTLinkSender();

protected:

    /// Configure port and device name
    IOIGTL_API virtual void configuring();

    /// Start the server.
    IOIGTL_API virtual void starting();

    /// Stop the server
    IOIGTL_API virtual void stopping();

private:
    /**
     * @brief method to send data. Linked to m_slotSendData
     *
     * @param[in] obj obj to send
     */
    IOIGTL_API void sendObject(const ::fwData::Object::sptr& obj);

    /// Server instance
    ::igtlNetwork::Server::sptr m_server;

    /// Future used to wait for the server
    std::future<void> m_serverFuture;

    /// port preference key
    std::string m_portConfig;

    ///device name
    std::string m_deviceName;
};

} // namespace ioIGTL

#endif /*__IOIGTL_SOPENIGTLINKSENDER_HPP__*/
