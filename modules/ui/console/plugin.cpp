/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "plugin.hpp"

#include <core/runtime/profile/profile.hpp>
#include <core/thread/worker.hpp>
#include <core/thread/worker.hxx>

namespace sight::module::ui::console
{

//-----------------------------------------------------------------------------

SIGHT_REGISTER_PLUGIN("sight::module::ui::console::plugin");

//-----------------------------------------------------------------------------

plugin::~plugin() noexcept =
    default;

//-----------------------------------------------------------------------------

void plugin::start()
{
    core::runtime::get_current_profile()->set_run_callback(run);
}

//-----------------------------------------------------------------------------

void plugin::stop() noexcept
{
}

//-----------------------------------------------------------------------------

int plugin::run() noexcept
{
    auto worker = core::thread::get_default_worker();
    worker->get_future().wait(); // This is required to start WorkerAsio loop

    const int result = std::any_cast<int>(worker->get_future().get());

    return result;
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::console
