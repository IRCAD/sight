/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwTools/Object.hpp>
#include <fwRuntime/profile/Profile.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwCore/spyLog.hpp>

#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/ServiceFactory.hpp>


#include "Ex02TimeLine/Plugin.hpp"
#include "Ex02TimeLine/SConsumer.hpp"

namespace Ex02TimeLine
{

//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::Ex02TimeLine::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{
    SLM_TRACE_FUNC();

    ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();
    ::fwServices::registry::ActiveWorkers::getDefault()
    ->addWorker(::fwServices::registry::ActiveWorkers::s_DEFAULT_WORKER, worker);
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

} // namespace Ex02TimeLine
