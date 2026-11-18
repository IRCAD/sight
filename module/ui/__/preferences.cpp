/************************************************************************
 *
 * Copyright (C) 2024-2026 IRCAD France
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

#include "preferences.hpp"

#include <data/string_serializable.hpp>

#include <ui/__/preferences.hpp>

#include <algorithm>
#include <chrono>
#include <iterator>
#include <vector>

namespace sight::module::ui
{

//------------------------------------------------------------------------------

service::connections_t preferences::auto_connections() const
{
    return {{m_preferences, data::signals::MODIFIED, service::slots::UPDATE}};
}

//-----------------------------------------------------------------------------

void preferences::configuring(const config_t& /*_config*/)
{
}

//-----------------------------------------------------------------------------

void preferences::starting()
{
    m_timer_worker = core::thread::worker::make();
    try
    {
        sight::ui::preferences preferences;
        std::ranges::for_each(
            m_preferences,
            [&](const auto& _x)
            {
                const auto save_pref =
                    [&preferences](const auto& _obj, const std::string& _key)
                    {
                        if(const auto& found = preferences.get_optional<std::string>(_key); found)
                        {
                            _obj->from_string(*found);
                            // We do not send any signal because we assume the service is started
                            // first in the config
                        }
                        else
                        {
                            preferences.put(_key, _obj->to_string());
                        }
                    };

                const auto obj         = _x.second->lock();
                auto serializable_data = std::dynamic_pointer_cast<sight::data::string_serializable>(obj.get_shared());
                if(serializable_data)
                {
                    save_pref(serializable_data, serializable_data->base_id());
                }
                else
                {
                    auto map = std::dynamic_pointer_cast<sight::data::map>(obj.get_shared());
                    if(map)
                    {
                        std::ranges::for_each(
                            *map,
                            [this, &save_pref, id = map->base_id()](const auto& _x)
                        {
                            auto data = std::dynamic_pointer_cast<sight::data::string_serializable>(_x.second);
                            save_pref(data, core::id::join(id, _x.first));
                            m_map_keys_connections.connect(
                                data,
                                data::signals::MODIFIED,
                                this->get_sptr(),
                                service::base::slots::UPDATE
                            );
                        });
                    }
                    else
                    {
                        SIGHT_ASSERT("Object is not a string serializable nor a map", serializable_data);
                    }
                }
            });
    }
    catch(const sight::ui::preferences_disabled& /*e*/)
    {
        // Preferences are disabled.
    }
}

//-----------------------------------------------------------------------------

void preferences::updating()
{
    // Delay saving by one second to avoid repeated disk writes during rapid changes.
    auto timer = m_timer_worker->create_timer();

    core::mt::write_lock lock(m_timers_mutex);
    const int timer_id = ++m_last_timer_id;
    m_timers[timer_id] = timer;
    timer->set_duration(std::chrono::milliseconds(1000));
    timer->set_one_shot(true);

    timer->set_function(
        [this, timer_id]()
        {
            // Only the latest timer saves the preferences.
            if(timer_id == m_last_timer_id)
            {
                // Keep the timer alive while writing, if stopping() has not already removed it.
                core::mt::read_lock lock(m_timers_mutex);
                const auto found = m_timers.find(timer_id);
                const auto timer = found != m_timers.end() ? found->second : nullptr;

                write();
                m_last_timer_id = -1;
            }

            core::mt::write_lock lock(m_timers_mutex);
            m_timers.erase(timer_id);
        });
    timer->start();
}

//-----------------------------------------------------------------------------

void preferences::write()
{
    try
    {
        sight::ui::preferences preferences;

        std::ranges::for_each(
            m_preferences,
            [&](const auto& _x)
            {
                const auto obj         = _x.second->const_lock();
                auto serializable_data =
                    std::dynamic_pointer_cast<const sight::data::string_serializable>(obj.get_shared());

                if(serializable_data)
                {
                    preferences.put(obj->base_id(), serializable_data->to_string());
                }
                else
                {
                    auto map = std::dynamic_pointer_cast<const sight::data::map>(obj.get_shared());
                    if(map)
                    {
                        std::ranges::for_each(
                            *map,
                            [&preferences, id = map->base_id()](const auto& _x)
                        {
                            auto data =
                                std::dynamic_pointer_cast<const sight::data::string_serializable>(_x.second);
                            preferences.put(core::id::join(id, _x.first), data->to_string());
                        });
                    }
                    else
                    {
                        SIGHT_ASSERT("Object is not a string serializable nor a map", serializable_data);
                    }
                }
            });
    }
    catch(const sight::ui::preferences_disabled& /*e*/)
    {
        // Preferences are disabled.
    }
}

//-----------------------------------------------------------------------------

void preferences::stopping()
{
    // Take the timers out under the lock, and keep them alive until the worker has stopped: stopping the worker waits
    // for a callback still running, which takes the lock to erase its timer, and nothing else guarantees that its timer
    // outlives that callback.
    std::vector<sight::core::thread::timer::sptr> timers;
    {
        core::mt::write_lock lock(m_timers_mutex);
        std::ranges::transform(m_timers, std::back_inserter(timers), [](const auto& _t){return _t.second;});
        m_timers.clear();
    }

    std::ranges::for_each(timers, [](const auto& _t){_t->stop();});
    m_timer_worker->stop();

    // Destroy the timers before their worker, as they depend on its context.
    timers.clear();
    m_timer_worker.reset();

    // Save pending changes before stopping.
    if(m_last_timer_id != -1)
    {
        write();
        m_last_timer_id = -1;
    }

    m_map_keys_connections.disconnect();
}

//-----------------------------------------------------------------------------

} //namespace sight::module::ui
