/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOIGTL_STDATALISTENER_HPP__
#define __IOIGTL_STDATALISTENER_HPP__

#include "ioIGTL/config.hpp"

#include <fwData/Object.hpp>

#include <fwThread/Worker.hpp>

#include <igtlNetwork/Client.hpp>

#include <ioNetwork/INetworkListener.hpp>

#include <cstdint>
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
 * @note : hostname and port of this service can be a value or a nameKey from preference settings
   (for example <server>%HOSTNAME%:%PORT%</server>)
 */
class IOIGTL_CLASS_API STDataListener : public ::ioNetwork::INetworkListener
{

public:

    fwCoreServiceClassDefinitionsMacro ( (STDataListener)(::ioNetwork::INetworkListener) );

    /// Constructor
    IOIGTL_API STDataListener();

    /// Destructor
    IOIGTL_API virtual ~STDataListener();

protected:

    /// Configures the service.
    IOIGTL_API virtual void configuring() throw ( ::fwTools::Failed );

    /**
     * @brief start the client and try to connect to the server specify in configuration
     */
    IOIGTL_API virtual void starting() throw ( ::fwTools::Failed );

    /**
     * @brief disconnect the client from the server
     */
    IOIGTL_API virtual void stopping() throw ( ::fwTools::Failed );

    /**
     * @brief method to set host and port of listener
     *
     * @see ioNetwork::INetworkListener
     *
     * @param[in] hostname hostname or ip of the server
     * @param[in] port port of the server
     */
    IOIGTL_API void setHost(std::string const& hostname, std::uint16_t const port) throw (::fwTools::Failed);

private:

    typedef std::map< std::string, unsigned long > MatrixNameIndexType;

    ///Push received matrices in timeline
    void manageTimeline(const SPTR(fwData::Composite)& obj);

    ///Helper to parse preference key
    std::string getPreferenceKey(const std::string& key) const;

    /**
     * @brief method contain a loop with receive and when we receive we emit m_sigReceiveObject
     *        this method run in a thread
     */
    void runClient() throw ( ::fwTools::Failed );

    /// listener thread for receiving data from client socket
    ::fwThread::Worker::sptr m_clientWorker;

    /// client socket
    ::igtlNetwork::Client m_client;

    /// hostname preference key
    std::string m_hostnameKey;

    /// port preference key
    std::string m_portKey;

    /// hostname
    std::string m_hostname;

    /// port
    std::uint16_t m_port;

    MatrixNameIndexType m_matrixNameIndex;
};

} // namespace ioIGTL

#endif /*__IOIGTL_STDATALISTENER_HPP__*/

