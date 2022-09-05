/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "Plugin.hpp"

#include <core/thread/Pool.hpp>

#include <service/extension/AppConfig.hpp>
#include <service/extension/AppConfigParameters.hpp>
#include <service/extension/Config.hpp>
#include <service/extension/Factory.hpp>

namespace sight::module::service
{

SIGHT_REGISTER_PLUGIN("sight::module::service::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept =
    default;

//-----------------------------------------------------------------------------

void Plugin::start()
{
    sight::service::extension::Factory::getDefault()->parseBundleInformation();
    sight::service::extension::Config::getDefault()->parseBundleInformation();
    sight::service::extension::AppConfig::getDefault()->parseBundleInformation();
    sight::service::extension::AppConfigParameters::getDefault()->parseBundleInformation();
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    // Clear all service configs
    sight::service::extension::Config::getDefault()->clearRegistry();

    // Clear all app configuration
    sight::service::extension::AppConfig::getDefault()->clearRegistry();

    // Clear all app configuration parameters
    sight::service::extension::AppConfigParameters::getDefault()->clearRegistry();

    // Clear all service factories
    sight::service::extension::Factory::getDefault()->clearFactory();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::service
