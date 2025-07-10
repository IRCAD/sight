/************************************************************************
 *
 * Copyright (C) 2024-2025 IRCAD France
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

#include "updater.hpp"

namespace sight::app
{

//-----------------------------------------------------------------------------

void updater::configuring(const config_t& _config)
{
    const auto config = _config.get_child("config");
    m_loop   = config.get<bool>("<xmlattr>.loop", m_loop);
    m_parent = config.get<std::string>("<xmlattr>.parent", "");
    for(const auto& element : config)
    {
        if(element.first == "service")
        {
            const auto uid            = element.second.get<std::string>("<xmlattr>.uid");
            const auto slot           = element.second.get<std::string>("<xmlattr>.slot", "update");
            const auto ignore_stopped = element.second.get<bool>("<xmlattr>.ignore_stopped", false);
            m_elements.push_back({uid, slot, type_t::SERVICE, ignore_stopped});
        }
        else if(element.first == "updater")
        {
            const auto uid = element.second.get<std::string>("<xmlattr>.uid");
            m_elements.push_back({uid, "update", type_t::UPDATER});
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::app
