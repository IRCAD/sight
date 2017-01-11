/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOIGTL_SOPENIGTLINKLISTENER_HPP__
#define __IOIGTL_SOPENIGTLINKLISTENER_HPP__

#include "ioIGTL/config.hpp"

#include <fwData/Object.hpp>

#include <fwThread/Worker.hpp>

#include <igtlNetwork/Client.hpp>

#include <ioNetwork/INetworkListener.hpp>

#include <string>

namespace ioIGTL
{

/**
 * @brief class for network client service use OpenIGTLink
 *
 * @section XML XML Configuration
 * @code{.xml}
 * <service uid="..." type="::ioIGTL::SOpenIGTLinkListener" >
 *      <inout key="object" uid="..." />
 *      <server>127.0.0.1:4242</server>
 *      <deviceName>...</deviceName>
 *      <deviceName>...</deviceName>
 *      ...
 * </service>
 * @endcode
 * @subsection In-Out In-Out:
 * - \b object [::fwData::Object]:
 *   - if associated object is a timeline (arData::MatrixT or arData::FrameTL): received IGTL data are pushed in timeline
 *   - else : object is updated with received IGTL data
 * @subsection Configuration Configuration:
 * - \b deviceName(optional) : filter by device Name in Message, by default all messages will be processed
 * - \b server : server URL. Need hostname and port in this format addr:port (default value is 127.0.0.1:4242).
 * @note : hostname and port of this service can be a value or a nameKey from preference settings
 *  (for example <server>%HOSTNAME%:%PORT%</server>)
 */
class IOIGTL_CLASS_API SOpenIGTLinkListener : public ::ioNetwork::INetworkListener
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SOpenIGTLinkListener)(::ioNetwork::INetworkListener) );

    /// Constructor
    IOIGTL_API SOpenIGTLinkListener();

    /// Destructor
    IOIGTL_API virtual ~SOpenIGTLinkListener();

protected:
    /**
     * @brief configure method to configure the network client. Need hostname and port in this format addr:port
     *
     * @code{.xml}
     * <service uid="..." type="::ioIGTL::SOpenIGTLinkListener" >
     *      <server>127.0.0.1:4242</server>
     *      <deviceName>...</deviceName>
     *      <deviceName>...</deviceName>
     *      ...
     * </service>
     * @endcode
     *
     * - deviceName(optional) : filter by device Name in Message, by default all message will be processed
     *
     */
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
    IOIGTL_API void setHost(std::string const& hostname, ::boost::uint16_t const port) throw (::fwTools::Failed);

private:

    /**
     * @brief method contain a loop with receive and when we receive we emit m_sigReceiveObject
     *        this method run in a thread
     */
    void    runClient() throw ( ::fwTools::Failed );
    /**
     * @brief method called when the current object is a timeline
     * @note Currently only arData::MatrixTL and arData::FrameTL are managed
     */
    void manageTimeline(::fwData::Object::sptr obj);

    ///Helper to parse preference key
    std::string getPreferenceKey(const std::string& key) const;


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
    ::boost::uint16_t m_port;

    ///Type of timeline
    typedef enum TimelineType
    {
        NONE = -1,
        MATRIX,
        FRAME
    }TimelineType;

    TimelineType m_timelineType;

    bool m_frameTLInitialized;
};

} // namespace OpenIGTLinkIO

#endif /*__IOIGTL_SOPENIGTLINKLISTENER_HPP__*/

