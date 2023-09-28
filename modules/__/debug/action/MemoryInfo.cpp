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

#include "MemoryInfo.hpp"

#include <core/memory/buffer_manager.hpp>
#include <core/memory/tools/memory_monitor_tools.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>

#include <iostream>

namespace sight::module::debug::action
{

//------------------------------------------------------------------------------

MemoryInfo::MemoryInfo() noexcept =
    default;

//------------------------------------------------------------------------------

MemoryInfo::~MemoryInfo() noexcept =
    default;

//------------------------------------------------------------------------------

void MemoryInfo::updating()
{
    // Memory information
    std::uint64_t mo                = 1024LL * 1024;
    std::uint64_t totalSystemMemory = core::memory::tools::memory_monitor_tools::get_total_system_memory();
    std::uint64_t freeSystemMemory  = core::memory::tools::memory_monitor_tools::get_free_system_memory();
    std::uint64_t usedProcessMemory = core::memory::tools::memory_monitor_tools::get_used_process_memory();
    std::uint64_t estimateFreeMem   = core::memory::tools::memory_monitor_tools::estimate_free_mem();

    core::memory::buffer_manager::size_t managedBufferSize = 0;
    core::memory::buffer_manager::size_t dumpedBufferSize  = 0;
    core::memory::buffer_manager::sptr manager             = core::memory::buffer_manager::get();
    if(manager)
    {
        core::memory::buffer_manager::buffer_stats stats = manager->get_buffer_stats().get();
        managedBufferSize = stats.total_managed;
        dumpedBufferSize  = stats.total_dumped;
    }

    std::stringstream stream;
    stream << "Total system memory = " << totalSystemMemory / mo << " Mo" << std::endl;
    stream << "Free system memory  = " << freeSystemMemory / mo << " Mo" << std::endl;
    stream << "Used process memory = " << usedProcessMemory / mo << " Mo" << std::endl;
    stream << "Estimated Free memory = " << estimateFreeMem / mo << " Mo" << std::endl;
    stream << "ManagedBuffer size  = " << managedBufferSize / mo << " Mo" << std::endl;
    stream << "DumpedBuffer size   = " << dumpedBufferSize / mo << " Mo" << std::endl;

    // Information message box
    sight::ui::dialog::message::show(
        "MemoryInfo",
        stream.str(),
        sight::ui::dialog::message::INFO
    );
}

//------------------------------------------------------------------------------

void MemoryInfo::configuring()
{
    this->sight::ui::action::initialize();
}

//------------------------------------------------------------------------------

void MemoryInfo::starting()
{
    this->sight::ui::action::actionServiceStarting();
}

//------------------------------------------------------------------------------

void MemoryInfo::stopping()
{
    this->sight::ui::action::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace sight::module::debug::action
