/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
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

#include <data/color.hpp>
#include <data/frame_tl.hpp>

#include <service/base.hpp>

namespace sight::service
{

/// Tracking quality, the lowest values are, better tracking is.
enum class tracking_quality_t : std::uint8_t
{
    tracked = 0,             ///< fully tracked.
    partially_out_of_volume, ///< partially out of volume.
    error_on_detection,      ///< other errors that affects detection.
    untracked                ///< untracked.
};

// Structure to deal with sensors.
struct SIGHT_SERVICE_CLASS_API sensor_t
{
    std::string name;                      // name of the tool
    std::string tracked_color {"#FFFFFF"}; // tracked color
    bool optional {false};                 // Requires the sensor to be plugged
    std::size_t tl_index {0};              // index on the timeline

    tracking_quality_t current_tracking_quality {tracking_quality_t::untracked}; // tracking quality
    std::string current_status;                                                  // current status
                                                                                 // (tracked/missing/...)

    // signal names
    std::string detected_sig_name;
    std::string undetected_sig_name;
    std::string error_in_detection_sig_name;
    std::string error_status_changed_sig_name;
    std::string change_color_sig_name;

    //------------------------------------------------------------------------------

    virtual ~sensor_t() = default;

    //------------------------------------------------------------------------------

    [[nodiscard]] virtual std::string to_string() const
    {
        std::stringstream ss;

        ss << "- name: '" << this->name << "'" << std::endl;
        ss << "- tracked_color: '" << this->tracked_color << "'" << std::endl;
        ss << "- optional: '" << this->optional << "'" << std::endl;
        ss << "- tl_index: '" << this->tl_index << "'" << std::endl;

        return ss.str();
    }

    // for debug purposes.
    friend std::ostream& operator<<(std::ostream& _os, const sensor_t& _s)
    {
        _os << _s.to_string();
        return _os;
    }
};

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
template<typename T>
class SIGHT_SERVICE_CLASS_API tracker : public service::base
{
public:

    SIGHT_DECLARE_SERVICE(tracker, service::base);

    using void_t = sight::core::com::signal<void ()>;

    struct signals
    {
        using void_t = sight::core::com::signal<void ()>;
        using bool_t = sight::core::com::signal<void (bool)>;

        using data_color_t = sight::data::color::sptr;
        using color_t      = sight::core::com::signal<void (data_color_t)>;
        using tracking_t   = sight::core::com::signal<void ()>;

        static inline const sight::core::com::signals::key_t TRACKING_STARTED = "tracking_started";
        static inline const sight::core::com::signals::key_t TRACKING_STOPPED = "tracking_stopped";
    };

    struct slots
    {
        using key_t = sight::core::com::slots::key_t;

        static inline const key_t START_TRACKING = "start_tracking";
        static inline const key_t STOP_TRACKING  = "stop_tracking";
    };

    struct color_codes
    {
        static inline const std::string GREEN  = "#00FF00";
        static inline const std::string ORANGE = "#FFA500";
        static inline const std::string RED    = "#FF0000";
    };

    static const core::com::slots::key_t TRACK_SLOT;

    static constexpr std::string_view TIMELINE_INPUT = "timeline";
    static constexpr std::string_view FRAME_INOUT    = "frame";

    /// Defines the auto-connection between the timeline and the 'track' slot
    service::connections_t auto_connections() const override;

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

    std::vector<T> m_sensors;

    ///@brief tracker constructor. Do nothing.
    tracker();

    ///@brief tracker destructor. Do nothing.
    ~tracker() override = default;

    virtual void setup_tool(
        const boost::property_tree::ptree& _config,
        T& _tool,
        std::size_t _index = 0
    );

    void configuring() override;
    void configuring(const config_t& _config) override;

    /**
     * @brief This method calls tracking.
     * If m_dropObj is true, tracking is called only if the given timestamp is greater than m_lastTimestamp.
     * @warning If tracking is stopped, this method does nothing.
     * @note You should connect this method to the input timeline
     */
    virtual void track(core::clock::type _timestamp);

    /// start the tracking
    virtual void start_tracking();

    /// stop the tracking
    virtual void stop_tracking();

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
    std::atomic_bool m_is_tracking {false};

    sight::data::ptr<sight::data::frame_tl, sight::data::access::in> m_timeline {this, TIMELINE_INPUT};
};

} // namespace sight::service
