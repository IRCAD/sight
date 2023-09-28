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

// cspell:ignore NOLINT

#if defined(linux) || defined(__linux)

#include "core/memory/tools/posix_memory_monitor_tools.hpp"

#include <core/base.hpp>

#include <boost/lexical_cast.hpp>

#include <fcntl.h>

#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>

#include <unistd.h>

#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

namespace sight::core::memory::tools
{

std::int64_t posix_memory_monitor_tools::s_page_size    = sysconf(_SC_PAGE_SIZE);
std::int64_t posix_memory_monitor_tools::s_total_memory = sysconf(_SC_PHYS_PAGES) * s_page_size;

//-----------------------------------------------------------------------------

posix_memory_monitor_tools::posix_memory_monitor_tools()
= default;

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

std::uint64_t posix_memory_monitor_tools::estimate_free_mem()
{
    std::uint64_t free_memory = 0;

//  std::uint64_t systemMemoryAverageInNormalCase = 500 * 1024 * 1024; // 500 Mo
//  std::uint64_t memoryUsedByProcess = MemoryMonitor::getDefault()->totalUsedSizeInBytes()
//                                          + 50 * 1024 * 1024; // + 50 Mo of libraries;
//  freeMemory = ( get_total_system_memory() < systemMemoryAverageInNormalCase + memoryUsedByProcess?
//                 0:
//                 get_total_system_memory() - systemMemoryAverageInNormalCase - memoryUsedByProcess
//               );

    free_memory = get_free_system_memory();

    return free_memory;
}

//-----------------------------------------------------------------------------

void posix_memory_monitor_tools::print_process_memory_information()
{
    status stat;
    get_status_of_pid(static_cast<unsigned int>(getpid()), stat);
    print_status(stat);
}

//-----------------------------------------------------------------------------

void posix_memory_monitor_tools::print_system_memory_information()
{
    mem_info memory;
    get_memory_stats(memory);
    std::uint64_t o_to_kmo = 1024LL * 1024;

    SIGHT_INFO("Total memory: " << memory.total / o_to_kmo << " Mo");
    SIGHT_INFO("Free memory:  " << memory.free / o_to_kmo << " Mo");
    SIGHT_INFO("Buffered:     " << memory.buffered / o_to_kmo << " Mo");
    SIGHT_INFO("Cached:       " << memory.cached / o_to_kmo << " Mo");
    SIGHT_INFO("Swap Cached:  " << memory.swapcached / o_to_kmo << " Mo");
    SIGHT_INFO("Swap Total:   " << memory.swaptotal / o_to_kmo << " Mo");
    SIGHT_INFO("Swap Free:    " << memory.swapfree / o_to_kmo << " Mo");

    status all_stat;
    get_all_status(all_stat);
    print_status(all_stat);

    std::uint64_t computed_free = (memory.total - all_stat.vm_rss) / o_to_kmo;
    std::uint64_t free          = memory.free / o_to_kmo;
    SIGHT_INFO(
        "(ComputedFree, Free, Diff) - ( "
        << std::setw(5) << computed_free
        << std::setw(5) << free
        << std::setw(5) << computed_free - free
        << " )"
    );
    (void) computed_free; // Fixes "unused variable" warnings
    (void) free;
}

//-----------------------------------------------------------------------------

void posix_memory_monitor_tools::print_memory_information()
{
    print_system_memory_information();
    print_process_memory_information();
}

//-----------------------------------------------------------------------------

std::uint64_t posix_memory_monitor_tools::get_total_system_memory()
{
    return static_cast<std::uint64_t>(s_total_memory);
}

//-----------------------------------------------------------------------------

std::uint64_t posix_memory_monitor_tools::get_used_system_memory()
{
    return get_total_system_memory() - get_free_system_memory();
}

//-----------------------------------------------------------------------------

std::uint64_t posix_memory_monitor_tools::get_free_system_memory()
{
    //Status allStat;
    //getAllStatus( allStat );
    mem_info memory;
    get_memory_stats(memory);
    return std::uint64_t(sysconf(_SC_AVPHYS_PAGES) * s_page_size) + memory.cached;
}

//-----------------------------------------------------------------------------

std::uint64_t posix_memory_monitor_tools::get_used_process_memory()
{
    status stat;
    get_status_of_pid(static_cast<unsigned int>(getpid()), stat);
    return stat.vm_size;
}

//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------

std::uint64_t posix_memory_monitor_tools::extract_number(char* str, int start, int end)
{
    std::size_t i = 0;
    std::size_t j = 0;
    std::vector<char> buf;
    buf.resize(std::size_t(end - start));

    for(i = std::size_t(start), j = 0 ; i < std::size_t(end) ; i++)
    {
        (isdigit(str[i]) != 0) && (buf[j++] = str[i]); // NOLINT(readability-implicit-bool-conversion)
    }

    buf[j] = '\0';

    return strtoul(buf.data(), nullptr, 0) * 1024;
}

//------------------------------------------------------------------------------

void posix_memory_monitor_tools::get_memory_stats(mem_info& meminfo)
{
/*
    // We are bothered about only the first 338 bytes of the /proc/meminfo file
    char buf[338];
    int in;

    in = open("/proc/meminfo", O_RDONLY);

    read(in, buf, sizeof(buf));

    close(in);

    meminfo.total = extract_number(buf, 9, 22);
    meminfo.free = extract_number(buf, 35, 49);
    meminfo.buffered = extract_number(buf, 61, 75);
    meminfo.cached = extract_number(buf, 86, 101);
    meminfo.swapcached = extract_number(buf, 116, 127);
    meminfo.swaptotal = extract_number(buf, 297, 309);
    meminfo.swapfree = extract_number(buf, 322, 335);
 */

    std::ifstream input("/proc/meminfo");

    std::string line;
    if(input.is_open())
    {
        while(!input.eof())
        {
            getline(input, line);
            analyse_mem_info(line, meminfo);
        }

        input.close();
    }
}

//------------------------------------------------------------------------------

void posix_memory_monitor_tools::analyse_mem_info(std::string& line, mem_info& meminfo)
{
    std::regex e("([A-Za-z:]+)([ \t]+)([0-9]+)([ \t]+)kB(.*)");
    std::string machine_format = "\\3";
    if(line.find("MemTotal") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        meminfo.total = boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("MemFree") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        meminfo.free = boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("Buffers") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        meminfo.buffered = boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("SwapCached") != std::string::npos) // Test before => line.find("Cached")
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        meminfo.swapcached = boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("Cached") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        meminfo.cached = boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("SwapTotal") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        meminfo.swaptotal = boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("SwapFree") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        meminfo.swapfree = boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void posix_memory_monitor_tools::print_status(status& stat)
{
    std::uint64_t o_to_mo = 1024LL * 1024;
    SIGHT_DEBUG("VmPeak = " << stat.vm_peak / o_to_mo << " Mo");
    SIGHT_DEBUG("VmSize = " << stat.vm_size / o_to_mo << " Mo");
    SIGHT_DEBUG("VmLck = " << stat.vm_lck / o_to_mo << " Mo");
    SIGHT_DEBUG("VmHWM = " << stat.vm_hwm / o_to_mo << " Mo");
    SIGHT_DEBUG("VmRSS = " << stat.vm_rss / o_to_mo << " Mo");
    SIGHT_DEBUG("VmData = " << stat.vm_data / o_to_mo << " Mo");
    SIGHT_DEBUG("VmStk = " << stat.vm_stk / o_to_mo << " Mo");
    SIGHT_DEBUG("VmExe = " << stat.vm_exe / o_to_mo << " Mo");
    SIGHT_DEBUG("VmLib = " << stat.vm_lib / o_to_mo << " Mo");
    SIGHT_DEBUG("VmPTE = " << stat.vm_pte / o_to_mo << " Mo");
    (void) o_to_mo; // Fixes "unused variable" warnings
}

//------------------------------------------------------------------------------

void posix_memory_monitor_tools::analyse_status_line(std::string& line, status& stat)
{
    std::regex e("([A-Za-z:]+)([ \t]+)([0-9]+)([ \t]+)kB(.*)");
    std::string machine_format = "\\3";
    if(line.find("VmPeak") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        stat.vm_peak = boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("VmSize") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        stat.vm_size = boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("VmLck") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        stat.vm_lck = boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("VmHWM") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        stat.vm_hwm = boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("VmRSS") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        stat.vm_rss = boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("VmData") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        stat.vm_data = boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("VmStk") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        stat.vm_stk = boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("VmExe") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        stat.vm_exe = boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("VmLib") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        stat.vm_lib = boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("VmPTE") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        stat.vm_pte = boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
}

//------------------------------------------------------------------------------

void posix_memory_monitor_tools::get_status_of_pid(std::uint64_t pid, status& stat)
{
    std::stringstream file;
    file << "/proc/" << pid << "/status";
    std::ifstream input(file.str().c_str());

    std::string line;
    if(input.is_open())
    {
        while(!input.eof())
        {
            getline(input, line);
            analyse_status_line(line, stat);
        }

        input.close();
    }
}

//------------------------------------------------------------------------------

void posix_memory_monitor_tools::get_all_status(status& all_stat)
{
    std::filesystem::path path("/proc");
    std::regex e("[0-9]+");

    all_stat.vm_peak = 0;
    all_stat.vm_size = 0;
    all_stat.vm_lck  = 0;
    all_stat.vm_hwm  = 0;
    all_stat.vm_rss  = 0;
    all_stat.vm_data = 0;
    all_stat.vm_stk  = 0;
    all_stat.vm_exe  = 0;
    all_stat.vm_lib  = 0;
    all_stat.vm_pte  = 0;

    for(std::filesystem::directory_iterator it(path) ;
        it != std::filesystem::directory_iterator() ;
        ++it)
    {
        if(std::filesystem::is_directory(*it))
        {
            std::filesystem::path tmp_path = (*it);
            std::string dir_name           = tmp_path.filename().string();
            if(regex_match(dir_name, e))
            {
                auto pid = strtoul(dir_name.c_str(), nullptr, 0);
                status stat;
                get_status_of_pid(pid, stat);
                all_stat.vm_peak += stat.vm_peak;
                all_stat.vm_size += stat.vm_size;
                all_stat.vm_lck  += stat.vm_lck;
                all_stat.vm_hwm  += stat.vm_hwm;
                all_stat.vm_rss  += stat.vm_rss;
                all_stat.vm_data += stat.vm_data;
                all_stat.vm_stk  += stat.vm_stk;
                all_stat.vm_exe  += stat.vm_exe;
                all_stat.vm_lib  += stat.vm_lib;
                all_stat.vm_pte  += stat.vm_pte;
            }
        }
    }
}

//------------------------------------------------------------------------------

void posix_memory_monitor_tools::print_all_status()
{
    std::filesystem::path path("/proc");
    std::regex e("[0-9]+");

    std::uint64_t total_vm_peak = 0;
    std::uint64_t total_vm_size = 0;
    std::uint64_t total_vm_lck  = 0;
    std::uint64_t total_vm_hwm  = 0;
    std::uint64_t total_vm_rss  = 0;
    std::uint64_t total_vm_data = 0;
    std::uint64_t total_vm_stk  = 0;
    std::uint64_t total_vm_exe  = 0;
    std::uint64_t total_vm_lib  = 0;
    std::uint64_t total_vm_pte  = 0;

    for(std::filesystem::directory_iterator it(path) ;
        it != std::filesystem::directory_iterator() ;
        ++it)
    {
        if(std::filesystem::is_directory(*it))
        {
            std::filesystem::path tmp_path = (*it);
            std::string dir_name           = tmp_path.filename().string();
            if(regex_match(dir_name, e))
            {
                auto pid = strtoul(dir_name.c_str(), nullptr, 0);
                status stat;
                get_status_of_pid(pid, stat);
                total_vm_peak += stat.vm_peak;
                total_vm_size += stat.vm_size;
                total_vm_lck  += stat.vm_lck;
                total_vm_hwm  += stat.vm_hwm;
                total_vm_rss  += stat.vm_rss;
                total_vm_data += stat.vm_data;
                total_vm_stk  += stat.vm_stk;
                total_vm_exe  += stat.vm_exe;
                total_vm_lib  += stat.vm_lib;
                total_vm_pte  += stat.vm_pte;
            }
        }
    }

    std::uint64_t o_to_mo = 1024LL * 1024;
    total_vm_peak /= o_to_mo;
    total_vm_size /= o_to_mo;
    total_vm_lck  /= o_to_mo;
    total_vm_hwm  /= o_to_mo;
    total_vm_rss  /= o_to_mo;
    total_vm_data /= o_to_mo;
    total_vm_stk  /= o_to_mo;
    total_vm_exe  /= o_to_mo;
    total_vm_lib  /= o_to_mo;
    total_vm_pte  /= o_to_mo;

    SIGHT_DEBUG(
        "( " << total_vm_peak << std::setw(5) << total_vm_size << std::setw(5) << total_vm_lck << std::setw(
            5
        ) << total_vm_hwm << std::setw(5) << total_vm_rss << std::setw(5) << total_vm_data << std::setw(
            5
        ) << total_vm_stk << std::setw(5) << total_vm_exe << std::setw(5) << total_vm_lib << std::setw(
            5
        ) << total_vm_pte << " )"
    );

    /*
       SIGHT_DEBUG("totalVmPeak = " << totalVmPeak / oToMo << " Mo" );
       SIGHT_DEBUG("totalVmSize = " << totalVmSize / oToMo << " Mo" );
       SIGHT_DEBUG("totalVmLck = " << totalVmLck / oToMo << " Mo" );
       SIGHT_DEBUG("totalVmHWM = " << totalVmHWM / oToMo << " Mo" );
       SIGHT_DEBUG("totalVmRSS = " << totalVmRSS / oToMo << " Mo" );
       SIGHT_DEBUG("totalVmData = " << totalVmData / oToMo << " Mo" );
       SIGHT_DEBUG("totalVmStk = " << totalVmStk / oToMo << " Mo" );
       SIGHT_DEBUG("totalVmExe = " << totalVmExe / oToMo << " Mo" );
       SIGHT_DEBUG("totalVmLib = " << totalVmLib / oToMo << " Mo" );
       SIGHT_DEBUG("totalVmPTE = " << totalVmPTE / oToMo << " Mo" );
     */
}

} // namespace sight::core::memory::tools

#endif //defined(linux) || defined(__linux)
