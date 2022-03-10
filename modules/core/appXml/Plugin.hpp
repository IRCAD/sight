/************************************************************************
 *
 * Copyright (C) 2015-2022 IRCAD France
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

#include "modules/appXml/config.hpp"

#include <core/runtime/Plugin.hpp>

#include <service/AppConfigManager.hpp>

namespace sight::module::appXml
{

/**
 * @brief   This class is started when the module is loaded.
 */
class MODULE_APPXML_CLASS_API Plugin : public core::runtime::Plugin
{
public:

    /// Constructor.
    MODULE_APPXML_API Plugin() noexcept;

    /// Destructor. Do nothing.
    MODULE_APPXML_API ~Plugin() noexcept;

    /// Overrides start method. .
    MODULE_APPXML_API void start();

    /// Overrides stop method. Do nothing
    MODULE_APPXML_API void stop() noexcept;

private:

    std::string m_configurationName;
    std::string m_parametersName;
    service::AppConfigManager::sptr m_appConfigMng;
};

} // namespace sight::module::appXml
