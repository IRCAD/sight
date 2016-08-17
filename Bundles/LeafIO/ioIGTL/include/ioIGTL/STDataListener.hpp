/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOIGTL_STDATALISTENER_HPP__
#define __IOIGTL_STDATALISTENER_HPP__

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
#include <map>
#include <cstdint>


namespace fwData
{
class Composite;
}

namespace ioIGTL
{

/**
 * @brief class for network client service use OpenIGTLink
 */
class IOIGTL_CLASS_API STDataListener : public ::ioNetwork::INetworkListener
{

public:

    fwCoreServiceClassDefinitionsMacro ( (STDataListener)(::ioNetwork::INetworkListener) );

    IOIGTL_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CONFIGURATION_SLOT;
    typedef ::fwCom::Slot< void (std::string const, ::boost::uint16_t const) > UpdateConfigurationSlotType;

    /// Constructor
    IOIGTL_API STDataListener();

    /// Destructor
    IOIGTL_API virtual ~STDataListener();

protected:
    /**
     * @brief configure method to configure the network client. Need hostname and port in this format addr:port
     *
     * @code{.xml}
     * <service type="::ioNetwork::INetworkListener" impl="::ioIGTL::STDataListener" uid="networkListener" autoConnect="no">
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
    IOIGTL_API void updateConfiguration(std::string const hostname, std::uint16_t const port);

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

    /**
     * @brief method contain a loop with receive and when we receive we emit m_sigReceiveObject
     *        this method run in a thread
     */
    void runClient() throw ( ::fwTools::Failed );

    /// listener thread for receiving data from client socket
    ::fwThread::Worker::sptr m_clientWorker;

    /// client socket
    ::igtlNetwork::Client m_client;

    /// hostname
    std::string m_hostname;

    /// port
    std::uint16_t m_port;

    /// update configuration slot
    UpdateConfigurationSlotType::sptr m_updateConfigurationSlot;

    MatrixNameIndexType m_matrixNameIndex;


};



} // namespace ioIGTL

#endif /*__IOIGTL_STDATALISTENER_HPP__*/

