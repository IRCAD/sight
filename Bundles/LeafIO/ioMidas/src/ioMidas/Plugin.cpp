/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwServices/macros.hpp>

#include "ioMidas/Plugin.hpp"

namespace ioMidas
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("ioMidas::Plugin");

//-----------------------------------------------------------------------------------------------------

Plugin::~Plugin() throw()
{}

//-----------------------------------------------------------------------------------------------------

void Plugin::start() throw(::fwRuntime::RuntimeException)
{}

//-----------------------------------------------------------------------------------------------------

void Plugin::stop() throw()
{}

} // namespace ioMidas

