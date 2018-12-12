/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwMemory/tools/MemoryMonitorTools.hpp"

#ifdef _WIN32
#define MEMORYTOOLIMPL Win32MemoryMonitorTools
#include "fwMemory/tools/Win32MemoryMonitorTools.hpp"
#elif defined(__APPLE__)
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
