/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOIGTL_SCLIENTSENDER_HPP__
#define __IOIGTL_SCLIENTSENDER_HPP__

#include "ioIGTL/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwThread/Worker.hpp>

#include <igtlNetwork/Client.hpp>

#include <ioNetwork/INetworkSender.hpp>

namespace ioIGTL
{

/**
 * @brief class of a client OpenIGTLink that will send objects to the connected server
 *
 * @section XML XML Configuration
 * @code{.xml}
 * <service uid="..." type="::ioIGTL::SClientSender">
 *      <server>127.0.0.1:4242</server>
 *      <deviceName>name1</deviceName>
 *      <deviceName>...</deviceName>
 *      <inout group="objects">
 *           <key uid="..." name="..." />
 *           <key uid="..." />
 *      </inout>
 * </service>
 * @endcode
 * @subsection Configuration Configuration:
 * - \b deviceName(optional) : filter by device Name in Message, by default all messages will be processed
 * - \b server : server URL. Need hostname and port in this format addr:port (default value is 127.0.0.1:4242).
 * @note : hostname and port of this service can be a value or a nameKey from preference settings
   (for example <server>%HOSTNAME%:%PORT%</server>)
 * @subsection In-Out In-Out:
 * - \b objects [::fwData::Object]: specified objects to send to the connected server. They can have an attribute
 * to change the devicename used for this specific data.

 */
class IOIGTL_CLASS_API SClientSender : public ::ioNetwork::INetworkSender
{

public:

    fwCoreServiceClassDefinitionsMacro( (SClientSender)(::ioNetwork::INetworkSender) );

    IOIGTL_API static const ::fwCom::Slots::SlotKeyType s_START_SENDING_SLOT;
    typedef ::fwCom::Slot< void () > StartSendingSlotType;

    IOIGTL_API static const ::fwCom::Slots::SlotKeyType s_STOP_SENDING_SLOT;
    typedef ::fwCom::Slot< void () > StopSendingSlotType;

    /// Constructor
    IOIGTL_API SClientSender() throw();

    /// Destructor
    IOIGTL_API virtual ~SClientSender() throw();

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

    /**
     * @brief change the port of the server use it before start
     */
    IOIGTL_API void setPort(boost::uint16_t const port) throw (::fwTools::Failed);

private:

    /**
     * @brief slot called when configuration of sender is updated
     *
     * @see ioNetwork::INetworkSender
     */
    void updateConfiguration(::boost::uint16_t const port);

    /// Starts the server
    IOIGTL_API void startSending();

    /// Stops the server
    IOIGTL_API void stopSending();

    /**
     * @brief method to send data.
     *
     * @param[in] obj obj to send
     */
    IOIGTL_API void sendObject(const ::fwData::Object::sptr& obj);

    /**
     * @brief method to send data.
     *
     * @param[in] obj obj to send
     * @param[in] index index of the object in the group
     */
    IOIGTL_API void sendObject(const ::fwData::Object::sptr& obj, const size_t index);

    ///Helper to parse preference key
    std::string getPreferenceKey(const std::string& key) const;

    /**
     * @brief method contain a loop with receive and when we receive we emit m_sigReceiveObject
     *        this method run in a thread
     */
    void runClient() throw ( ::fwTools::Failed );

    /// listener thread for receiving data from client socket
    ::fwThread::Worker::sptr m_clientWorker;

    ///Slot to start sending
    StartSendingSlotType::sptr m_startSendingSlot;

    ///Slot to stop sending
    StopSendingSlotType::sptr m_stopSendingSlot;

    /// client socket
    ::igtlNetwork::Client m_client;

    /// hostname preference key
    std::string m_hostnameKey;

    /// port preference key
    std::string m_portKey;

    /// hostname
    std::string m_hostname;

    /// device name
    std::string m_deviceName;

    /// port
    std::uint16_t m_port;

    /// Vector of device name used if there are specified names in the group configuration.
    std::vector< std::string > m_deviceNames;

};

} // namespace ioIGTL

#endif /*__IOIGTL_SCLIENTSENDER_HPP__*/
