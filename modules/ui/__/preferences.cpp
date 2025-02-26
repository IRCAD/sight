/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include <core/com/slots.hxx>
#include <core/object.hpp>
#include <core/runtime/path.hpp>
#include <core/time_stamp.hpp>

#include <data/string_serializable.hpp>

namespace sight::module::ui
{

//------------------------------------------------------------------------------

service::connections_t preferences::auto_connections() const
{
    return {{m_preferences, data::object::MODIFIED_SIG, service::slots::UPDATE}};
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
                                data::object::MODIFIED_SIG,
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
        // Nothing to do...
    }
}

//-----------------------------------------------------------------------------

void preferences::updating()
{
    /// Timer used to delay the write of preferences and avoid to spam disk I/O
    auto timer = m_timer_worker->create_timer();

    core::mt::write_lock lock(m_timers_mutex);
    m_timers[++m_last_timer_id] = timer;
    timer->set_duration(std::chrono::milliseconds(1000));
    timer->set_one_shot(true);

    timer->set_function(
        [this, timer_id = m_last_timer_id]()
        {
            // Only saves if this is the last timer
            if(timer_id == m_last_timer_id)
            {
                core::mt::read_lock lock(m_timers_mutex);
                auto timer = m_timers[timer_id];

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
                    // Nothing to do...
                }

                m_last_timer_id = -1;
            }

            core::mt::read_lock write(m_timers_mutex);
            m_timers.erase(timer_id);
        });
    timer->start();
}

//-----------------------------------------------------------------------------

void preferences::stopping()
{
    // Let a chance to write if we close the app very fast
    sight::core::time_stamp time_stamp;
    time_stamp.set_life_period(1000);
    time_stamp.modified();
    while(!m_timers.empty() && !time_stamp.period_expired())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::ranges::for_each(m_timers, [](const auto& _t){_t.second->stop();});
    m_timers.clear();
    m_timer_worker->stop();
    m_timer_worker.reset();
    m_map_keys_connections.disconnect();
}

//-----------------------------------------------------------------------------

} //namespace sight::module::ui
