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
    buffer_info& _info,
    core::memory::buffer_manager::const_buffer_ptr_t _buffer,
    buffer_info::size_t _size
)
{
    SIGHT_NOT_USED(_info);
    SIGHT_NOT_USED(_buffer);
    SIGHT_NOT_USED(_size);
}

//------------------------------------------------------------------------------

void never_dump::set_request(
    buffer_info& _info,
    core::memory::buffer_manager::const_buffer_ptr_t _buffer,
    buffer_info::size_t _size
)
{
    SIGHT_NOT_USED(_info);
    SIGHT_NOT_USED(_buffer);
    SIGHT_NOT_USED(_size);
}

//------------------------------------------------------------------------------

void never_dump::reallocate_request(
    buffer_info& _info,
    core::memory::buffer_manager::const_buffer_ptr_t _buffer,
    buffer_info::size_t _new_size
)
{
    SIGHT_NOT_USED(_info);
    SIGHT_NOT_USED(_buffer);
    SIGHT_NOT_USED(_new_size);
}

//------------------------------------------------------------------------------

void never_dump::destroy_request(buffer_info& _info, core::memory::buffer_manager::const_buffer_ptr_t _buffer)
{
    SIGHT_NOT_USED(_info);
    SIGHT_NOT_USED(_buffer);
}

//------------------------------------------------------------------------------

void never_dump::lock_request(buffer_info& _info, core::memory::buffer_manager::const_buffer_ptr_t _buffer)
{
    SIGHT_NOT_USED(_info);
    SIGHT_NOT_USED(_buffer);
}

//------------------------------------------------------------------------------

void never_dump::unlock_request(buffer_info& _info, core::memory::buffer_manager::const_buffer_ptr_t _buffer)
{
    SIGHT_NOT_USED(_info);
    SIGHT_NOT_USED(_buffer);
}

//------------------------------------------------------------------------------

void never_dump::dump_success(buffer_info& _info, core::memory::buffer_manager::const_buffer_ptr_t _buffer)
{
    SIGHT_NOT_USED(_info);
    SIGHT_NOT_USED(_buffer);
}

//------------------------------------------------------------------------------

void never_dump::restore_success(buffer_info& _info, core::memory::buffer_manager::const_buffer_ptr_t _buffer)
{
    SIGHT_NOT_USED(_info);
    SIGHT_NOT_USED(_buffer);
}

//------------------------------------------------------------------------------

void never_dump::refresh()
{
}

//------------------------------------------------------------------------------

std::string never_dump::get_param(const std::string& _name, bool* _ok) const
{
    SIGHT_NOT_USED(_name);
    if(_ok != nullptr)
    {
        *_ok = false;
    }

    return "";
}

//------------------------------------------------------------------------------

} // namespace sight::core::memory::policy
