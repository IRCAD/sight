/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "memory_info.hpp"

#include <core/memory/buffer_manager.hpp>
#include <core/memory/tools/memory_monitor_tools.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>

#include <iostream>

namespace sight::module::debug::action
{

//------------------------------------------------------------------------------

memory_info::memory_info() noexcept =
    default;

//------------------------------------------------------------------------------

memory_info::~memory_info() noexcept =
    default;

//------------------------------------------------------------------------------

void memory_info::updating()
{
    // Memory information
    std::uint64_t mo                  = 1024LL * 1024;
    std::uint64_t total_system_memory = core::memory::tools::memory_monitor_tools::get_total_system_memory();
    std::uint64_t free_system_memory  = core::memory::tools::memory_monitor_tools::get_free_system_memory();
    std::uint64_t used_process_memory = core::memory::tools::memory_monitor_tools::get_used_process_memory();
    std::uint64_t estimate_free_mem   = core::memory::tools::memory_monitor_tools::estimate_free_mem();

    core::memory::buffer_manager::size_t managed_buffer_size = 0;
    core::memory::buffer_manager::size_t dumped_buffer_size  = 0;
    core::memory::buffer_manager::sptr manager               = core::memory::buffer_manager::get();
    if(manager)
    {
        core::memory::buffer_manager::buffer_stats stats = manager->get_buffer_stats().get();
        managed_buffer_size = stats.total_managed;
        dumped_buffer_size  = stats.total_dumped;
    }

    std::stringstream stream;
    stream << "Total system memory = " << total_system_memory / mo << " Mo" << std::endl;
    stream << "Free system memory  = " << free_system_memory / mo << " Mo" << std::endl;
    stream << "Used process memory = " << used_process_memory / mo << " Mo" << std::endl;
    stream << "Estimated Free memory = " << estimate_free_mem / mo << " Mo" << std::endl;
    stream << "ManagedBuffer size  = " << managed_buffer_size / mo << " Mo" << std::endl;
    stream << "DumpedBuffer size   = " << dumped_buffer_size / mo << " Mo" << std::endl;

    // Information message box
    sight::ui::dialog::message::show(
        "memory_info",
        stream.str(),
        sight::ui::dialog::message::info
    );
}

//------------------------------------------------------------------------------

void memory_info::configuring()
{
    this->sight::ui::action::initialize();
}

//------------------------------------------------------------------------------

void memory_info::starting()
{
    this->sight::ui::action::action_service_starting();
}

//------------------------------------------------------------------------------

void memory_info::stopping()
{
    this->sight::ui::action::action_service_stopping();
}

//------------------------------------------------------------------------------

} // namespace sight::module::debug::action
