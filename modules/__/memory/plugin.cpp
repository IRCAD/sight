/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "plugin.hpp"

#include <core/memory/buffer_manager.hpp>
#include <core/spy_log.hpp>

namespace sight::module::memory
{

//-----------------------------------------------------------------------------

SIGHT_REGISTER_PLUGIN("sight::module::memory::plugin");

//-----------------------------------------------------------------------------

plugin::~plugin() noexcept =
    default;

//-----------------------------------------------------------------------------

void plugin::start()
{
    core::memory::buffer_manager::sptr manager = core::memory::buffer_manager::get();

    const std::string mode_key = "loading_mode";

    if(this->get_module()->has_parameter(mode_key))
    {
        core::mt::write_lock lock(manager->get_mutex());
        std::string mode = this->get_module()->get_parameter_value(mode_key);
        if(mode == "lazy")
        {
            manager->set_loading_mode(core::memory::buffer_manager::LAZY);
            SIGHT_INFO("Enabled lazy loading mode");
        }
        else if(mode == "direct")
        {
            manager->set_loading_mode(core::memory::buffer_manager::DIRECT);
            SIGHT_INFO("Enabled direct loading mode");
        }
        else
        {
            SIGHT_ERROR("Unknown loading mode : '" + mode + "'");
        }
    }
}

//-----------------------------------------------------------------------------

void plugin::stop() noexcept
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::memory
