/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include <service/extension/config.hpp>
#include <service/extension/factory.hpp>

namespace sight::module::service
{

SIGHT_REGISTER_PLUGIN("sight::module::service::plugin");

//-----------------------------------------------------------------------------

void plugin::start()
{
    sight::service::extension::factory::get()->parse_plugin_infos();
    sight::service::extension::config::getDefault()->parse_plugin_infos();
}

//-----------------------------------------------------------------------------

void plugin::stop() noexcept
{
    // Clear all service configs
    sight::service::extension::config::getDefault()->clear_registry();

    // Clear all service factories
    sight::service::extension::factory::get()->clear_factory();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::service
