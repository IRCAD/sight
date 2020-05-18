/************************************************************************
 *
 * Copyright (C) 2017-2018 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "arServices/ITracker.hpp"

#include <arData/BufferTL.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

//-----------------------------------------------------------------------------

namespace arServices
{

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType ITracker::s_TRACK_SLOT          = "track";
const ::fwCom::Slots::SlotKeyType ITracker::s_START_TRACKING_SLOT = "startTracking";
const ::fwCom::Slots::SlotKeyType ITracker::s_STOP_TRACKING_SLOT  = "stopTracking";

const ::fwServices::IService::KeyType ITracker::s_TIMELINE_INPUT = "timeline";
const ::fwServices::IService::KeyType ITracker::s_FRAME_INOUT    = "frame";

//-----------------------------------------------------------------------------

ITracker::ITracker() :
    m_lastTimestamp(0),
    m_dropObj(true),
    m_isTracking(false)
{
    newSlot(s_TRACK_SLOT, &ITracker::track, this);
    newSlot(s_START_TRACKING_SLOT, &ITracker::startTracking, this);
    newSlot(s_STOP_TRACKING_SLOT, &ITracker::stopTracking, this);
}

//-----------------------------------------------------------------------------

ITracker::~ITracker()
{
}

//-----------------------------------------------------------------------------

void ITracker::configuring()
{
    const ::fwServices::IService::ConfigType config = this->getConfigTree();
    if (config.count("dropObj"))
    {
        const std::string dropStr = config.get< std::string >("dropObj");
        SLM_ASSERT("'dropObj' value must be 'true' or 'false'.", dropStr == "true" || dropStr == "false");
        m_dropObj = (dropStr == "true");
    }
}

//-----------------------------------------------------------------------------

void ITracker::track(::fwCore::HiResClock::HiResClockType timestamp)
{
    SLM_DEBUG_IF("["+this->getClassname()+"] Tracking is not started: does nothing", !m_isTracking);
    SLM_DEBUG_IF("["+this->getClassname()+"] Dropping object at " + std::to_string(timestamp),
                 m_isTracking && m_dropObj && timestamp <= m_lastTimestamp);

    if (m_isTracking && (!m_dropObj || timestamp > m_lastTimestamp))
    {
        ::arData::BufferTL::csptr timeline = this->getInput< ::arData::BufferTL >(s_TIMELINE_INPUT);
        SLM_WARN_IF("the object '" + s_TIMELINE_INPUT + "' is not defined, the 'drop' mode cannot be managed.",
                    !timeline);
        if (timeline)
        {
            if (m_dropObj)
            {
                timestamp = timeline->getNewerTimestamp();
            }
            if (timeline->getClosestObject(timestamp) == nullptr)
            {
                SLM_WARN("["+this->getClassname()+"] No buffer found for the timeline.");
                return;
            }
        }

        SLM_DEBUG("["+this->getClassname()+"] Tracking at " +  std::to_string(timestamp) + "...");
        this->tracking(timestamp);
        m_lastTimestamp = timestamp;
    }
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap ITracker::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push( s_TIMELINE_INPUT, ::arData::BufferTL::s_OBJECT_PUSHED_SIG, s_TRACK_SLOT );

    return connections;
}

//-----------------------------------------------------------------------------

void ITracker::startTracking()
{
    m_isTracking = true;
}

//-----------------------------------------------------------------------------

void ITracker::stopTracking()
{
    m_isTracking = false;
}

//-----------------------------------------------------------------------------

} // namespace arServices
