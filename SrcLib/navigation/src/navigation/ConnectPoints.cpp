/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "navigation/ConnectPoints.hpp"

#include <fwData/Object.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>

namespace navigation
{

ConnectPoints::ConnectPoints()
{
}

//------------------------------------------------------------------------------

ConnectPoints::~ConnectPoints()
{
    this->disconnectSplinePoints();
}

//------------------------------------------------------------------------------

void ConnectPoints::connectPointToService(
    const ::fwCom::HasSignals::sptr& hasSignals,
    const ::fwCom::Signals::SignalKeyType& signalKey,
    const ::fwCom::HasSlots::sptr& hasSlots,
    const ::fwCom::Slots::SlotKeyType& slotKey )
{
    ::fwCom::Connection connection;

    connection = hasSignals->signal( signalKey )->connect(hasSlots->slot(slotKey));
    m_connections.insert(std::make_pair(hasSignals, connection));
}

//------------------------------------------------------------------------------

void ConnectPoints::disconnectPointToService(const ::fwCom::HasSignals::sptr& hasSignals)
{
    ConnectionContainerType::iterator it = m_connections.find(hasSignals);
    OSLM_ASSERT("Failed to retrieve signal holder in connections container", it != m_connections.end());

    it->second.disconnect();
    m_connections.erase(m_connections.find (hasSignals));
}

//------------------------------------------------------------------------------

void ConnectPoints::connectAllSplinePoints(
    const ::fwData::PointList::sptr& pointList,
    const ::fwCom::HasSlots::sptr& hasSlots,
    const ::fwCom::Slots::SlotKeyType& slotKey )
{
    for(::fwData::Point::sptr point :  pointList->getRefPoints())
    {
        this->connectPointToService(point, ::fwData::Object::s_MODIFIED_SIG, hasSlots, slotKey);
    }
}

//------------------------------------------------------------------------------

void ConnectPoints::disconnectSplinePoints()
{
    for(auto elt : m_connections)
    {
        elt.second.disconnect();
    }

    m_connections.clear();
}

} // namespace navigation

