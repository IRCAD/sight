/************************************************************************
 *
 * Copyright (C) 2015-2017 IRCAD France
 * Copyright (C) 2015-2017 IHU Strasbourg
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

#ifndef __APPXML_PLUGIN_HPP__
#define __APPXML_PLUGIN_HPP__

#include "appXml/config.hpp"

#include <fwRuntime/Plugin.hpp>

#include <fwServices/AppConfigManager.hpp>

namespace appXml
{

/**
 * @brief   This class is started when the bundles is loaded.
 */
class APPXML_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Constructor.
    APPXML_API Plugin() noexcept;

    /// Destructor. Do nothing.
    APPXML_API ~Plugin() noexcept;

    /// Overrides start method. .
    APPXML_API void start();

    /// Overrides stop method. Do nothing
    APPXML_API void stop() noexcept;

    APPXML_API void initialize();

    APPXML_API void uninitialize() noexcept;

private:
    std::string m_configurationName;
    std::string m_parametersName;
    ::fwServices::AppConfigManager::sptr m_appConfigMng;
};

} // namespace appXml

#endif // __APPXML_PLUGIN_HPP__
