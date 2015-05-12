/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2004-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_HELPER_SIGSLOTCONNECTION_HPP__
#define __FWSERVICES_HELPER_SIGSLOTCONNECTION_HPP__

#include <list>
#include <utility>
#include <vector>

#include <boost/make_shared.hpp>

#include <fwCore/BaseObject.hpp>

#include <fwCom/Connection.hpp>
#include <fwCom/HasSlots.hpp>
#include <fwCom/HasSignals.hpp>

#include "fwServices/config.hpp"

namespace fwServices
{
namespace helper
{

/**
 * @brief  This class provides few tools to connect/disconnect signals/slots between obj/srv
 *
 * @class  SigSlotConnection
 *
 * @date   2012.
 */
class FWSERVICES_CLASS_API SigSlotConnection : public ::fwCore::BaseObject
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (SigSlotConnection)(::fwCore::BaseObject),
                                            (()),
                                            ::boost::make_shared< SigSlotConnection > );

    typedef std::list< ::fwCom::Connection > ConnectionContainerType;

    typedef std::pair< ::fwCom::Signals::SignalKeyType, ::fwCom::Slots::SlotKeyType > KeyConnectionType;

    typedef std::vector< KeyConnectionType > KeyConnectionsType;

    /// Constructor, does nothing
    FWSERVICES_API SigSlotConnection();

    /// Destructor, calls disconnect()
    FWSERVICES_API virtual ~SigSlotConnection();

    /// Connect signal to slot, and register this new connection in  m_connections
    FWSERVICES_API void connect( ::fwCom::HasSignals::sptr hasSignals, ::fwCom::Signals::SignalKeyType signalKey,
                                 ::fwCom::HasSlots::sptr hasSlots, ::fwCom::Slots::SlotKeyType slotKey );

    /// Connect signals to slots, and register these new connections in  m_connections
    FWSERVICES_API void connect( ::fwCom::HasSignals::sptr hasSignals, ::fwCom::HasSlots::sptr hasSlots,
                                 const KeyConnectionsType & keyConnections );

    /// Disconnect all registered connections and clear m_connections
    FWSERVICES_API void disconnect();

protected:

    /// Connections storage
    ConnectionContainerType m_connections;
};

} // end namespace helper
} // end namespace fwServices

#endif // __FWSERVICES_HELPER_SIGSLOTCONNECTION_HPP__
