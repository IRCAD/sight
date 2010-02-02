/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwComEd/ImageMsg.hpp>

#include "servicesReg/Plugin.hpp"

namespace servicesReg
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::servicesReg::Plugin");

Plugin::~Plugin() throw()
{
    //Hack: force link with fwServices
    ::fwServices::ObjectServiceRegistry::getDefault();
    //Hack: force link with fwcomEd
    ::fwComEd::ImageMsg im;
}

void Plugin::start() throw( ::fwRuntime::RuntimeException )
{}

void Plugin::stop() throw()
{}

} // namespace servicesReg
