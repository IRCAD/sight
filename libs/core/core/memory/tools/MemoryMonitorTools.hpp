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

#include "core/config.hpp"

#include <cstdint>

namespace sight::core::memory
{
namespace tools
{

class CORE_CLASS_API MemoryMonitorTools
{

public:

    CORE_API MemoryMonitorTools();

    CORE_API ~MemoryMonitorTools();

    CORE_API static std::uint64_t estimateFreeMem();

    CORE_API static void printProcessMemoryInformation();

    CORE_API static void printSystemMemoryInformation();

    CORE_API static void printMemoryInformation();

    CORE_API static std::uint64_t getTotalSystemMemory();

    CORE_API static std::uint64_t getUsedSystemMemory();

    CORE_API static std::uint64_t getFreeSystemMemory();

    CORE_API static std::uint64_t getUsedProcessMemory();

};

} // namespace tools
} // namespace sight::core::memory
