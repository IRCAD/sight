/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "get_activity_object.hpp"

namespace sight::module::data
{

//------------------------------------------------------------------------------

service::connections_t get_activity_object::auto_connections() const
{
    return {
        {m_activity_set, sight::data::activity_set::MODIFIED_SIG, slots::UPDATE}
    };
}

//------------------------------------------------------------------------------

void get_activity_object::configuring(const service::config_t& _config)
{
    m_id = _config.get<std::string>("id");

    const config_t config_out = _config.get_child("out");

    const auto key_cfg = config_out.equal_range("key");
    for(auto it_cfg = key_cfg.first ; it_cfg != key_cfg.second ; ++it_cfg)
    {
        const auto name = it_cfg->second.get<std::string>("<xmlattr>.name");
        m_keys_names.push_back(name);
    }
}

//------------------------------------------------------------------------------

void get_activity_object::starting()
{
}

//-----------------------------------------------------------------------------

void get_activity_object::updating()
{
    const auto activity_set = m_activity_set.lock();
    if(activity_set == nullptr)
    {
        SIGHT_THROW_EXCEPTION(sight::data::exception("Missing activity set"));
    }

    const auto activity = std::ranges::find_if(
        *activity_set,
        [&](const auto& _x)
        {
            return _x->get_activity_config_id() == m_id;
        });
    if(activity == activity_set->end())
    {
        // NOLINTNEXTLINE(modernize-loop-convert)
        for(std::size_t i = 0 ; i < m_objects.size() ; ++i)
        {
            m_objects[i] = nullptr;
        }

        SIGHT_THROW_EXCEPTION(
            sight::data::exception(
                "Cannot find an activity named '" + m_id + "' in activity set '"
                + activity_set->get_id() + "'"
            )
        );
    }

    size_t i = 0;
    for(const auto& name : m_keys_names)
    {
        if(auto it = activity->get()->find(name); it != activity->get()->end())
        {
            m_objects[i] = (*(*activity))[name];
        }
        else
        {
            m_objects[i] = nullptr;
        }

        i++;
    }
}

//-----------------------------------------------------------------------------

void get_activity_object::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data
