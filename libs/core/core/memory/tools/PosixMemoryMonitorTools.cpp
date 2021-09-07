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

#if defined(linux) || defined(__linux)

#include "core/memory/tools/PosixMemoryMonitorTools.hpp"

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

namespace sight::core::memory
{

namespace tools
{

std::uint64_t PosixMemoryMonitorTools::s_pageSize    = sysconf(_SC_PAGE_SIZE);
std::uint64_t PosixMemoryMonitorTools::s_totalMemory = sysconf(_SC_PHYS_PAGES) * s_pageSize;

//-----------------------------------------------------------------------------

PosixMemoryMonitorTools::PosixMemoryMonitorTools()
{
}

//-----------------------------------------------------------------------------

PosixMemoryMonitorTools::~PosixMemoryMonitorTools()
{
}

//-----------------------------------------------------------------------------

std::uint64_t PosixMemoryMonitorTools::estimateFreeMem()
{
    std::uint64_t freeMemory = 0;

//  std::uint64_t systemMemoryAverageInNormalCase = 500 * 1024 * 1024; // 500 Mo
//  std::uint64_t memoryUsedByProcess = MemoryMonitor::getDefault()->totalUsedSizeInBytes()
//                                          + 50 * 1024 * 1024; // + 50 Mo of librairies;
//  freeMemory = ( getTotalSystemMemory() < systemMemoryAverageInNormalCase + memoryUsedByProcess?
//                 0:
//                 getTotalSystemMemory() - systemMemoryAverageInNormalCase - memoryUsedByProcess
//               );

    freeMemory = getFreeSystemMemory();

    return freeMemory;
}

//-----------------------------------------------------------------------------

void PosixMemoryMonitorTools::printProcessMemoryInformation()
{
    Status stat;
    getStatusOfPid(getpid(), stat);
    printStatus(stat);
}

//-----------------------------------------------------------------------------

void PosixMemoryMonitorTools::printSystemMemoryInformation()
{
    MemInfo memory;
    get_memory_stats(memory);
    std::uint64_t oToKMo = 1024 * 1024;

    SIGHT_INFO("Total memory: " << memory.total / oToKMo << " Mo");
    SIGHT_INFO("Free memory:  " << memory.free / oToKMo << " Mo");
    SIGHT_INFO("Buffered:     " << memory.buffered / oToKMo << " Mo");
    SIGHT_INFO("Cached:       " << memory.cached / oToKMo << " Mo");
    SIGHT_INFO("Swap Cached:  " << memory.swapcached / oToKMo << " Mo");
    SIGHT_INFO("Swap Total:   " << memory.swaptotal / oToKMo << " Mo");
    SIGHT_INFO("Swap Free:    " << memory.swapfree / oToKMo << " Mo");

    Status allStat;
    getAllStatus(allStat);
    printStatus(allStat);

    std::uint64_t computedFree = (memory.total - allStat.VmRSS) / oToKMo;
    std::uint64_t free         = memory.free / oToKMo;
    SIGHT_INFO(
        "(ComputedFree, Free, Diff) - ( "
        << std::setw(5) << computedFree
        << std::setw(5) << free
        << std::setw(5) << computedFree - free
        << " )"
    );
    (void) computedFree; // Fixes "unused variable" warnings
    (void) free;
}

//-----------------------------------------------------------------------------

void PosixMemoryMonitorTools::printMemoryInformation()
{
    printSystemMemoryInformation();
    printProcessMemoryInformation();
}

//-----------------------------------------------------------------------------

std::uint64_t PosixMemoryMonitorTools::getTotalSystemMemory()
{
    return s_totalMemory;
}

//-----------------------------------------------------------------------------

std::uint64_t PosixMemoryMonitorTools::getUsedSystemMemory()
{
    return getTotalSystemMemory() - getFreeSystemMemory();
}

//-----------------------------------------------------------------------------

std::uint64_t PosixMemoryMonitorTools::getFreeSystemMemory()
{
    //Status allStat;
    //getAllStatus( allStat );
    MemInfo memory;
    get_memory_stats(memory);
    return sysconf(_SC_AVPHYS_PAGES) * s_pageSize + memory.cached;
}

//-----------------------------------------------------------------------------

std::uint64_t PosixMemoryMonitorTools::getUsedProcessMemory()
{
    Status stat;
    getStatusOfPid(getpid(), stat);
    return stat.VmSize;
}

//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------

std::uint64_t PosixMemoryMonitorTools::extract_number(char* str, int start, int end)
{
    int i, j;
    char buf[end - start];

    for(i = start, j = 0 ; i < end ; i++)
    {
        isdigit(str[i]) && (buf[j++] = str[i]);
    }

    buf[j] = '\0';

    return strtoul(buf, NULL, 0) * 1024;
}

//------------------------------------------------------------------------------

void PosixMemoryMonitorTools::get_memory_stats(MemInfo& meminfo)
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
            analyseMemInfo(line, meminfo);
        }

