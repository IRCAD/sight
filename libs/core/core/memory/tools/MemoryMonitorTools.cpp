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

#include "core/memory/tools/MemoryMonitorTools.hpp"

#ifdef _WIN32
#define MEMORYTOOLIMPL Win32MemoryMonitorTools
#include "core/memory/tools/Win32MemoryMonitorTools.hpp"
#elif defined(__APPLE__)
#define MEMORYTOOLIMPL DarwinMemoryMonitorTools
#include "core/memory/tools/DarwinMemoryMonitorTools.hpp"
#else
#define MEMORYTOOLIMPL PosixMemoryMonitorTools
#include "core/memory/tools/PosixMemoryMonitorTools.hpp"
#endif

namespace sight::core::memory
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

} // namespace sight::core::memory
