/************************************************************************
 *
 * Copyright (C) 2014-2026 IRCAD France
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

#include <sight/io/tracking/config.hpp>

#include <core/clock.hpp>

#include <data/color.hpp>
#include <data/frame_tl.hpp>

#include <service/base.hpp>

#include <vector>

namespace sight::io::tracking
{

/// Tracking quality, the lowest values are, better tracking is.
enum class status_t : std::uint8_t
{
    tracked = 0,             ///< fully tracked.
    partially_out_of_volume, ///< partially out of volume.
    error_on_detection,      ///< other errors that affects detection.
    untracked                ///< untracked.
};

// Structure to deal with sensors.
struct SIGHT_IO_TRACKING_CLASS_API sensor_t
{
    std::string name;         // name of the tool
    bool optional {false};    // Requires the sensor to be plugged
    std::size_t tl_index {0}; // index on the timeline

    status_t current_tracking_quality {status_t::untracked}; // tracking quality
    std::string current_status;                              // current status
                                                             // (tracked/missing/...)

    //------------------------------------------------------------------------------

    SIGHT_IO_TRACKING_API virtual ~sensor_t() = default;

    //------------------------------------------------------------------------------

    [[nodiscard]] SIGHT_IO_TRACKING_API virtual std::string to_string() const;

    //------------------------------------------------------------------------------

    /// for debug purposes.
    SIGHT_IO_TRACKING_API friend std::ostream& operator<<(std::ostream& _os, const sensor_t& _s);
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
class SIGHT_IO_TRACKING_CLASS_API base : public sight::service::base
{
public:

    SIGHT_DECLARE_SERVICE(base, sight::service::base);

    using void_t = sight::core::com::signal<void ()>;

    struct signals
    {
        using void_t = sight::core::com::signal<void ()>;

        static inline const signal_key_t TRACKING_STARTED = "tracking_started";
        static inline const signal_key_t TRACKING_STOPPED = "tracking_stopped";
    };

    struct slots
    {
        static inline const slot_key_t START_TRACKING = "start_tracking";
        static inline const slot_key_t STOP_TRACKING  = "stop_tracking";
        static inline const slot_key_t TRACK          = "track";
    };

    struct color_codes
    {
        static inline const std::string GREEN  = "#00FF00";
        static inline const std::string ORANGE = "#FFA500";
        static inline const std::string RED    = "#FF0000";
    };

    static constexpr std::string_view TIMELINE_INPUT = "timeline";
    static constexpr std::string_view FRAME_INOUT    = "frame";

    ///@brief tracker destructor. Do nothing.
    ~base() override = default;

    /// Return true if the tracking is started.
    [[nodiscard]] bool is_tracking() const;

    /// Set the tracking state, return the previous state.
    bool set_tracking(bool _tracking);

    /// Enable/Disable drop
    void enable_drop(bool _enable);

    /// Return true if tracker drop frames
    [[nodiscard]] bool is_dropping() const;

protected:

    ///@brief tracker constructor. Do nothing.
    base();

    virtual void setup_tool(
        const boost::property_tree::ptree& _config,
        T& _tool,
        std::size_t _index = 0
    );

    /// Defines the auto-connection between the timeline and the 'track' slot
    sight::service::connections_t auto_connections() const override;

    void configuring() override;
    void configuring(const config_t& _config) override;

    /**
     * @brief process the tracking
     * @param[in,out] _timestamp the timestamp of the processes object of the timeline
     */
    SIGHT_IO_TRACKING_API virtual void tracking(core::clock::type& _timestamp) = 0;

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

    /// Access to the sensors vector
    std::vector<T>& sensors();

    /// Access to the sensors vector (const version)
    const std::vector<T>& sensors() const;

private:

    std::vector<T> m_sensors;

    /// timestamp of the last tracking
    core::clock::type m_last_timestamp {0};

    /// If true, only last object is retrieved
    bool m_drop_obj {true};

    /// If false, the trackMethod does nothing
    std::atomic_bool m_is_tracking {false};

    sight::data::ptr<sight::data::frame_tl, sight::data::access::in> m_timeline {this, TIMELINE_INPUT};
};

//-----------------------------------------------------------------------------

