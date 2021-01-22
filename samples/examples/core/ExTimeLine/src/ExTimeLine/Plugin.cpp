/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "ExTimeLine/Plugin.hpp"

#include "ExTimeLine/SConsumer.hpp"

#include <core/spyLog.hpp>

#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

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

    m_worker = ::fwThread::Worker::New();
    ::fwServices::registry::ActiveWorkers::getDefault()
    ->addWorker(::fwServices::registry::ActiveWorkers::s_DEFAULT_WORKER, m_worker);

    ::fwRuntime::Runtime* const rntm = ::fwRuntime::Runtime::getDefault();
    std::shared_ptr< ::fwRuntime::Module >    module = rntm->findModule( "ExTimeLine" );
    std::cout << module->getLibraryLocation() << std::endl;
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    m_worker->stop();
    m_worker.reset();
}

//------------------------------------------------------------------------------

} // namespace ExTimeLine.
