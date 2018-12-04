/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "monitor/action/MemoryInfo.hpp"

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwMemory/BufferManager.hpp>
#include <fwMemory/tools/MemoryMonitorTools.hpp>

#include <fwServices/macros.hpp>

#include <iostream>

namespace monitor
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::monitor::action::MemoryInfo, ::fwData::Object );

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
    std::uint64_t totalSystemMemory = ::fwMemory::tools::MemoryMonitorTools::getTotalSystemMemory();
    std::uint64_t freeSystemMemory  = ::fwMemory::tools::MemoryMonitorTools::getFreeSystemMemory();
    std::uint64_t usedProcessMemory = ::fwMemory::tools::MemoryMonitorTools::getUsedProcessMemory();
    std::uint64_t estimateFreeMem   = ::fwMemory::tools::MemoryMonitorTools::estimateFreeMem();

    ::fwMemory::BufferManager::SizeType managedBufferSize = 0;
    ::fwMemory::BufferManager::SizeType dumpedBufferSize  = 0;
    ::fwMemory::BufferManager::sptr manager               = ::fwMemory::BufferManager::getDefault();
    if( manager )
    {
        ::fwMemory::BufferManager::BufferStats stats = manager->getBufferStats().get();
        managedBufferSize                            = stats.totalManaged;
        dumpedBufferSize                             = stats.totalDumped;
    }
    std::stringstream stream;
    stream << "Total system memory = "  << totalSystemMemory/mo << " Mo" << std::endl;
    stream << "Free system memory  = "   << freeSystemMemory/mo  << " Mo" << std::endl;
    stream << "Used process memory = "  << usedProcessMemory/mo << " Mo" << std::endl;
    stream << "Estimed Free memory = "  << estimateFreeMem/mo << " Mo" << std::endl;
    stream << "ManagedBuffer size  = "  << managedBufferSize/mo << " Mo" << std::endl;
    stream << "DumpedBuffer size   = "  << dumpedBufferSize/mo << " Mo" << std::endl;

    // Information message box
    ::fwGui::dialog::MessageDialog::showMessageDialog(
        "MemoryInfo",
        stream.str(),
        ::fwGui::dialog::IMessageDialog::INFO);
}

//------------------------------------------------------------------------------

void MemoryInfo::configuring()
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void MemoryInfo::starting()
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void MemoryInfo::stopping()
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace basicVisuCtrl