template<typename T>
base<T>::base()
{
    new_signal<typename signals::void_t>(signals::TRACKING_STARTED);
    new_signal<typename signals::void_t>(signals::TRACKING_STOPPED);

    new_slot(base<T>::slots::TRACK, &base::track, this);
    new_slot(base<T>::slots::START_TRACKING, &base::start_tracking, this);
    new_slot(base<T>::slots::STOP_TRACKING, &base::stop_tracking, this);
}

//-----------------------------------------------------------------------------

template<typename T>
void base<T>::setup_tool(
    const boost::property_tree::ptree& _config,
    T& _tool,
    std::size_t _index
)
{
    const auto toolattr = _config.get_child("<xmlattr>");

    // 1. Name (should be unique).
    const auto name = toolattr.get<std::string>("name");

    [[maybe_unused]] const auto name_exists = std::ranges::find_if(
        m_sensors,
        [&name](const T& _s)
        {
            return name == _s.name;
        });

    SIGHT_ASSERT("'" + name + "' is already used", name_exists == m_sensors.end());

    _tool.name = name;

    SIGHT_ASSERT("'name' attribute must not be empty", !name.empty());

    // Can be missing ?
    const auto opt = toolattr.get<bool>("optional", false);
    _tool.optional = opt;

    // Timeline index only useful if part number is set, if missing considering xml order as the index.
    const auto tl_index            = toolattr.get<std::size_t>("index", _index);
    [[maybe_unused]] const auto it = std::ranges::find_if(
        m_sensors,
        [&tl_index](const T& _s)
        {
            return _s.tl_index == tl_index;
        });

    SIGHT_ASSERT(
        "Timeline index '" + std::to_string(tl_index) + "' already exists for tool '" + it->name + "'.",
        it == m_sensors.end()
    );

    _tool.tl_index = tl_index;
}

//------------------------------------------------------------------------------

template<typename T>
void base<T>::configuring()
{
    const sight::service::config_t config = this->get_config();

    // Parse tools configuration
    const auto tools = config.get_child_optional("tools");

    if(tools.has_value() && tools->count("tool") > 0)
    {
        const auto tool_cfg = tools->equal_range("tool");

        // Configure tracked tool
        std::size_t default_index = 0;
        for(auto t = tool_cfg.first ; t != tool_cfg.second ; ++t)
        {
            T tool;
            setup_tool(t->second, tool, default_index++);
            m_sensors.push_back(tool);
        }
    }
    else
    {
        SIGHT_ERROR("Missing tools section in tracker device.")
    }

    if(config.count("dropObj") != 0U)
    {
        const auto drop_str = config.get<std::string>("dropObj");
        SIGHT_ASSERT("'dropObj' value must be 'true' or 'false'.", drop_str == "true" || drop_str == "false");
        m_drop_obj = (drop_str == "true");
    }
}

//-----------------------------------------------------------------------------

template<typename T>
void base<T>::configuring(const sight::service::config_t& /*unused*/)
{
}

//-----------------------------------------------------------------------------

template<typename T>
void base<T>::track(sight::core::clock::type _timestamp)
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

template<typename T>
sight::service::connections_t base<T>::auto_connections() const
{
    return {{m_timeline, sight::data::timeline::signals::PUSHED, slots::TRACK}};
}

//-----------------------------------------------------------------------------

template<typename T>
void base<T>::start_tracking()
{
    m_is_tracking = true;
}

//-----------------------------------------------------------------------------

template<typename T>
void base<T>::stop_tracking()
{
    m_is_tracking = false;
}

//-----------------------------------------------------------------------------

template<typename T>
inline bool base<T>::is_tracking() const
{
    return m_is_tracking;
}

//------------------------------------------------------------------------------

template<typename T>
inline bool base<T>::set_tracking(bool _tracking)
{
    return m_is_tracking.exchange(_tracking);
}

//------------------------------------------------------------------------------

template<typename T>
inline void base<T>::enable_drop(bool _enable)
{
    m_drop_obj = _enable;
}

//------------------------------------------------------------------------------

template<typename T>
inline bool base<T>::is_dropping() const
{
    return m_drop_obj;
}

//------------------------------------------------------------------------------

template<typename T>
inline std::vector<T>& base<T>::sensors()
{
    return m_sensors;
}

//------------------------------------------------------------------------------

template<typename T>
inline const std::vector<T>& base<T>::sensors() const
{
    return m_sensors;
}

} // namespace sight::io::tracking