        input.close();
    }
}

//------------------------------------------------------------------------------

void PosixMemoryMonitorTools::analyseMemInfo(std::string& line, MemInfo& meminfo)
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
        meminfo.total = ::boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("MemFree") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        meminfo.free = ::boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("Buffers") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        meminfo.buffered = ::boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("SwapCached") != std::string::npos) // Test before => line.find("Cached")
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        meminfo.swapcached = ::boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("Cached") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        meminfo.cached = ::boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("SwapTotal") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        meminfo.swaptotal = ::boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("SwapFree") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        meminfo.swapfree = ::boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void PosixMemoryMonitorTools::printStatus(Status& stat)
{
    int oToMo = 1024 * 1024;
    SIGHT_DEBUG("VmPeak = " << stat.VmPeak / oToMo << " Mo");
    SIGHT_DEBUG("VmSize = " << stat.VmSize / oToMo << " Mo");
    SIGHT_DEBUG("VmLck = " << stat.VmLck / oToMo << " Mo");
    SIGHT_DEBUG("VmHWM = " << stat.VmHWM / oToMo << " Mo");
    SIGHT_DEBUG("VmRSS = " << stat.VmRSS / oToMo << " Mo");
    SIGHT_DEBUG("VmData = " << stat.VmData / oToMo << " Mo");
    SIGHT_DEBUG("VmStk = " << stat.VmStk / oToMo << " Mo");
    SIGHT_DEBUG("VmExe = " << stat.VmExe / oToMo << " Mo");
    SIGHT_DEBUG("VmLib = " << stat.VmLib / oToMo << " Mo");
    SIGHT_DEBUG("VmPTE = " << stat.VmPTE / oToMo << " Mo");
    (void) oToMo; // Fixes "unused variable" warnings
}

//------------------------------------------------------------------------------

void PosixMemoryMonitorTools::analyseStatusLine(std::string& line, Status& stat)
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
        stat.VmPeak = ::boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("VmSize") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        stat.VmSize = ::boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("VmLck") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        stat.VmLck = ::boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("VmHWM") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        stat.VmHWM = ::boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("VmRSS") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        stat.VmRSS = ::boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("VmData") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        stat.VmData = ::boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("VmStk") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        stat.VmStk = ::boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("VmExe") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        stat.VmExe = ::boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("VmLib") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        stat.VmLib = ::boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
    else if(line.find("VmPTE") != std::string::npos)
    {
        std::string size = regex_replace(
            line,
            e,
            machine_format,
            std::regex_constants::match_default | std::regex_constants::format_sed
        );
        stat.VmPTE = ::boost::lexical_cast<std::uint64_t>(size) * 1024;
    }
}

//------------------------------------------------------------------------------

void PosixMemoryMonitorTools::getStatusOfPid(unsigned long pid, Status& stat)
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
            analyseStatusLine(line, stat);
        }

        input.close();
    }
}

//------------------------------------------------------------------------------

