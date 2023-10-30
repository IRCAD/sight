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

#include "app/config_controller.hpp"

#include "service/macros.hpp"

namespace sight::app
{

//------------------------------------------------------------------------------

config_controller::config_controller() noexcept :
    m_config_launcher(std::make_unique<app::helper::config_launcher>())
{
}

//------------------------------------------------------------------------------

void config_controller::configuring(const config_t& _config)
{
    m_config_launcher->parse_config(_config, this->get_sptr());
}

//------------------------------------------------------------------------------

void config_controller::starting()
{
    m_config_launcher->start_config(this->get_sptr());
}

//------------------------------------------------------------------------------

void config_controller::stopping()
{
    m_config_launcher->stop_config();
}

//------------------------------------------------------------------------------

void config_controller::updating()
{
}

//------------------------------------------------------------------------------

void config_controller::info(std::ostream& /*_sstream*/)
{
}

//------------------------------------------------------------------------------

} // namespace sight::app
