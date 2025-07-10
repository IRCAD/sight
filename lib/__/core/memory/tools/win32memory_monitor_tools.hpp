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

#ifdef _WIN32

#include <sight/core/config.hpp>

#include <cstdint>

namespace sight::core::memory
{

namespace tools
{

class SIGHT_CORE_CLASS_API win32memory_monitor_tools
{
public:

    SIGHT_CORE_API win32memory_monitor_tools();

    SIGHT_CORE_API ~win32memory_monitor_tools();

    SIGHT_CORE_API static std::uint64_t estimate_free_mem();

    SIGHT_CORE_API static void print_process_memory_information();

    SIGHT_CORE_API static void print_system_memory_information();

    SIGHT_CORE_API static void print_memory_information();

    SIGHT_CORE_API static std::uint64_t get_total_system_memory();

    SIGHT_CORE_API static std::uint64_t get_used_system_memory();

    SIGHT_CORE_API static std::uint64_t get_free_system_memory();

    SIGHT_CORE_API static std::uint64_t get_used_process_memory();
};

} // namespace tools

} // namespace sight::core::memory

#endif
