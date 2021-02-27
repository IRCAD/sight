/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "Plugin.hpp"

#include <core/runtime/profile/Profile.hpp>
#include <core/runtime/utils/GenericExecutableFactoryRegistry.hpp>
#include <core/thread/ActiveWorkers.hpp>
#include <core/thread/Worker.hpp>
#include <core/thread/Worker.hxx>

namespace sight::module::ui::console
{
//-----------------------------------------------------------------------------

static core::runtime::utils::GenericExecutableFactoryRegistry<Plugin>  registry(
    "::sight::module::ui::console::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{
    m_worker = core::thread::Worker::New();
    core::thread::ActiveWorkers::setDefaultWorker(m_worker);

    core::runtime::getCurrentProfile()->setRunCallback(std::bind(&Plugin::run, this));
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
    m_worker->post( []() {core::runtime::getCurrentProfile()->setup(); } );
    m_worker->getFuture().wait(); // This is required to start WorkerAsio loop

    core::runtime::getCurrentProfile()->cleanup();
    const std::uint64_t result = std::any_cast<std::uint64_t>(m_worker->getFuture().get());

    core::thread::ActiveWorkers::getDefault()->clearRegistry();
    m_worker.reset();

    return result;
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::console
