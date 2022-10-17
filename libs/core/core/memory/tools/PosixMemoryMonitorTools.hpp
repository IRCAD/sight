/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

namespace sight::core::memory::tools
{

//------------------------------------------------------------------------------

struct MemInfo
{
    std::uint64_t total {0};
    std::uint64_t free {0};
    std::uint64_t buffered {0};
    std::uint64_t cached {0};
    std::uint64_t swapcached {0};
    std::uint64_t swaptotal {0};
    std::uint64_t swapfree {0};
};

//------------------------------------------------------------------------------

struct Status
{
    std::uint64_t VmPeak {0};
    std::uint64_t VmSize {0};
    std::uint64_t VmLck {0};
    std::uint64_t VmHWM {0};
    std::uint64_t VmRSS {0};
    std::uint64_t VmData {0};
    std::uint64_t VmStk {0};
    std::uint64_t VmExe {0};
    std::uint64_t VmLib {0};
    std::uint64_t VmPTE {0};
};

//------------------------------------------------------------------------------

class CORE_CLASS_API PosixMemoryMonitorTools
{
public:

    CORE_API PosixMemoryMonitorTools();

    CORE_API ~PosixMemoryMonitorTools() = default;

    CORE_API static std::uint64_t estimateFreeMem();

    CORE_API static void printProcessMemoryInformation();

    CORE_API static void printSystemMemoryInformation();

    CORE_API static void printMemoryInformation();

    CORE_API static std::uint64_t getTotalSystemMemory();

    CORE_API static std::uint64_t getUsedSystemMemory();

    CORE_API static std::uint64_t getFreeSystemMemory();

    CORE_API static std::uint64_t getUsedProcessMemory();

private:

    static std::int64_t s_pageSize;
    static std::int64_t s_totalMemory;

    /* Extract numbers from a string between the start and end indices */
    static std::uint64_t extract_number(char* str, int start, int end);

    /* Parse the contents of /proc/meminfo file into the meminfo structure */
    static void get_memory_stats(MemInfo& meminfo);

    static void printStatus(Status& stat);

    static void analyseMemInfo(std::string& line, MemInfo& meminfo);

    static void analyseStatusLine(std::string& line, Status& stat);

    static void getStatusOfPid(std::uint64_t pid, Status& stat);

    static void getAllStatus(Status& allStat);

    static void printAllStatus();
};

} // namespace sight::core::memory::tools

#endif //defined(linux) || defined(__linux)
