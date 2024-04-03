/************************************************************************
 *
 * Copyright (C) 2015-2024 IRCAD France
 * Copyright (C) 2015-2021 IHU Strasbourg
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

#include "sight/modules/app/config.hpp"

#include <app/config_manager.hpp>

#include <core/runtime/plugin.hpp>

namespace sight::module::app
{

/**
 * @brief   This class is started when the module is loaded.
 */
class SIGHT_MODULE_APP_CLASS_API plugin : public core::runtime::plugin
{
public:

    /// Destructor. Do nothing.
    SIGHT_MODULE_APP_API ~plugin() noexcept override = default;

    /// Overrides start method. .
    SIGHT_MODULE_APP_API void start() override;

    /// Overrides stop method. Do nothing
    SIGHT_MODULE_APP_API void stop() noexcept override;

private:

    /// Allows to postpone the launch of the application of the main worker
    void run();

    std::string m_configuration_name;
    std::string m_parameters_name;
    sight::app::config_manager::sptr m_app_config_mng;
};

} // namespace sight::module::app
