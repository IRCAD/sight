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

#include "helper.hpp"

namespace sight::app::ut
{

//------------------------------------------------------------------------------

app::config_manager::sptr launch_app_config_mgr(
    const std::string& _name,
    bool _auto_prefix
)
{
    auto app_config_mgr = app::config_manager::make();

    const app::field_adaptor_t fields;
    app_config_mgr->set_config(_name, fields, _auto_prefix);
    app_config_mgr->launch();

    return app_config_mgr;
}

} // namespace sight::app::ut
