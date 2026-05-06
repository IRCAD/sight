/************************************************************************
 *
 * Copyright (C) 2017-2026 IRCAD France
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

#pragma once

#include "base.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <data/buffer_tl.hpp>

//-----------------------------------------------------------------------------

namespace sight::io::tracking
{

//-----------------------------------------------------------------------------

template<typename T>
const sight::core::com::slots::key_t base<T>::TRACK_SLOT = "track";

//-----------------------------------------------------------------------------

template<typename T>
base<T>::base()
{
    new_signal<typename signals::void_t>(signals::TRACKING_STARTED);
    new_signal<typename signals::void_t>(signals::TRACKING_STOPPED);

    new_slot(TRACK_SLOT, &base::track, this);
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
    const service::config_t config = this->get_config();

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
void base<T>::configuring(const config_t& /*unused*/)
{
}

//-----------------------------------------------------------------------------

template<typename T>
void base<T>::track(core::clock::type _timestamp)
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
service::connections_t base<T>::auto_connections() const
{
    return {{m_timeline, data::timeline::signals::PUSHED, TRACK_SLOT}};
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

} // namespace sight::io::tracking
