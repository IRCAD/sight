/************************************************************************
 *
 * Copyright (C) 2015-2024 IRCAD France
 * Copyright (C) 2015-2020 IHU Strasbourg
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

#include "sight/module_app/config.hpp"

#include <app/config_manager.hpp>

#include <core/runtime/plugin.hpp>

namespace sight::module::app
{

/**
 * @brief   This class is started when the module is loaded.
 */
class SIGHT_MODULE_APPXML_CLASS_API plugin : public core::runtime::plugin
{
public:

    /// Constructor.
    SIGHT_MODULE_APPXML_API plugin() noexcept;

    /// Destructor. Do nothing.
    SIGHT_MODULE_APPXML_API ~plugin() noexcept;

    /// Overrides start method. .
    SIGHT_MODULE_APPXML_API void start() override;

    /// Overrides stop method. Do nothing
    SIGHT_MODULE_APPXML_API void stop() noexcept override;

private:

    std::string m_configurationName;
    std::string m_parametersName;
    app::config_manager::sptr m_appConfigMng;
};

} // namespace sight::module::app
