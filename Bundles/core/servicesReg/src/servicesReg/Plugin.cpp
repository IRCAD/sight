/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "servicesReg/Plugin.hpp"

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/registry/AppConfigParameters.hpp>
#include <fwServices/registry/ServiceConfig.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

#include <fwThread/Pool.hpp>

namespace servicesReg
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::servicesReg::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{
    ::fwServices::registry::ServiceFactory::getDefault()->parseBundleInformation();
    ::fwServices::registry::ServiceConfig::getDefault()->parseBundleInformation();
    ::fwServices::registry::AppConfig::getDefault()->parseBundleInformation();
    ::fwServices::registry::AppConfigParameters::getDefault()->parseBundleInformation();

    ::fwThread::createDefaultPool();
}

//-----------------------------------------------------------------------------

void Plugin::initialize()
{
}

//-----------------------------------------------------------------------------

void Plugin::uninitialize()
{
    ::fwThread::deleteDefaultPool();

    // Clear all service configs
    ::fwServices::registry::ServiceConfig::getDefault()->clearRegistry();

    // Clear all app configuration
    ::fwServices::registry::AppConfig::getDefault()->clearRegistry();

    // Clear all app configuration parameters
    ::fwServices::registry::AppConfigParameters::getDefault()->clearRegistry();

    // Clear all service factories
    ::fwServices::registry::ServiceFactory::getDefault()->clearFactory();

    // Clear all active Workers
    ::fwServices::registry::ActiveWorkers::getDefault()->clearRegistry();
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//-----------------------------------------------------------------------------

} // namespace servicesReg
