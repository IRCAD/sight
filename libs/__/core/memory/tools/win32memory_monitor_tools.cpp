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

#ifdef _WIN32

#include "core/memory/tools/win32memory_monitor_tools.hpp"

#include <core/base.hpp>

// Do not change this include order, windows.h is required by psapi.h
#include <windows.h>
#include <psapi.h>

namespace sight::core::memory
{

namespace tools
{

//-----------------------------------------------------------------------------

win32memory_monitor_tools::win32memory_monitor_tools()
{
}

//-----------------------------------------------------------------------------

win32memory_monitor_tools::~win32memory_monitor_tools()
{
}

//-----------------------------------------------------------------------------

std::uint64_t win32memory_monitor_tools::estimate_free_mem()
{
    std::uint64_t free_memory = 0;
#ifdef _M_X64
    free_memory = get_free_system_memory();
#else
    std::uint64_t windows_limit_for_process = 1.2 * 1024 * 1024 * 1024; // 1.5 Go
    std::uint64_t free_mem                  = std::min(
        get_free_system_memory(),
        windows_limit_for_process - get_used_process_memory()
    );
    free_memory = std::max((std::uint64_t) 0, free_mem);
#endif
    return free_memory;
}

//-----------------------------------------------------------------------------

void win32memory_monitor_tools::print_process_memory_information()
{
    DWORD process_id = GetCurrentProcessId();
    HANDLE h_process;

    h_process = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
        FALSE,
        process_id
    );

    if(NULL != h_process)
    {
        // Get the process name.
        int name_size = 100;
        char* name    = new char [name_size];

        HMODULE h_mod;
        DWORD cb_needed;

        SIGHT_INFO("-- Process memory information --");
        if(EnumProcessModules(h_process, &h_mod, sizeof(h_mod), &cb_needed))
        {
            GetModuleBaseNameW(h_process, h_mod, (LPWSTR) name, name_size);
            SIGHT_INFO("    Name : " << name << "  ( PID : " << process_id << " )");
        }
        else
        {
            SIGHT_INFO("    Name : not found ( PID : " << process_id << " )");
        }

        PROCESS_MEMORY_COUNTERS_EX pmc;
        if(GetProcessMemoryInfo(h_process, reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc)))
        {
            SIGHT_INFO("    PageFaultCount : " << (int) pmc.PageFaultCount);
            SIGHT_INFO("    PeakWorkingSetSize : " << (int) pmc.PeakWorkingSetSize);
            SIGHT_INFO("    WorkingSetSize : " << (int) pmc.WorkingSetSize);
            SIGHT_INFO("    QuotaPeakPagedPoolUsage : " << (int) pmc.QuotaPeakPagedPoolUsage);
            SIGHT_INFO("    QuotaPagedPoolUsage : " << (int) pmc.QuotaPagedPoolUsage);
            SIGHT_INFO("    QuotaPeakNonPagedPoolUsage : " << (int) pmc.QuotaPeakNonPagedPoolUsage);
            SIGHT_INFO("    QuotaNonPagedPoolUsage : " << (int) pmc.QuotaNonPagedPoolUsage);
            SIGHT_INFO("    PagefileUsage : " << (int) pmc.PagefileUsage);
            SIGHT_INFO("    PeakPagefileUsage : " << (int) pmc.PeakPagefileUsage);
            SIGHT_INFO("    PrivateUsage : " << (int) pmc.PrivateUsage);
        }
        else
        {
            SIGHT_WARN("GetProcessMemoryInfo failed !");
        }

        delete[] name;
        CloseHandle(h_process);
    }
}

//-----------------------------------------------------------------------------

void win32memory_monitor_tools::print_system_memory_information()
{
    std::uint64_t o_to_ko = 1024;

    MEMORYSTATUSEX statex;

    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);

    SIGHT_INFO("-- System memory information --");
    SIGHT_INFO("    There is " << statex.dwMemoryLoad << " percent of memory in use.");
    SIGHT_INFO("    There are " << statex.ullTotalPhys / o_to_ko << " total Ko of physical memory.");
    SIGHT_INFO("    There are " << statex.ullAvailPhys / o_to_ko << " free Ko of physical memory.");
    SIGHT_INFO("    There are " << statex.ullTotalPageFile / o_to_ko << " total Ko of paging file.");
    SIGHT_INFO("    There are " << statex.ullAvailPageFile / o_to_ko << " free Ko of paging file.");
    SIGHT_INFO("    There are " << statex.ullTotalVirtual / o_to_ko << " total Ko of virtual memory.");
    SIGHT_INFO("    There are " << statex.ullAvailVirtual / o_to_ko << " free Ko of virtual memory.");
}

//-----------------------------------------------------------------------------

void win32memory_monitor_tools::print_memory_information()
{
    print_system_memory_information();
    print_process_memory_information();
}

//-----------------------------------------------------------------------------

std::uint64_t win32memory_monitor_tools::get_total_system_memory()
{
    MEMORYSTATUSEX statex;

    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);

    return statex.ullTotalPhys;
}

//-----------------------------------------------------------------------------

std::uint64_t win32memory_monitor_tools::get_used_system_memory()
{
    MEMORYSTATUSEX statex;

    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);

    return statex.ullTotalPhys - statex.ullAvailPhys;
}

//-----------------------------------------------------------------------------

std::uint64_t win32memory_monitor_tools::get_free_system_memory()
{
    MEMORYSTATUSEX statex;

    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);

    return statex.ullAvailPhys;
}

//-----------------------------------------------------------------------------

std::uint64_t win32memory_monitor_tools::get_used_process_memory()
{
    std::uint64_t memory = 0;

    BOOL result;
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if(result =
           GetProcessMemoryInfo(
               GetCurrentProcess(),
               reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc),
               sizeof(pmc)
           );
       result)
    {
        memory = pmc.WorkingSetSize;
    }

    SIGHT_WARN_IF("GetProcessMemoryInfo failed !", !result);

    return memory;
}

//-----------------------------------------------------------------------------

} // namespace tools

} // namespace sight::core::memory

#endif // _WIN32
