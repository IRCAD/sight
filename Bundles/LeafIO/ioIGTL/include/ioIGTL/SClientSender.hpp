/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOIGTL_SCLIENTSENDER_HPP__
#define __IOIGTL_SCLIENTSENDER_HPP__

#include "ioIGTL/config.hpp"

#include <igtlNetwork/Client.hpp>

#include <ioNetwork/INetworkSender.hpp>

namespace ioIGTL
{

/**
 * @brief OpenIGTLink client that will send objects to the connected server
 *
 * @section XML XML Configuration
 * @code{.xml}
 * <service uid="..." type="::ioIGTL::SClientSender" autoConnect="yes">
 *      <server>127.0.0.1:4242</server>
 *      <in group="objects">
 *           <key uid="..." deviceName="device01" />
 *           <key uid="..." deviceName="device02" />
 *      </in>
 * </service>
 * @endcode
 * @subsection Input Input:
 * - \b objects [::fwData::Object]: specified objects to send to the connected server.
 * They must have an attribute 'deviceName' to known the device-name used for this specific data.
 * @subsection Configuration Configuration:
 * - \b deviceName : filter by device Name in Message
 * - \b server : server URL. Need hostname and port in this format addr:port (default value is 127.0.0.1:4242).
 * @note : hostname and port of this service can be a value or a nameKey from preference settings
   (for example <server>%HOSTNAME%:%PORT%</server>)
 */
class IOIGTL_CLASS_API SClientSender : public ::ioNetwork::INetworkSender
{

public:

    fwCoreServiceClassDefinitionsMacro( (SClientSender)(::ioNetwork::INetworkSender) );

    IOIGTL_API static const ::fwCom::Slots::SlotKeyType s_START_SENDING_SLOT;

    IOIGTL_API static const ::fwCom::Slots::SlotKeyType s_STOP_SENDING_SLOT;

    /// Constructor
    IOIGTL_API SClientSender();

    /// Destructor
    IOIGTL_API virtual ~SClientSender();

protected:

    /// Configures the service.
    IOIGTL_API virtual void configuring() override;

    /**
     * @brief start the client and try to connect to the server specify in configuration
     */
    IOIGTL_API virtual void starting() override;

    /**
     * @brief disconnect the client from the server
     */
    IOIGTL_API virtual void stopping() override;

private:

    /**
     * @brief method to send data.
     *
     * @param[in] obj obj to send
     * @param[in] index index of the object in the group
     */
    void sendObject(const ::fwData::Object::csptr& obj, const size_t index) override;

    /// client socket
    ::igtlNetwork::Client m_client;

    /// hostname config key
    std::string m_hostnameConfig;

    /// port config key
    std::string m_portConfig;

    /// Vector of device name used
    std::vector< std::string > m_deviceNames;
};

} // namespace ioIGTL

#endif /*__IOIGTL_SCLIENTSENDER_HPP__*/
