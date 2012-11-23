/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwCore/spyLog.hpp>

#include "Tuto15MultithreadCtrl/Plugin.hpp"

namespace Tuto15MultithreadCtrl
{

//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::Tuto15MultithreadCtrl::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() throw()
{}

//-----------------------------------------------------------------------------

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void Plugin::stop() throw()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void Plugin::initialize() throw( ::fwRuntime::RuntimeException )
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void Plugin::uninitialize() throw()
{
    SLM_TRACE_FUNC();
}

} // namespace Tuto15MultithreadCtrl
