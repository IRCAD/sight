/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "monitorQt/Plugin.hpp"

#include <fwRuntime/helper.hpp>
#include <fwRuntime/operations.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

namespace monitorQt
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::monitorQt::Plugin");

//------------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//------------------------------------------------------------------------------

} // namespace monitorQt
