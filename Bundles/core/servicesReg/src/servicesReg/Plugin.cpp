/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwComEd/parser/Composite.hpp>

#include <fwServices/registry/ServiceConfig.hpp>
#include <fwServices/registry/ServiceFactory.hpp>
#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/GlobalEventManager.hpp>

#include "servicesReg/Plugin.hpp"

namespace servicesReg
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::servicesReg::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() throw()
{
    ::fwComEd::parser::Composite::sptr objectCreatedToForceLink;
}

//-----------------------------------------------------------------------------

void Plugin::start() throw( ::fwRuntime::RuntimeException )
{
    ::fwServices::registry::ServiceFactory::getDefault()->parseBundleInformation();
    ::fwServices::registry::ServiceConfig::getDefault()->parseBundleInformation();
    ::fwServices::registry::AppConfig::getDefault()->parseBundleInformation();
}

//-----------------------------------------------------------------------------

void Plugin::initialize() throw( ::fwRuntime::RuntimeException )
{}

//-----------------------------------------------------------------------------

void Plugin::uninitialize() throw( ::fwRuntime::RuntimeException )
{
    // Clear all messages
    ::fwServices::GlobalEventManager::getDefault()->clearMessages();

    // Clear all service configs
    ::fwServices::registry::ServiceConfig::getDefault()->clearRegistry();

    // Clear all app configuration
    ::fwServices::registry::AppConfig::getDefault()->clearRegistry();

    // Clear all service factories
    ::fwServices::registry::ServiceFactory::getDefault()->clearFactory();

    // Clear all factories before stop application.
    ::fwTools::ClassFactoryRegistry::getFactories().clear();
}

//-----------------------------------------------------------------------------

void Plugin::stop() throw()
{}

//-----------------------------------------------------------------------------

} // namespace servicesReg
