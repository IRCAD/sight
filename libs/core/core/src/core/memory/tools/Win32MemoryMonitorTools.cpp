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

#ifdef _WIN32

#include "core/memory/tools/Win32MemoryMonitorTools.hpp"

#include <core/base.hpp>

// Do not change this include order, windows.h is required by psapi.h
#include <windows.h>
#include <psapi.h>

namespace sight::core::memory
{
namespace tools
{

//-----------------------------------------------------------------------------

Win32MemoryMonitorTools::Win32MemoryMonitorTools()
{
}

//-----------------------------------------------------------------------------

Win32MemoryMonitorTools::~Win32MemoryMonitorTools()
{
}

//-----------------------------------------------------------------------------

std::uint64_t Win32MemoryMonitorTools::estimateFreeMem()
{
    std::uint64_t freeMemory = 0;
#ifdef _M_X64
    freeMemory = getFreeSystemMemory();
#else
    std::uint64_t windowsLimitForAProcess = 1.2 * 1024 * 1024 * 1024; // 1.5 Go
    std::uint64_t freeMem                 = std::min(
        getFreeSystemMemory(), windowsLimitForAProcess - getUsedProcessMemory());
    freeMemory = std::max((std::uint64_t) 0, freeMem );
#endif
    return freeMemory;
}

//-----------------------------------------------------------------------------

void Win32MemoryMonitorTools::printProcessMemoryInformation()
{
    DWORD processID = GetCurrentProcessId();
    HANDLE hProcess;

    hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |PROCESS_VM_READ,
                            FALSE,
                            processID );

    if ( NULL != hProcess )
    {
        // Get the process name.
        int nameSize = 100;
        char* name   = new char[nameSize];

        HMODULE hMod;
        DWORD cbNeeded;

        SLM_INFO( "-- Process memory information --" );
        if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
        {
            GetModuleBaseNameW( hProcess, hMod, (LPWSTR)name, nameSize );
            SLM_INFO( "    Name : " << name << "  ( PID : " << processID << " )");
        }
        else
        {
            SLM_INFO( "    Name : not found ( PID : " << processID << " )");
        }

        PROCESS_MEMORY_COUNTERS_EX pmc;
        if ( GetProcessMemoryInfo( hProcess, reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc)) )
        {
            SLM_INFO("    PageFaultCount : " << (int)pmc.PageFaultCount );
            SLM_INFO("    PeakWorkingSetSize : " << (int)pmc.PeakWorkingSetSize );
            SLM_INFO("    WorkingSetSize : " << (int)pmc.WorkingSetSize );
            SLM_INFO("    QuotaPeakPagedPoolUsage : " << (int)pmc.QuotaPeakPagedPoolUsage );
            SLM_INFO("    QuotaPagedPoolUsage : " << (int)pmc.QuotaPagedPoolUsage );
            SLM_INFO("    QuotaPeakNonPagedPoolUsage : " << (int)pmc.QuotaPeakNonPagedPoolUsage );
            SLM_INFO("    QuotaNonPagedPoolUsage : " << (int)pmc.QuotaNonPagedPoolUsage );
            SLM_INFO("    PagefileUsage : " << (int)pmc.PagefileUsage );
            SLM_INFO("    PeakPagefileUsage : " << (int)pmc.PeakPagefileUsage );
            SLM_INFO("    PrivateUsage : " << (int)pmc.PrivateUsage );
        }
        else
        {
            SLM_WARN("GetProcessMemoryInfo failed !");
        }

        delete[] name;
        CloseHandle( hProcess );
    }
}

//-----------------------------------------------------------------------------

void Win32MemoryMonitorTools::printSystemMemoryInformation()
{
    std::uint64_t oToKo = 1024;

    MEMORYSTATUSEX statex;

    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx(&statex);

    SLM_INFO( "-- System memory information --" );
    SLM_INFO( "    There is " << statex.dwMemoryLoad               <<  " percent of memory in use." );
    SLM_INFO( "    There are " << statex.ullTotalPhys/oToKo        <<  " total Ko of physical memory." );
    SLM_INFO( "    There are " << statex.ullAvailPhys/oToKo        <<  " free Ko of physical memory." );
    SLM_INFO( "    There are " << statex.ullTotalPageFile/oToKo    <<  " total Ko of paging file." );
    SLM_INFO( "    There are " << statex.ullAvailPageFile/oToKo    <<  " free Ko of paging file." );
    SLM_INFO( "    There are " << statex.ullTotalVirtual/oToKo     <<  " total Ko of virtual memory." );
    SLM_INFO( "    There are " << statex.ullAvailVirtual/oToKo     <<  " free Ko of virtual memory." );
}

//-----------------------------------------------------------------------------

void Win32MemoryMonitorTools::printMemoryInformation()
{
    printSystemMemoryInformation();
    printProcessMemoryInformation();
}

//-----------------------------------------------------------------------------

std::uint64_t Win32MemoryMonitorTools::getTotalSystemMemory()
{
    MEMORYSTATUSEX statex;

    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx(&statex);

    return statex.ullTotalPhys;
}

//-----------------------------------------------------------------------------

std::uint64_t Win32MemoryMonitorTools::getUsedSystemMemory()
{
    MEMORYSTATUSEX statex;

    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx(&statex);

    return statex.ullTotalPhys - statex.ullAvailPhys;
}

//-----------------------------------------------------------------------------

std::uint64_t Win32MemoryMonitorTools::getFreeSystemMemory()
{
    MEMORYSTATUSEX statex;

    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx(&statex);

    return statex.ullAvailPhys;
}

//-----------------------------------------------------------------------------

std::uint64_t Win32MemoryMonitorTools::getUsedProcessMemory()
{
    std::uint64_t memory = 0;

    BOOL result;
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if ( result =
             GetProcessMemoryInfo( GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc)) )
    {
        memory = pmc.WorkingSetSize;
    }
    SLM_WARN_IF("GetProcessMemoryInfo failed !", !result);

    return memory;
}

//-----------------------------------------------------------------------------

} // namespace tools
} // namespace sight::core::memory

#endif // _WIN32