void PosixMemoryMonitorTools::getAllStatus(Status& allStat)
{
    std::filesystem::path path("/proc");
    std::regex e("[0-9]+");

    allStat.VmPeak = 0;
    allStat.VmSize = 0;
    allStat.VmLck  = 0;
    allStat.VmHWM  = 0;
    allStat.VmRSS  = 0;
    allStat.VmData = 0;
    allStat.VmStk  = 0;
    allStat.VmExe  = 0;
    allStat.VmLib  = 0;
    allStat.VmPTE  = 0;

    for(std::filesystem::directory_iterator it(path) ;
        it != std::filesystem::directory_iterator() ;
        ++it)
    {
        if(std::filesystem::is_directory(*it))
        {
            std::filesystem::path tmpPath = (*it);
            std::string dirName           = tmpPath.filename().string();
            if(regex_match(dirName, e))
            {
                auto pid = strtoul(dirName.c_str(), NULL, 0);
                Status stat;
                getStatusOfPid(pid, stat);
                allStat.VmPeak += stat.VmPeak;
                allStat.VmSize += stat.VmSize;
                allStat.VmLck  += stat.VmLck;
                allStat.VmHWM  += stat.VmHWM;
                allStat.VmRSS  += stat.VmRSS;
                allStat.VmData += stat.VmData;
                allStat.VmStk  += stat.VmStk;
                allStat.VmExe  += stat.VmExe;
                allStat.VmLib  += stat.VmLib;
                allStat.VmPTE  += stat.VmPTE;
            }
        }
    }
}

//------------------------------------------------------------------------------

void PosixMemoryMonitorTools::printAllStatus()
{
    std::filesystem::path path("/proc");
    std::regex e("[0-9]+");
    int oToMo = 1024 * 1024;

    std::uint64_t totalVmPeak = 0;
    std::uint64_t totalVmSize = 0;
    std::uint64_t totalVmLck  = 0;
    std::uint64_t totalVmHWM  = 0;
    std::uint64_t totalVmRSS  = 0;
    std::uint64_t totalVmData = 0;
    std::uint64_t totalVmStk  = 0;
    std::uint64_t totalVmExe  = 0;
    std::uint64_t totalVmLib  = 0;
    std::uint64_t totalVmPTE  = 0;

    for(std::filesystem::directory_iterator it(path) ;
        it != std::filesystem::directory_iterator() ;
        ++it)
    {
        if(std::filesystem::is_directory(*it))
        {
            std::filesystem::path tmpPath = (*it);
            std::string dirName           = tmpPath.filename().string();
            if(regex_match(dirName, e))
            {
                auto pid = strtoul(dirName.c_str(), NULL, 0);
                Status stat;
                getStatusOfPid(pid, stat);
                totalVmPeak += stat.VmPeak;
                totalVmSize += stat.VmSize;
                totalVmLck  += stat.VmLck;
                totalVmHWM  += stat.VmHWM;
                totalVmRSS  += stat.VmRSS;
                totalVmData += stat.VmData;
                totalVmStk  += stat.VmStk;
                totalVmExe  += stat.VmExe;
                totalVmLib  += stat.VmLib;
                totalVmPTE  += stat.VmPTE;
            }
        }
    }

    totalVmPeak /= oToMo;
    totalVmSize /= oToMo;
    totalVmLck  /= oToMo;
    totalVmHWM  /= oToMo;
    totalVmRSS  /= oToMo;
    totalVmData /= oToMo;
    totalVmStk  /= oToMo;
    totalVmExe  /= oToMo;
    totalVmLib  /= oToMo;
    totalVmPTE  /= oToMo;

    SIGHT_DEBUG(
        "( " << totalVmPeak << std::setw(5) << totalVmSize << std::setw(5) << totalVmLck << std::setw(
            5
        ) << totalVmHWM << std::setw(5) << totalVmRSS << std::setw(5) << totalVmData << std::setw(
            5
        ) << totalVmStk << std::setw(5) << totalVmExe << std::setw(5) << totalVmLib << std::setw(
            5
        ) << totalVmPTE << " )"
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

} // namespace tools

} // namespace sight::core::memory

#endif //defined(linux) || defined(__linux)
