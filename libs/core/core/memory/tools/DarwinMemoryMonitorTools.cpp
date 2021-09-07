/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#ifdef __APPLE__

#include "core/memory/tools/DarwinMemoryMonitorTools.hpp"

#include <core/base.hpp>

#include <mach/mach.h>
#include <mach/mach_host.h>
#include <mach/mach_init.h>
#include <mach/mach_types.h>
#include <mach/vm_statistics.h>

#include <sys/sysctl.h>
#include <sys/types.h>

#include <iomanip>
#include <iostream>

namespace sight::core::memory
{

namespace tools
{

//-----------------------------------------------------------------------------

DarwinMemoryMonitorTools::DarwinMemoryMonitorTools()
{
}

//-----------------------------------------------------------------------------

DarwinMemoryMonitorTools::~DarwinMemoryMonitorTools()
{
}

//-----------------------------------------------------------------------------

std::uint64_t DarwinMemoryMonitorTools::estimateFreeMem()
{
    std::uint64_t freeMemory = 0;

    freeMemory = getFreeSystemMemory();

#ifndef __LP64__
    struct task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

    if(KERN_SUCCESS != task_info(
           mach_task_self(),
           TASK_BASIC_INFO,
           (task_info_t) &t_info,
           &t_info_count
    ))
    {
        SIGHT_ASSERT("Failed to retrieve used process memory information", 0);
        return 0;
    }

    // Hard coded 3Gb limit for 32bit process
    const std::uint64_t maxMemory         = 3221225472LL; // 3 Go
    const std::uint64_t usedProcessMemory = getUsedProcessMemory();
    freeMemory = std::min(maxMemory - usedProcessMemory, freeMemory);
    const std::uint64_t maxVMemory = 4294967296LL; // 4 Go
    freeMemory = std::min(maxVMemory - t_info.virtual_size, freeMemory);
#endif

    return freeMemory;
}

//-----------------------------------------------------------------------------

void DarwinMemoryMonitorTools::printProcessMemoryInformation()
{
}

//-----------------------------------------------------------------------------

void DarwinMemoryMonitorTools::printSystemMemoryInformation()
{
}

//-----------------------------------------------------------------------------

void DarwinMemoryMonitorTools::printMemoryInformation()
{
    printSystemMemoryInformation();
    printProcessMemoryInformation();
}

//-----------------------------------------------------------------------------

std::uint64_t DarwinMemoryMonitorTools::getTotalSystemMemory()
{
    static int64_t physical_memory = 0;

    if(physical_memory == 0)
    {
        int mib[2];
        mib[0] = CTL_HW;
        mib[1] = HW_MEMSIZE;
        size_t length = sizeof(int64_t);
        sysctl(mib, 2, &physical_memory, &length, NULL, 0);
    }

    return physical_memory;
}

//-----------------------------------------------------------------------------

std::uint64_t DarwinMemoryMonitorTools::getUsedSystemMemory()
{
    vm_size_t page_size;
    mach_port_t mach_port;
    mach_msg_type_number_t count;
    vm_statistics_data_t vm_stats;

    mach_port = mach_host_self();
    count     = sizeof(vm_stats) / sizeof(natural_t);
    if(KERN_SUCCESS == host_page_size(mach_port, &page_size)
       && KERN_SUCCESS == host_statistics(
           mach_port,
           HOST_VM_INFO,
           (host_info_t) &vm_stats,
           &count
       )
    )
    {
        uint64_t used_memory = (
            (int64_t) vm_stats.active_count
            + (int64_t) vm_stats.wire_count
                               ) * (int64_t) page_size;

        return used_memory;
    }

    SIGHT_ASSERT("Failed to retrieve used system memory information", 0);
    return 0;
}

//-----------------------------------------------------------------------------

std::uint64_t DarwinMemoryMonitorTools::getFreeSystemMemory()
{
    vm_size_t page_size;
    mach_port_t mach_port;
    mach_msg_type_number_t count;
    vm_statistics_data_t vm_stats;

    mach_port = mach_host_self();
    count     = sizeof(vm_stats) / sizeof(natural_t);
    if(KERN_SUCCESS == host_page_size(mach_port, &page_size)
       && KERN_SUCCESS == host_statistics(
           mach_port,
           HOST_VM_INFO,
           (host_info_t) &vm_stats,
           &count
       )
    )
    {
        uint64_t freeMemory = (
            (int64_t) vm_stats.free_count
            + (int64_t) vm_stats.inactive_count
                              ) * (int64_t) page_size;
        return freeMemory;
    }

    SIGHT_ASSERT("Failed to retrieve free system memory information", 0);
    return 0;
}

//-----------------------------------------------------------------------------

std::uint64_t DarwinMemoryMonitorTools::getUsedProcessMemory()
{
    struct task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

    if(KERN_SUCCESS != task_info(
           mach_task_self(),
           TASK_BASIC_INFO,
           (task_info_t) &t_info,
           &t_info_count
    ))
    {
        SIGHT_ASSERT("Failed to retrieve used process memory information", 0);
        return 0;
    }

    return t_info.resident_size;
}

//-----------------------------------------------------------------------------

} // namespace tools

} // namespace sight::core::memory

#endif // ifdef __APPLE__
