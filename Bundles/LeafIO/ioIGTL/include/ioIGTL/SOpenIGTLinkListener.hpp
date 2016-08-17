/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOIGTL_SOPENIGTLINKLISTENER_HPP__
#define __IOIGTL_SOPENIGTLINKLISTENER_HPP__

#include "ioIGTL/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwData/Object.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwThread/Worker.hpp>
#include <igtlNetwork/Client.hpp>
#include <ioNetwork/INetworkListener.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

#include <string>

namespace ioIGTL
{

/**
 * @brief class for network client service use OpenIGTLink
 */
class IOIGTL_CLASS_API SOpenIGTLinkListener : public ::ioNetwork::INetworkListener
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SOpenIGTLinkListener)(::ioNetwork::INetworkListener) );

    IOIGTL_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CONFIGURATION_SLOT;
    typedef ::fwCom::Slot< void (std::string const, ::boost::uint16_t const) > UpdateConfigurationSlotType;

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
     * @brief slot to call when configuration is updated
     *
     * @see ioNetwork::INetworkListener
     *
     * @param[in] hostname hostname or ip of the server
     * @param[in] port port of the server
     */
    IOIGTL_API void updateConfiguration(std::string const hostname, boost::uint16_t const port);

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

    /// listener thread for receiving data from client socket
    ::fwThread::Worker::sptr m_clientWorker;

    /// client socket
    ::igtlNetwork::Client m_client;

    /// hostname
    std::string m_hostname;

    /// port
    ::boost::uint16_t m_port;

    /// update configuration slot
    UpdateConfigurationSlotType::sptr m_updateConfigurationSlot;

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

