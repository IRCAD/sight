/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#pragma once

#if defined(linux) || defined(__linux)

#include "core/config.hpp"

#include <cstdint>
#include <string>

namespace sight::core::memory
{

namespace tools
{

//------------------------------------------------------------------------------

struct MemInfo
{
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

struct Status
{
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

class CORE_CLASS_API PosixMemoryMonitorTools
{
public:

    CORE_API PosixMemoryMonitorTools();

    CORE_API ~PosixMemoryMonitorTools();

    CORE_API static std::uint64_t estimateFreeMem();

    CORE_API static void printProcessMemoryInformation();

    CORE_API static void printSystemMemoryInformation();

    CORE_API static void printMemoryInformation();

    CORE_API static std::uint64_t getTotalSystemMemory();

    CORE_API static std::uint64_t getUsedSystemMemory();

    CORE_API static std::uint64_t getFreeSystemMemory();

    CORE_API static std::uint64_t getUsedProcessMemory();

private:

    static std::uint64_t s_pageSize;
    static std::uint64_t s_totalMemory;

    /* Extract numbers from a string between the start and end indices */
    static std::uint64_t extract_number(char* str, int start, int end);

    /* Parse the contents of /proc/meminfo file into the meminfo structure */
    static void get_memory_stats(MemInfo& meminfo);

    static void printStatus(Status& stat);

    static void analyseMemInfo(std::string& line, MemInfo& meminfo);

    static void analyseStatusLine(std::string& line, Status& stat);

    static void getStatusOfPid(unsigned long pid, Status& stat);

    static void getAllStatus(Status& allStat);

    static void printAllStatus();
};

} // namespace tools

} // namespace sight::core::memory

#endif //defined(linux) || defined(__linux)
