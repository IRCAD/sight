/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#pragma once

#include <sight/service/config.hpp>

#include <core/clock.hpp>

#include <data/frame_tl.hpp>

#include <service/base.hpp>

namespace sight::service
{

/**
 * @brief  This interface defines Tracker service API.
 *
 * The sub-classes must implements the method 'tracking(timestamp)' that is called by 'track' slot.
 *
 * If 'dropObj' is enabled, the sub-class 'tracking' method is called with the last timestamp in the timeline if the
 * received timestamp is greater than the last processed timestamp.
 *
 * @section Slots Slots
 * - \b track(core::clock::type  timestamp) : performs the tracking, does nothing if the tracking is
 *   not started.
 * - \b start_tracking() : start the tracking
 * - \b stop_tracking() : stop the tracking
 *
 * @subsection Input Input
 * - \b timeline : timeline used to retrieve the tracked objects
 * @subsection Configuration Configuration
 * - \b dropObj(optional, default=true) : defines if the tracker should drop few objects from the timeline (and always
 *   get the last one) or not.
 */
class SIGHT_SERVICE_CLASS_API tracker : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(tracker, service::base);

    SIGHT_SERVICE_API static const core::com::slots::key_t TRACK_SLOT;
    SIGHT_SERVICE_API static const core::com::slots::key_t START_TRACKING_SLOT;
    SIGHT_SERVICE_API static const core::com::slots::key_t STOP_TRACKING_SLOT;

    SIGHT_SERVICE_API static constexpr std::string_view TIMELINE_INPUT = "timeline";
    SIGHT_SERVICE_API static constexpr std::string_view FRAME_INOUT    = "frame";

    /// Defines the auto-connection between the timeline and the 'track' slot
    SIGHT_SERVICE_API service::connections_t auto_connections() const override;

    /// Return true if the tracking is started.
    bool is_tracking() const
    {
        return m_is_tracking;
    }

    /// Enable/Disable drop
    void enable_drop(bool _enable)
    {
        m_drop_obj = _enable;
    }

    /// Return true if tracker drop frames
    bool is_dropping() const
    {
        return m_drop_obj;
    }

protected:

    ///@brief tracker constructor. Do nothing.
    SIGHT_SERVICE_API tracker();

    ///@brief tracker destructor. Do nothing.
    SIGHT_SERVICE_API ~tracker() override;

    SIGHT_SERVICE_API void configuring() override;
    SIGHT_SERVICE_API void configuring(const config_t& _config) override;

    /**
     * @brief This method calls tracking.
     * If m_dropObj is true, tracking is called only if the given timestamp is greater than m_lastTimestamp.
     * @warning If tracking is stopped, this method does nothing.
     * @note You should connect this method to the input timeline
     */
    SIGHT_SERVICE_API virtual void track(core::clock::type _timestamp);

    /// start the tracking
    SIGHT_SERVICE_API virtual void start_tracking();

    /// stop the tracking
    SIGHT_SERVICE_API virtual void stop_tracking();

    /**
     * @brief process the tracking
     * @param[in,out] _timestamp the timestamp of the processes object of the timeline
     */
    SIGHT_SERVICE_API virtual void tracking(core::clock::type& _timestamp) = 0;

    /// timestamp of the last tracking
    core::clock::type m_last_timestamp {0};

    /// If true, only last object is retrieved
    bool m_drop_obj {true};

    /// If false, the trackMethod does nothing
    bool m_is_tracking {false};

    sight::data::ptr<sight::data::frame_tl, sight::data::access::in> m_timeline {this, TIMELINE_INPUT};
};

} // namespace sight::service
