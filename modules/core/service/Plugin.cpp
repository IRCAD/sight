/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <core/runtime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <core/thread/Pool.hpp>

#include <service/registry/ActiveWorkers.hpp>
#include <service/registry/AppConfig.hpp>
#include <service/registry/AppConfigParameters.hpp>
#include <service/registry/ServiceConfig.hpp>
#include <service/registry/ServiceFactory.hpp>

namespace sight::modules::service
{

static core::runtime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::sight::modules::service::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{
    sight::service::registry::ServiceFactory::getDefault()->parseBundleInformation();
    sight::service::registry::ServiceConfig::getDefault()->parseBundleInformation();
    sight::service::registry::AppConfig::getDefault()->parseBundleInformation();
    sight::service::registry::AppConfigParameters::getDefault()->parseBundleInformation();
}

//-----------------------------------------------------------------------------

void Plugin::initialize()
{
}

//-----------------------------------------------------------------------------

void Plugin::uninitialize()
{
    // Clear all service configs
    sight::service::registry::ServiceConfig::getDefault()->clearRegistry();

    // Clear all app configuration
    sight::service::registry::AppConfig::getDefault()->clearRegistry();

    // Clear all app configuration parameters
    sight::service::registry::AppConfigParameters::getDefault()->clearRegistry();

    // Clear all service factories
    sight::service::registry::ServiceFactory::getDefault()->clearFactory();

    // Clear all active Workers
    sight::service::registry::ActiveWorkers::getDefault()->clearRegistry();
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//-----------------------------------------------------------------------------

} // namespace sight::modules::service
