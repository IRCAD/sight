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

#include "Plugin.hpp"

#include "SConsumer.hpp"

#include <core/runtime/Runtime.hpp>
#include <core/spyLog.hpp>
#include <core/thread/ActiveWorkers.hpp>

#include <service/extension/Factory.hpp>

namespace ExTimeLine
{

//-----------------------------------------------------------------------------

SIGHT_REGISTER_PLUGIN("::ExTimeLine::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{

    m_worker = sight::core::thread::Worker::New();
    sight::core::thread::ActiveWorkers::getDefault()
    ->addWorker(sight::core::thread::ActiveWorkers::s_DEFAULT_WORKER, m_worker);
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    m_worker->stop();
    m_worker.reset();
}

//------------------------------------------------------------------------------

} // namespace ExTimeLine.
