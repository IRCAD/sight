/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOIGTL_SCLIENTLISTENER_HPP__
#define __IOIGTL_SCLIENTLISTENER_HPP__

#include "ioIGTL/config.hpp"

#include <fwData/Object.hpp>

#include <igtlNetwork/Client.hpp>

#include <ioNetwork/INetworkListener.hpp>

#include <future>
#include <string>

namespace ioIGTL
{

/**
 * @brief   Network client that uses OpenIGTLink protocol
 *
 * @section XML XML Configuration
 * @code{.xml}
 * <service uid="..." type="::ioIGTL::SClientListener" >
 *      <server>127.0.0.1:4242</server>
 *      <inout group="objects">
 *          <key uid="..." name="device01" />
 *          <key uid="..." name="device02" />
 *      </inout>
 * </service>
 * @endcode
 * @subsection In-Out In-Out:
 * - \b object [::fwData::Object]:
 *   - if associated object is a timeline (arData::MatrixT or arData::FrameTL): received IGTL data are pushed in
 * timeline
 *   - else : object is updated with received IGTL data
 * @subsection Configuration Configuration:
 * - \b deviceName(optional) : filter by device Name in Message, by default all messages will be processed
 * - \b server : server URL. Need hostname and port in this format addr:port (default value is 127.0.0.1:4242).
 * @note : hostname and port of this service can be a value or a nameKey from preference settings
 *  (for example <server>%HOSTNAME%:%PORT%</server>)
 */
class IOIGTL_CLASS_API SClientListener : public ::ioNetwork::INetworkListener
{

public:

    fwCoreServiceClassDefinitionsMacro( (SClientListener)(::ioNetwork::INetworkListener) );

    /// Constructor
    IOIGTL_API SClientListener();

    /// Destructor
    IOIGTL_API virtual ~SClientListener();

protected:

    /// Configure port, hostname and device name
    IOIGTL_API virtual void configuring() override;

    /// start the client and try to connect to the server specify in configuration
    IOIGTL_API virtual void starting() override;

    /// disconnect the client from the server
    IOIGTL_API virtual void stopping() override;

private:

    /**
     * @brief method contain a loop with receive and when we receive we emit m_sigReceiveObject
     *        this method run in a thread
     */
    void    runClient();

    /**
     * @brief method called when the current object is a timeline
     * @note Currently only arData::MatrixTL and arData::FrameTL are managed
     */
    void manageTimeline(::fwData::Object::sptr obj, size_t index);

    /// client socket
    ::igtlNetwork::Client m_client;

    /// Future used to wait for the client
    std::future<void> m_clientFuture;

    /// hostname config key
    std::string m_hostnameConfig;

    /// port config key
    std::string m_portConfig;

    bool m_tlInitialized;

    /// map to associate device-name and object uid
    std::vector<std::string> m_deviceNames;
};

} // namespace ioIGTL

#endif /*__IOIGTL_SCLIENTLISTENER_HPP__*/

