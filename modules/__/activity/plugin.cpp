/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <activity/extension/activity.hpp>

namespace sight::module::activity
{

SIGHT_REGISTER_PLUGIN("sight::module::activity::plugin");

plugin::~plugin() noexcept =
    default;

//------------------------------------------------------------------------------

void plugin::start()
{
    sight::activity::extension::activity::get_default()->parse_plugin_infos();
}

//------------------------------------------------------------------------------

void plugin::stop() noexcept
{
    // Clear all operator configurations
    sight::activity::extension::activity::get_default()->clear_registry();
}

} // namespace sight::module::activity
