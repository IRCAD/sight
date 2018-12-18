/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWMEMORY_TOOLS_WIN32MEMORYMONITORTOOLS_HPP__
#define __FWMEMORY_TOOLS_WIN32MEMORYMONITORTOOLS_HPP__

#ifdef _WIN32

#include "fwMemory/config.hpp"

#include <cstdint>

namespace fwMemory
{
namespace tools
{

class FWMEMORY_CLASS_API Win32MemoryMonitorTools
{

public:

    FWMEMORY_API Win32MemoryMonitorTools();

    FWMEMORY_API ~Win32MemoryMonitorTools();

    FWMEMORY_API static std::uint64_t estimateFreeMem();

    FWMEMORY_API static void printProcessMemoryInformation();

    FWMEMORY_API static void printSystemMemoryInformation();

    FWMEMORY_API static void printMemoryInformation();

    FWMEMORY_API static std::uint64_t getTotalSystemMemory();

    FWMEMORY_API static std::uint64_t getUsedSystemMemory();

    FWMEMORY_API static std::uint64_t getFreeSystemMemory();

    FWMEMORY_API static std::uint64_t getUsedProcessMemory();

};

} // namespace tools
} // namespace fwMemory

#endif

#endif // __FWMEMORY_TOOLS_WIN32MEMORYMONITORTOOLS_HPP__
