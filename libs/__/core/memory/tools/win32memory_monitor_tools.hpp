/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "core/config.hpp"

#include <cstdint>

namespace sight::core::memory
{

namespace tools
{

class CORE_CLASS_API win32memory_monitor_tools
{
public:

    CORE_API win32memory_monitor_tools();

    CORE_API ~win32memory_monitor_tools();

    CORE_API static std::uint64_t estimate_free_mem();

    CORE_API static void print_process_memory_information();

    CORE_API static void print_system_memory_information();

    CORE_API static void print_memory_information();

    CORE_API static std::uint64_t get_total_system_memory();

    CORE_API static std::uint64_t get_used_system_memory();

    CORE_API static std::uint64_t get_free_system_memory();

    CORE_API static std::uint64_t get_used_process_memory();
};

} // namespace tools

} // namespace sight::core::memory

#endif
