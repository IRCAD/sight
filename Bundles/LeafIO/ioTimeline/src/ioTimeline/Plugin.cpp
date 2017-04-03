/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioTimeline/Plugin.hpp"

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/macros.hpp>

namespace ioTimeline
{
static fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::ioTimeline::Plugin");

Plugin::~Plugin() throw()
{
}

//------------------------------------------------------------------------------

void Plugin::start() throw(fwRuntime::RuntimeException)
{
}

//------------------------------------------------------------------------------

void Plugin::stop() throw()
{
}
} // namespace ioTimeline
