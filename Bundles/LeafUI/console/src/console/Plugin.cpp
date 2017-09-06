/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "console/Plugin.hpp"

#include <fwRuntime/profile/Profile.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/registry/ActiveWorkers.hpp>

namespace console
{
//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::console::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{
    m_worker = ::fwThread::Worker::New();
    ::fwServices::registry::ActiveWorkers::setDefaultWorker(m_worker);

    ::fwRuntime::profile::getCurrentProfile()->setRunCallback(std::bind(&Plugin::run, this));
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    if(m_worker)
    {
        m_worker->stop();
    }
}

//-----------------------------------------------------------------------------

int Plugin::run() noexcept
{
    m_worker->post( []() {::fwRuntime::profile::getCurrentProfile()->setup(); } );
    m_worker->getFuture().wait(); // This is required to start WorkerAsio loop

    ::fwRuntime::profile::getCurrentProfile()->cleanup();
    const std::uint64_t result = ::boost::any_cast<std::uint64_t>(m_worker->getFuture().get());

    ::fwServices::registry::ActiveWorkers::getDefault()->clearRegistry();
    m_worker.reset();

    return result;
}

//-----------------------------------------------------------------------------

} // namespace console
