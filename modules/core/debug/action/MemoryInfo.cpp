/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <core/memory/BufferManager.hpp>
#include <core/memory/tools/MemoryMonitorTools.hpp>

#include <services/macros.hpp>

#include <ui/base/dialog/MessageDialog.hpp>

#include <iostream>

namespace sight::modules::debug
{
namespace action
{

fwServicesRegisterMacro( ::sight::ui::base::IActionSrv, ::sight::modules::debug::action::MemoryInfo,
                         ::sight::data::Object )

//------------------------------------------------------------------------------

MemoryInfo::MemoryInfo( ) noexcept
{
}

//------------------------------------------------------------------------------

MemoryInfo::~MemoryInfo() noexcept
{
}

//------------------------------------------------------------------------------

void MemoryInfo::updating( )
{
    // Memory information
    std::uint64_t mo                = 1024*1024;
    std::uint64_t totalSystemMemory = core::memory::tools::MemoryMonitorTools::getTotalSystemMemory();
    std::uint64_t freeSystemMemory  = core::memory::tools::MemoryMonitorTools::getFreeSystemMemory();
    std::uint64_t usedProcessMemory = core::memory::tools::MemoryMonitorTools::getUsedProcessMemory();
    std::uint64_t estimateFreeMem   = core::memory::tools::MemoryMonitorTools::estimateFreeMem();

    core::memory::BufferManager::SizeType managedBufferSize = 0;
    core::memory::BufferManager::SizeType dumpedBufferSize  = 0;
    core::memory::BufferManager::sptr manager               = core::memory::BufferManager::getDefault();
    if( manager )
    {
        core::memory::BufferManager::BufferStats stats = manager->getBufferStats().get();
        managedBufferSize = stats.totalManaged;
        dumpedBufferSize  = stats.totalDumped;
    }
    std::stringstream stream;
    stream << "Total system memory = "  << totalSystemMemory/mo << " Mo" << std::endl;
    stream << "Free system memory  = "   << freeSystemMemory/mo  << " Mo" << std::endl;
    stream << "Used process memory = "  << usedProcessMemory/mo << " Mo" << std::endl;
    stream << "Estimed Free memory = "  << estimateFreeMem/mo << " Mo" << std::endl;
    stream << "ManagedBuffer size  = "  << managedBufferSize/mo << " Mo" << std::endl;
    stream << "DumpedBuffer size   = "  << dumpedBufferSize/mo << " Mo" << std::endl;

    // Information message box
    sight::ui::base::dialog::MessageDialog::show(
        "MemoryInfo",
        stream.str(),
        sight::ui::base::dialog::IMessageDialog::INFO);
}

//------------------------------------------------------------------------------

void MemoryInfo::configuring()
{
    this->sight::ui::base::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void MemoryInfo::starting()
{
    this->sight::ui::base::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void MemoryInfo::stopping()
{
    this->sight::ui::base::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace basicVisuCtrl
