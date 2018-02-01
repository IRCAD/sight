/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioIGTL/config.hpp"

#include <fwData/Object.hpp>

#include <igtlNetwork/Client.hpp>

#include <ioNetwork/INetworkListener.hpp>

#include <cstdint>
#include <future>
#include <map>
#include <string>

namespace fwData
{
class Composite;
}

namespace ioIGTL
{

/**
 * @brief class for network client service use OpenIGTLink
 *
 * @section XML XML Configuration
 * @code{.xml}
 * <service type="::ioIGTL::STDataListener">
 *      <inout key="timeline" uid="..." />
 *      <server>127.0.0.1:4242</server>
 *      <deviceName>Name1</deviceName>
 *      <deviceName>...</deviceName>
 *      <TData>
 *          <matrix name="matrix_name" index="0" />
 *          <matrix name="..." index="..." />
 *      </TData>
 *      ...
 * </service>
 * @endcode
 * @subsection In-Out In-Out:
 * - \b timeline [::arData::MatrixTL]: Timeline used to store received matrix.
 * @subsection Configuration Configuration:
 * - \b deviceName(optional) : filter by device Name in Message, by default all messages will be processed
 * - \b TData : specified IGTL matrix name to push in specified timeline index
 * - \b server : server URL. Need hostname and port in this format addr:port (default value is 127.0.0.1:4242).
 * @note : hostname, port and deviceName of this service can be a value or a nameKey from preference settings
   (for example <server>%HOSTNAME%:%PORT%</server>, <deviceName>%DEVICE_NAME%</deviceName>)
 */
class IOIGTL_CLASS_API STDataListener : public ::ioNetwork::INetworkListener
{

public:

    fwCoreServiceClassDefinitionsMacro( (STDataListener)(::ioNetwork::INetworkListener) );

    /// Constructor
    IOIGTL_API STDataListener();

    /// Destructor
    IOIGTL_API virtual ~STDataListener();

protected:

    /// Configure port, hostname and device name
    IOIGTL_API virtual void configuring() override;

    /// Start the client and try to connect to the server specify in configuration
    IOIGTL_API virtual void starting() override;

    /// Disconnect the client from the server
    IOIGTL_API virtual void stopping() override;

private:

    typedef std::map< std::string, unsigned long > MatrixNameIndexType;

    ///Push received matrices in timeline
    void manageTimeline(const SPTR(fwData::Composite)& obj);

    /**
     * @brief method contain a loop with receive and when we receive we emit m_sigReceiveObject
     *        this method run in a thread
     */
    void runClient();

    /// Future used to wait for the client
    std::future<void> m_clientFuture;

    /// client socket
    ::igtlNetwork::Client m_client;

    /// hostname config key
    std::string m_hostnameConfig;

    /// port config key
    std::string m_portConfig;

    /// device names key
    std::vector < ::fwRuntime::ConfigurationElement::sptr > m_deviceNamesConfig;

    MatrixNameIndexType m_matrixNameIndex;
};

} // namespace ioIGTL
