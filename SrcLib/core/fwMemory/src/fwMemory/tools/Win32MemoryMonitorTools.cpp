/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <assert.h>
#include <iomanip>
#include <iostream>

#include <boost/regex.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwCore/base.hpp>

#include "fwMemory/MemoryMonitor.hpp"
#include "fwMemory/tools/Win32MemoryMonitorTools.hpp"

#ifdef _WIN32

#include <windows.h>
#include <stdio.h>
#include <psapi.h>
#include <tchar.h>


namespace fwMemory
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

::boost::uint64_t Win32MemoryMonitorTools::estimateFreeMem()
{
    ::boost::uint64_t freeMemory = 0;
#ifdef _M_X64
    freeMemory = getFreeSystemMemory();
#else
    ::boost::uint64_t windowsLimitForAProcess = 1.2 * 1024 * 1024 * 1024; // 1.5 Go
    ::boost::uint64_t freeMem = std::min(getFreeSystemMemory(), windowsLimitForAProcess - getUsedProcessMemory());
    freeMemory = std::max((::boost::uint64_t) 0, freeMem );
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
        char * name = new char[nameSize];

        HMODULE hMod;
        DWORD cbNeeded;

        SLM_INFO( "-- Process memory information --" );
        if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
        {
            GetModuleBaseNameW( hProcess, hMod, (LPWSTR)name, nameSize );
            OSLM_INFO( "    Name : " << name << "  ( PID : " << processID << " )");
        }
        else
        {
            OSLM_INFO( "    Name : not found ( PID : " << processID << " )");
        }

        PROCESS_MEMORY_COUNTERS_EX pmc;
        if ( GetProcessMemoryInfo( hProcess, reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc)) )
        {
            OSLM_INFO("    PageFaultCount : "               << (int)pmc.PageFaultCount );
            OSLM_INFO("    PeakWorkingSetSize : "           << (int)pmc.PeakWorkingSetSize );
            OSLM_INFO("    WorkingSetSize : "               << (int)pmc.WorkingSetSize );
            OSLM_INFO("    QuotaPeakPagedPoolUsage : "      << (int)pmc.QuotaPeakPagedPoolUsage );
            OSLM_INFO("    QuotaPagedPoolUsage : "          << (int)pmc.QuotaPagedPoolUsage );
            OSLM_INFO("    QuotaPeakNonPagedPoolUsage : "   << (int)pmc.QuotaPeakNonPagedPoolUsage );
            OSLM_INFO("    QuotaNonPagedPoolUsage : "       << (int)pmc.QuotaNonPagedPoolUsage );
            OSLM_INFO("    PagefileUsage : "                << (int)pmc.PagefileUsage );
            OSLM_INFO("    PeakPagefileUsage : "            << (int)pmc.PeakPagefileUsage );
            OSLM_INFO("    PrivateUsage : "                 << (int)pmc.PrivateUsage );
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
    ::boost::uint64_t oToKo = 1024;

    MEMORYSTATUSEX statex;

    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);

    SLM_INFO( "-- System memory information --" );
    OSLM_INFO( "    There is " << statex.dwMemoryLoad               <<  " percent of memory in use." );
    OSLM_INFO( "    There are " << statex.ullTotalPhys/oToKo        <<  " total Ko of physical memory." );
    OSLM_INFO( "    There are " << statex.ullAvailPhys/oToKo        <<  " free Ko of physical memory." );
    OSLM_INFO( "    There are " << statex.ullTotalPageFile/oToKo    <<  " total Ko of paging file." );
    OSLM_INFO( "    There are " << statex.ullAvailPageFile/oToKo    <<  " free Ko of paging file." );
    OSLM_INFO( "    There are " << statex.ullTotalVirtual/oToKo     <<  " total Ko of virtual memory." );
    OSLM_INFO( "    There are " << statex.ullAvailVirtual/oToKo     <<  " free Ko of virtual memory." );
}

//-----------------------------------------------------------------------------

void Win32MemoryMonitorTools::printMemoryInformation()
{
    printSystemMemoryInformation();
    printProcessMemoryInformation();
}

//-----------------------------------------------------------------------------

::boost::uint64_t Win32MemoryMonitorTools::getTotalSystemMemory()
{
    MEMORYSTATUSEX statex;

    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);

    return statex.ullTotalPhys;
}

//-----------------------------------------------------------------------------

::boost::uint64_t Win32MemoryMonitorTools::getUsedSystemMemory()
{
    MEMORYSTATUSEX statex;

    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);

    return statex.ullTotalPhys - statex.ullAvailPhys;
}

//-----------------------------------------------------------------------------

::boost::uint64_t Win32MemoryMonitorTools::getFreeSystemMemory()
{
    MEMORYSTATUSEX statex;

    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);

    return statex.ullAvailPhys;
}

//-----------------------------------------------------------------------------

::boost::uint64_t Win32MemoryMonitorTools::getUsedProcessMemory()
{
    ::boost::uint64_t memory = 0;

    BOOL result;
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if ( result = GetProcessMemoryInfo( GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc)) )
    {
        memory = pmc.WorkingSetSize;
    }
    SLM_WARN_IF("GetProcessMemoryInfo failed !", !result);

    return memory;
}

//-----------------------------------------------------------------------------

} // namespace tools
} // namespace fwMemory

#endif
