/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "service/tracker.hpp"

#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <data/buffer_tl.hpp>

//-----------------------------------------------------------------------------

namespace sight::service
{

//-----------------------------------------------------------------------------

const core::com::slots::key_t tracker::TRACK_SLOT          = "track";
const core::com::slots::key_t tracker::START_TRACKING_SLOT = "startTracking";
const core::com::slots::key_t tracker::STOP_TRACKING_SLOT  = "stopTracking";

//-----------------------------------------------------------------------------

tracker::tracker()
{
    new_slot(TRACK_SLOT, &tracker::track, this);
    new_slot(START_TRACKING_SLOT, &tracker::startTracking, this);
    new_slot(STOP_TRACKING_SLOT, &tracker::stopTracking, this);
}

//-----------------------------------------------------------------------------

tracker::~tracker()
= default;

//-----------------------------------------------------------------------------

void tracker::configuring()
{
    const service::config_t config = this->get_config();
    if(config.count("dropObj") != 0U)
    {
        const auto dropStr = config.get<std::string>("dropObj");
        SIGHT_ASSERT("'dropObj' value must be 'true' or 'false'.", dropStr == "true" || dropStr == "false");
        m_dropObj = (dropStr == "true");
    }
}

//-----------------------------------------------------------------------------

void tracker::track(core::hires_clock::type timestamp)
{
    SIGHT_DEBUG_IF("[" + this->get_classname() + "] Tracking is not started: does nothing", !m_isTracking);
    SIGHT_DEBUG_IF(
        "[" + this->get_classname() + "] Dropping object at " + std::to_string(timestamp),
        m_isTracking && m_dropObj && timestamp <= m_lastTimestamp
    );

    if(m_isTracking && (!m_dropObj || timestamp > m_lastTimestamp))
    {
        {
            const auto timeline = m_timeline.lock();
            SIGHT_WARN_IF(
                "the object '" << s_TIMELINE_INPUT << "' is not defined, the 'drop' mode cannot be managed.",
                !timeline
            );
            if(timeline)
            {
                if(m_dropObj)
                {
                    timestamp = timeline->getNewerTimestamp();
                }

                if(timeline->getClosestObject(timestamp) == nullptr)
                {
                    SIGHT_WARN("[" + this->get_classname() + "] No buffer found for the timeline.");
                    return;
                }
            }
        }

        SIGHT_DEBUG("[" + this->get_classname() + "] Tracking at " + std::to_string(timestamp) + "...");
        this->tracking(timestamp);
        m_lastTimestamp = timestamp;
    }
}

//-----------------------------------------------------------------------------

service::connections_t tracker::auto_connections() const
{
    return {{s_TIMELINE_INPUT, data::timeline::signals::PUSHED, TRACK_SLOT}};
}

//-----------------------------------------------------------------------------

void tracker::startTracking()
{
    m_isTracking = true;
}

//-----------------------------------------------------------------------------

void tracker::stopTracking()
{
    m_isTracking = false;
}

//-----------------------------------------------------------------------------

} // namespace sight::service
