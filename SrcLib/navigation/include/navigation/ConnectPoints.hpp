/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __NAVIGATION_CONNECTPOINTS_HPP__
#define __NAVIGATION_CONNECTPOINTS_HPP__

#include "navigation/config.hpp"

#include <fwCore/BaseObject.hpp>

#include <fwCom/Connection.hpp>

#include <map>
#include <utility>
#include <vector>

#include <fwCom/HasSignals.hpp>
#include <fwCom/HasSlots.hpp>

namespace fwData
{
class PointList;
}

namespace navigation
{

/**
 * @brief Helps to connect spline point signals to slots.
 */
class NAVIGATION_CLASS_API ConnectPoints : public ::fwCore::BaseObject
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (ConnectPoints)(::fwCore::BaseObject),
                                            (()),
                                            std::make_shared< ConnectPoints > );

    /**
     * @name Typedefs
     * @{ */
    typedef std::map< ::fwCom::HasSignals::sptr, ::fwCom::Connection >                 ConnectionContainerType;
    typedef std::pair< ::fwCom::Signals::SignalKeyType, ::fwCom::Slots::SlotKeyType >   KeyConnectionType;
    typedef std::vector< KeyConnectionType >                                            KeyConnectionsType;
    /**  @} */

    /// Constructor, does nothing
    NAVIGATION_API ConnectPoints();

    /// Destructor, calls disconnectSplinePoints()
    NAVIGATION_API virtual ~ConnectPoints();

    /// Connect signal to slot, and register this new connection in  m_connections
    NAVIGATION_API void connectPointToService(
        const SPTR(::fwCom::HasSignals)& hasSignals,
        const ::fwCom::Signals::SignalKeyType& signalKey,
        const SPTR(::fwCom::HasSlots)& hasSlots,
        const ::fwCom::Slots::SlotKeyType& slotKey);

    /// Connect signal to slot, and register this new connection in  m_connections
    NAVIGATION_API void disconnectPointToService(const SPTR(::fwCom::HasSignals)& hasSignals);

    /// Connect signal to slot, and register this new connection in  m_connections
    NAVIGATION_API void connectAllSplinePoints(
        const SPTR(::fwData::PointList)& pointList,
        const SPTR(::fwCom::HasSlots)& hasSlots,
        const ::fwCom::Slots::SlotKeyType& slotKey );

    /// Connect signal to slot, and register this new connection in  m_connections
    NAVIGATION_API void disconnectSplinePoints();

private:

    /// Connections storage
    ConnectionContainerType m_connections;

};

}// namespace navigation

#endif // __NAVIGATION_CONNECTPOINTS_HPP__
