/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/core/config.hpp>

#include <cstdint>
#include <string>

namespace sight::core::memory::tools
{

//------------------------------------------------------------------------------

struct mem_info
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

struct status
{
    std::uint64_t vm_peak {0};
    std::uint64_t vm_size {0};
    std::uint64_t vm_lck {0};
    std::uint64_t vm_hwm {0};
    std::uint64_t vm_rss {0};
    std::uint64_t vm_data {0};
    std::uint64_t vm_stk {0};
    std::uint64_t vm_exe {0};
    std::uint64_t vm_lib {0};
    std::uint64_t vm_pte {0};
};

//------------------------------------------------------------------------------

class SIGHT_CORE_CLASS_API posix_memory_monitor_tools
{
public:

    SIGHT_CORE_API posix_memory_monitor_tools();

    SIGHT_CORE_API ~posix_memory_monitor_tools() = default;

    SIGHT_CORE_API static std::uint64_t estimate_free_mem();

    SIGHT_CORE_API static void print_process_memory_information();

    SIGHT_CORE_API static void print_system_memory_information();

    SIGHT_CORE_API static void print_memory_information();

    SIGHT_CORE_API static std::uint64_t get_total_system_memory();

    SIGHT_CORE_API static std::uint64_t get_used_system_memory();

    SIGHT_CORE_API static std::uint64_t get_free_system_memory();

    SIGHT_CORE_API static std::uint64_t get_used_process_memory();

private:

    static std::int64_t s_page_size;
    static std::int64_t s_total_memory;

    /* Extract numbers from a string between the start and end indices */
    static std::uint64_t extract_number(char* _str, int _start, int _end);

    /* Parse the contents of /proc/meminfo file into the meminfo structure */
    static void get_memory_stats(mem_info& _meminfo);

    static void print_status(status& _stat);

    static void analyse_mem_info(std::string& _line, mem_info& _meminfo);

    static void analyse_status_line(std::string& _line, status& _stat);

    static void get_status_of_pid(std::uint64_t _pid, status& _stat);

    static void get_all_status(status& _all_stat);

    static void print_all_status();
};

} // namespace sight::core::memory::tools

#endif //defined(linux) || defined(__linux)
