/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMemory/tools/MemoryMonitorTools.hpp"

#ifdef _WIN32
#define MEMORYTOOLIMPL Win32MemoryMonitorTools
#include "fwMemory/tools/Win32MemoryMonitorTools.hpp"
#elif defined(__MACOSX__)
#define MEMORYTOOLIMPL DarwinMemoryMonitorTools
#include "fwMemory/tools/DarwinMemoryMonitorTools.hpp"
#else
#define MEMORYTOOLIMPL PosixMemoryMonitorTools
#include "fwMemory/tools/PosixMemoryMonitorTools.hpp"
#endif

namespace fwMemory
{
namespace tools
{

//-----------------------------------------------------------------------------

MemoryMonitorTools::MemoryMonitorTools()
{
}

//-----------------------------------------------------------------------------

MemoryMonitorTools::~MemoryMonitorTools()
{
}

//-----------------------------------------------------------------------------

std::uint64_t MemoryMonitorTools::estimateFreeMem()
{
    return MEMORYTOOLIMPL::estimateFreeMem();
}

//-----------------------------------------------------------------------------

void MemoryMonitorTools::printProcessMemoryInformation()
{
    MEMORYTOOLIMPL::printProcessMemoryInformation();
}

//-----------------------------------------------------------------------------

void MemoryMonitorTools::printSystemMemoryInformation()
{
    MEMORYTOOLIMPL::printSystemMemoryInformation();
}

//-----------------------------------------------------------------------------

void MemoryMonitorTools::printMemoryInformation()
{
    MEMORYTOOLIMPL::printMemoryInformation();
}

//-----------------------------------------------------------------------------

std::uint64_t MemoryMonitorTools::getTotalSystemMemory()
{
    return MEMORYTOOLIMPL::getTotalSystemMemory();
}

//-----------------------------------------------------------------------------

std::uint64_t MemoryMonitorTools::getUsedSystemMemory()
{
    return MEMORYTOOLIMPL::getUsedSystemMemory();
}

//-----------------------------------------------------------------------------

std::uint64_t MemoryMonitorTools::getFreeSystemMemory()
{
    return MEMORYTOOLIMPL::getFreeSystemMemory();
}

//-----------------------------------------------------------------------------

std::uint64_t MemoryMonitorTools::getUsedProcessMemory()
{
    return MEMORYTOOLIMPL::getUsedProcessMemory();
}

//-----------------------------------------------------------------------------

} // namespace tools
} // namespace fwMemory
