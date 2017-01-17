/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEMORY_TOOLS_POSIXMEMORYMONITORTOOLS_HPP__
#define __FWMEMORY_TOOLS_POSIXMEMORYMONITORTOOLS_HPP__

#if defined(linux) || defined(__linux)

#include "fwMemory/config.hpp"

#include <cstdint>
#include <string>

namespace fwMemory
{
namespace tools
{

//------------------------------------------------------------------------------

struct MemInfo {

    std::uint64_t total;
    std::uint64_t free;
    std::uint64_t buffered;
    std::uint64_t cached;
    std::uint64_t swapcached;
    std::uint64_t swaptotal;
    std::uint64_t swapfree;

    MemInfo()
    {
        total      = 0;
        free       = 0;
        buffered   = 0;
        cached     = 0;
        swapcached = 0;
        swaptotal  = 0;
        swapfree   = 0;
    }

};

//------------------------------------------------------------------------------

struct Status {

    std::uint64_t VmPeak;
    std::uint64_t VmSize;
    std::uint64_t VmLck;
    std::uint64_t VmHWM;
    std::uint64_t VmRSS;
    std::uint64_t VmData;
    std::uint64_t VmStk;
    std::uint64_t VmExe;
    std::uint64_t VmLib;
    std::uint64_t VmPTE;

    Status()
    {
        VmPeak = 0;
        VmSize = 0;
        VmLck  = 0;
        VmHWM  = 0;
        VmRSS  = 0;
        VmData = 0;
        VmStk  = 0;
        VmExe  = 0;
        VmLib  = 0;
        VmPTE  = 0;
    }

};

//------------------------------------------------------------------------------

class FWMEMORY_CLASS_API PosixMemoryMonitorTools
{

public:

    FWMEMORY_API PosixMemoryMonitorTools();

    FWMEMORY_API ~PosixMemoryMonitorTools();

    FWMEMORY_API static std::uint64_t estimateFreeMem();

    FWMEMORY_API static void printProcessMemoryInformation();

    FWMEMORY_API static void printSystemMemoryInformation();

    FWMEMORY_API static void printMemoryInformation();

    FWMEMORY_API static std::uint64_t getTotalSystemMemory();

    FWMEMORY_API static std::uint64_t getUsedSystemMemory();

    FWMEMORY_API static std::uint64_t getFreeSystemMemory();

    FWMEMORY_API static std::uint64_t getUsedProcessMemory();

private:

    static std::uint64_t s_pageSize;
    static std::uint64_t s_totalMemory;

    /* Extract numbers from a string between the start and end indices */
    static std::uint64_t extract_number( char* str, int start, int end );

    /* Parse the contents of /proc/meminfo file into the meminfo structure */
    static void get_memory_stats( MemInfo& meminfo );

    static void printStatus( Status& stat );

    static void analyseMemInfo( std::string& line, MemInfo& meminfo );

    static void analyseStatusLine( std::string& line, Status& stat );

    static void getStatusOfPid( int pid, Status& stat);

    static void getAllStatus(Status& allStat);

    static void printAllStatus();
};

} // namespace tools
} // namespace fwMemory

#endif //defined(linux) || defined(__linux)

#endif // __FWMEMORY_TOOLS_POSIXMEMORYMONITORTOOLS_HPP__
