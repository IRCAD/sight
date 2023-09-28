/************************************************************************
 *
 * Copyright (C) 2015-2023 IRCAD France
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

#include "module_appXml/config.hpp"

#include <core/runtime/plugin.hpp>

#include <service/app_config_manager.hpp>

namespace sight::module::appXml
{

/**
 * @brief   This class is started when the module is loaded.
 */
class MODULE_APPXML_CLASS_API plugin : public core::runtime::plugin
{
public:

    /// Constructor.
    MODULE_APPXML_API plugin() noexcept;

    /// Destructor. Do nothing.
    MODULE_APPXML_API ~plugin() noexcept;

    /// Overrides start method. .
    MODULE_APPXML_API void start() override;

    /// Overrides stop method. Do nothing
    MODULE_APPXML_API void stop() noexcept override;

private:

    std::string m_configurationName;
    std::string m_parametersName;
    service::app_config_manager::sptr m_appConfigMng;
};

} // namespace sight::module::appXml
