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

#include "core/memory/policy/always_dump.hpp"

#include "core/memory/policy/barrier_dump.hpp"
#include "core/memory/policy/registry/macros.hpp"

namespace sight::core::memory::policy
{

SIGHT_REGISTER_MEMORY_POLICY(core::memory::policy::always_dump);

//------------------------------------------------------------------------------

void always_dump::allocation_request(
    buffer_info& _info,
    core::memory::buffer_manager::const_buffer_ptr_t _buffer,
    buffer_info::size_t _size
)
{
    SIGHT_NOT_USED(_info);
    SIGHT_NOT_USED(_buffer);
    SIGHT_NOT_USED(_size);
    this->apply();
}

//------------------------------------------------------------------------------

void always_dump::set_request(
    buffer_info& _info,
    core::memory::buffer_manager::const_buffer_ptr_t _buffer,
    buffer_info::size_t _size
)
{
    SIGHT_NOT_USED(_info);
    SIGHT_NOT_USED(_buffer);
    SIGHT_NOT_USED(_size);
    this->apply();
}

//------------------------------------------------------------------------------

void always_dump::reallocate_request(
    buffer_info& _info,
    core::memory::buffer_manager::const_buffer_ptr_t _buffer,
    buffer_info::size_t _new_size
)
{
    SIGHT_NOT_USED(_info);
    SIGHT_NOT_USED(_buffer);
    SIGHT_NOT_USED(_new_size);
    this->apply();
}

//------------------------------------------------------------------------------

void always_dump::destroy_request(buffer_info& _info, core::memory::buffer_manager::const_buffer_ptr_t _buffer)
{
    SIGHT_NOT_USED(_info);
    SIGHT_NOT_USED(_buffer);
    this->apply();
}

//------------------------------------------------------------------------------

void always_dump::lock_request(buffer_info& _info, core::memory::buffer_manager::const_buffer_ptr_t _buffer)
{
    SIGHT_NOT_USED(_info);
    SIGHT_NOT_USED(_buffer);
    this->apply();
}

//------------------------------------------------------------------------------

void always_dump::unlock_request(buffer_info& _info, core::memory::buffer_manager::const_buffer_ptr_t _buffer)
{
    SIGHT_NOT_USED(_info);
    SIGHT_NOT_USED(_buffer);
    this->apply();
}

//------------------------------------------------------------------------------

void always_dump::dump_success(buffer_info& _info, core::memory::buffer_manager::const_buffer_ptr_t _buffer)
{
    SIGHT_NOT_USED(_info);
    SIGHT_NOT_USED(_buffer);
}

//------------------------------------------------------------------------------

void always_dump::restore_success(buffer_info& _info, core::memory::buffer_manager::const_buffer_ptr_t _buffer)
{
    SIGHT_NOT_USED(_info);
    SIGHT_NOT_USED(_buffer);
}

//------------------------------------------------------------------------------

std::size_t always_dump::dump()
{
    std::size_t dumped = 0;

    core::memory::buffer_manager::sptr manager = core::memory::buffer_manager::get();
    if(manager)
    {
        const core::memory::buffer_manager::buffer_info_map_t buffer_infos = manager->get_buffer_infos().get();

        for(const core::memory::buffer_manager::buffer_info_map_t::value_type& elt : buffer_infos)
        {
            const core::memory::buffer_info& info = elt.second;
            if(!(info.size == 0 || info.lock_count() > 0 || !info.loaded))
            {
                if(manager->dump_buffer(elt.first).get())
                {
                    dumped += elt.second.size;
                }
            }
        }
    }

    return dumped;
}

//------------------------------------------------------------------------------

void always_dump::apply()
{
    this->dump();
}

//------------------------------------------------------------------------------

void always_dump::refresh()
{
    this->apply();
}

//------------------------------------------------------------------------------

std::string always_dump::get_param(const std::string& _name, bool* _ok) const
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
