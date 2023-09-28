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

#include "core/memory/policy/never_dump.hpp"

#include "core/memory/policy/registry/macros.hpp"

namespace sight::core::memory::policy
{

SIGHT_REGISTER_MEMORY_POLICY(core::memory::policy::never_dump);

//------------------------------------------------------------------------------

void never_dump::allocation_request(
    buffer_info& info,
    core::memory::buffer_manager::const_buffer_ptr_t buffer,
    buffer_info::size_t size
)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
    SIGHT_NOT_USED(size);
}

//------------------------------------------------------------------------------

void never_dump::set_request(
    buffer_info& info,
    core::memory::buffer_manager::const_buffer_ptr_t buffer,
    buffer_info::size_t size
)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
    SIGHT_NOT_USED(size);
}

//------------------------------------------------------------------------------

void never_dump::reallocate_request(
    buffer_info& info,
    core::memory::buffer_manager::const_buffer_ptr_t buffer,
    buffer_info::size_t new_size
)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
    SIGHT_NOT_USED(new_size);
}

//------------------------------------------------------------------------------

void never_dump::destroy_request(buffer_info& info, core::memory::buffer_manager::const_buffer_ptr_t buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
}

//------------------------------------------------------------------------------

void never_dump::lock_request(buffer_info& info, core::memory::buffer_manager::const_buffer_ptr_t buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
}

//------------------------------------------------------------------------------

void never_dump::unlock_request(buffer_info& info, core::memory::buffer_manager::const_buffer_ptr_t buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
}

//------------------------------------------------------------------------------

void never_dump::dump_success(buffer_info& info, core::memory::buffer_manager::const_buffer_ptr_t buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
}

//------------------------------------------------------------------------------

void never_dump::restore_success(buffer_info& info, core::memory::buffer_manager::const_buffer_ptr_t buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
}

//------------------------------------------------------------------------------

void never_dump::refresh()
{
}

//------------------------------------------------------------------------------

std::string never_dump::get_param(const std::string& name, bool* ok) const
{
    SIGHT_NOT_USED(name);
    if(ok != nullptr)
    {
        *ok = false;
    }

    return "";
}

//------------------------------------------------------------------------------

} // namespace sight::core::memory::policy
