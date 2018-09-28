/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ExTimeLine/Plugin.hpp"

#include "ExTimeLine/SConsumer.hpp"

#include <fwCore/spyLog.hpp>

#include <fwRuntime/profile/Profile.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

#include <fwTools/Object.hpp>

namespace ExTimeLine
{

//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::ExTimeLine::Plugin");

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

} // namespace ExTimeLine
