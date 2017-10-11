/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOIGTL_SSERVERLISTENER_HPP__
#define __IOIGTL_SSERVERLISTENER_HPP__

#include "ioIGTL/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Object.hpp>

#include <fwTools/Failed.hpp>

#include <igtlNetwork/Client.hpp>
#include <igtlNetwork/Server.hpp>

#include <ioNetwork/INetworkListener.hpp>

#include <cstdint>
#include <future>
#include <string>

namespace ioIGTL
{

/**
 * @brief OpenIGTLink server that will listen objects from the connected clients
 *
 * @section XML XML Configuration
 * @code{.xml}
 * <service uid="..." type="::ioIGTL::SServerListener" autoConnect="yes" >
 *      <port>...</port>
 *      <inout group="objects">
 *           <key uid="..." deviceName="device01" />
 *           <key uid="..." deviceName="device02" />
 *      </in>
 * </service>
 * @endcode
 * @subsection Configuration Configuration:
 * - \b port : defines the port where the objects will be listen
 * @subsection Input Input:
 * - \b objects [::fwData::Object]: specified objects to listen.
 * They must have an attribute 'deviceName' to know the device-name used for this specific data.
 **/
class IOIGTL_CLASS_API SServerListener : public ::ioNetwork::INetworkListener
{

public:

    fwCoreServiceClassDefinitionsMacro( (SServerListener)( ::ioNetwork::INetworkListener ) );

    /// Constructor
    IOIGTL_API SServerListener();

    /// Destructor
    IOIGTL_API virtual ~SServerListener();

protected:

    /// Configure port and device name
    IOIGTL_API virtual void configuring() override;

    /// Start the server.
    IOIGTL_API virtual void starting() override;

    /// Stop the server
    IOIGTL_API virtual void stopping() override;

private:

    /**
     * @brief method contain the receive loop
     * this method run in a thread
     */
    void receiveObject();

    /// Server instance
    ::igtlNetwork::Server::sptr m_server;

    /// Future used to wait for the server
    std::future<void> m_serverFuture;

    /// Future used to wait for message
    std::future<void> m_receiveFuture;

    /// port preference key
    std::string m_portConfig;

    /// Vector of device name if the group configuration exists.
    std::vector< std::string > m_deviceNames;

};

} // namespace ioIGTL

#endif /*__IOIGTL_SSERVERLISTENER_HPP__*/
