/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
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

#include <core/com/slots.hxx>

#include <data/buffer_tl.hpp>

//-----------------------------------------------------------------------------

namespace sight::service
{

//-----------------------------------------------------------------------------

const core::com::slots::key_t tracker::TRACK_SLOT          = "track";
const core::com::slots::key_t tracker::START_TRACKING_SLOT = "start_tracking";
const core::com::slots::key_t tracker::STOP_TRACKING_SLOT  = "stop_tracking";

//-----------------------------------------------------------------------------

tracker::tracker()
{
    new_slot(TRACK_SLOT, &tracker::track, this);
    new_slot(START_TRACKING_SLOT, &tracker::start_tracking, this);
    new_slot(STOP_TRACKING_SLOT, &tracker::stop_tracking, this);
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
        const auto drop_str = config.get<std::string>("dropObj");
        SIGHT_ASSERT("'dropObj' value must be 'true' or 'false'.", drop_str == "true" || drop_str == "false");
        m_drop_obj = (drop_str == "true");
    }
}

//-----------------------------------------------------------------------------

void tracker::configuring(const config_t& /*unused*/)
{
    // Move the code of the following method here once it reaches the end of the deprecation phase
    this->configuring();
}

//-----------------------------------------------------------------------------

void tracker::track(core::clock::type _timestamp)
{
    SIGHT_DEBUG_IF("[" + this->get_classname() + "] Tracking is not started: does nothing", !m_is_tracking);
    SIGHT_DEBUG_IF(
        "[" + this->get_classname() + "] Dropping object at " + std::to_string(_timestamp),
        m_is_tracking && m_drop_obj && _timestamp <= m_last_timestamp
    );

    if(m_is_tracking && (!m_drop_obj || _timestamp > m_last_timestamp))
    {
        {
            const auto timeline = m_timeline.lock();
            SIGHT_WARN_IF(
                "the object '" << TIMELINE_INPUT << "' is not defined, the 'drop' mode cannot be managed.",
                !timeline
            );
            if(timeline)
            {
                if(m_drop_obj)
                {
                    _timestamp = timeline->get_newer_timestamp();
                }

                if(timeline->get_closest_object(_timestamp) == nullptr)
                {
                    SIGHT_WARN("[" + this->get_classname() + "] No buffer found for the timeline.");
                    return;
                }
            }
        }

        SIGHT_DEBUG("[" + this->get_classname() + "] Tracking at " + std::to_string(_timestamp) + "...");
        this->tracking(_timestamp);
        m_last_timestamp = _timestamp;
    }
}

//-----------------------------------------------------------------------------

service::connections_t tracker::auto_connections() const
{
    return {{m_timeline, data::timeline::signals::PUSHED, TRACK_SLOT}};
}

//-----------------------------------------------------------------------------

void tracker::start_tracking()
{
    m_is_tracking = true;
}

//-----------------------------------------------------------------------------

void tracker::stop_tracking()
{
    m_is_tracking = false;
}

//-----------------------------------------------------------------------------

} // namespace sight::service
