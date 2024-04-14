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

// cspell:ignore NOLINT

#include "core/memory/tools/memory_monitor_tools.hpp"

#ifdef _WIN32
#define MEMORYTOOLIMPL win32memory_monitor_tools // NOLINT(cppcoreguidelines-macro-usage): It's a class name
#include "core/memory/tools/win32memory_monitor_tools.hpp"
#else
#define MEMORYTOOLIMPL posix_memory_monitor_tools // NOLINT(cppcoreguidelines-macro-usage): It's a class name
#include "core/memory/tools/posix_memory_monitor_tools.hpp"
#endif

namespace sight::core::memory::tools
{

//-----------------------------------------------------------------------------

memory_monitor_tools::memory_monitor_tools()
= default;

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

std::uint64_t memory_monitor_tools::estimate_free_mem()
{
    return MEMORYTOOLIMPL::estimate_free_mem();
}

//-----------------------------------------------------------------------------

void memory_monitor_tools::print_process_memory_information()
{
    MEMORYTOOLIMPL::print_process_memory_information();
}

//-----------------------------------------------------------------------------

void memory_monitor_tools::print_system_memory_information()
{
    MEMORYTOOLIMPL::print_system_memory_information();
}

//-----------------------------------------------------------------------------

void memory_monitor_tools::print_memory_information()
{
    MEMORYTOOLIMPL::print_memory_information();
}

//-----------------------------------------------------------------------------

std::uint64_t memory_monitor_tools::get_total_system_memory()
{
    return MEMORYTOOLIMPL::get_total_system_memory();
}

//-----------------------------------------------------------------------------

std::uint64_t memory_monitor_tools::get_used_system_memory()
{
    return MEMORYTOOLIMPL::get_used_system_memory();
}

//-----------------------------------------------------------------------------

std::uint64_t memory_monitor_tools::get_free_system_memory()
{
    return MEMORYTOOLIMPL::get_free_system_memory();
}

//-----------------------------------------------------------------------------

std::uint64_t memory_monitor_tools::get_used_process_memory()
{
    return MEMORYTOOLIMPL::get_used_process_memory();
}

//-----------------------------------------------------------------------------

} // namespace sight::core::memory::tools
