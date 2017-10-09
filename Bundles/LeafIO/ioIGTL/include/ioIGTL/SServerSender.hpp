/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOIGTL_SSERVERSENDER_HPP__
#define __IOIGTL_SSERVERSENDER_HPP__

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
 * <service uid="..." type="::ioIGTL::SServerSender" autoConnect="yes" >
 *      <port>...</port>
 *      <deviceName>...</deviceName>
 *      <in group="objects">
 *           <key uid="..." name="..." />
 *           <key uid="..." />
 *      </in>
 * </service>
 * @endcode
 * @subsection Configuration Configuration:
 * - \b port : defines the port where the objects will be sent
 * - \b deviceName : defines the name of the device (here, our server)
 * @subsection Input Input:
 * - \b objects [::fwData::Object] : defines the objects to send.
 **/

class IOIGTL_CLASS_API SServerSender : public ::ioNetwork::INetworkSender
{

public:

    fwCoreServiceClassDefinitionsMacro( (SServerSender)( ::ioNetwork::INetworkSender ) );

    /// Constructor
    IOIGTL_API SServerSender();

    /// Destructor
    IOIGTL_API virtual ~SServerSender();

protected:

    /// Configure port and device name
    IOIGTL_API virtual void configuring() override;

    /// Start the server.
    IOIGTL_API virtual void starting() override;

    /// Stop the server
    IOIGTL_API virtual void stopping() override;

private:

    /**
     * @brief method to send data.
     *
     * @param[in] obj obj to send
     * @param[in] index index of the object in the group
     */
    void sendObject(const ::fwData::Object::csptr& obj, const size_t index);

    /// Server instance
    ::igtlNetwork::Server::sptr m_server;

    /// Future used to wait for the server
    std::future<void> m_serverFuture;

    /// port preference key
    std::string m_portConfig;

    ///device name
    std::string m_deviceName;

    /// Vector of device name if the group configuration exists.
    std::vector< std::string > m_deviceNames;

};

} // namespace ioIGTL

#endif /*__IOIGTL_SSERVERSENDER_HPP__*/
