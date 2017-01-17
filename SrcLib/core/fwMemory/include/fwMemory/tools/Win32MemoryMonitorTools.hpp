/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
