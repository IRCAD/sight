/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include "monitor/installSIGSEVBacktrace.hpp"
#include "monitor/Plugin.hpp"

namespace monitor
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::monitor::Plugin");

//------------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
    SLM_TRACE_FUNC();

    installSIGSEVBacktrace();
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//------------------------------------------------------------------------------

} // namespace monitor
