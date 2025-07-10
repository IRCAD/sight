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

#include "core/memory/byte_size.hpp"
#include "core/memory/exception/bad_cast.hpp"
#include "core/memory/policy/registry/macros.hpp"
#include "core/memory/policy/valve_dump.hpp"

#include <cstddef>

namespace sight::core::memory::policy
{

SIGHT_REGISTER_MEMORY_POLICY(core::memory::policy::valve_dump<core::memory::tools::memory_monitor_tools>);

//------------------------------------------------------------------------------

template<typename TOOL>
valve_dump<TOOL>::valve_dump() :
    m_min_free_mem(1024LL * 1024 * 500LL)
{
}

//------------------------------------------------------------------------------

template<typename TOOL>
void valve_dump<TOOL>::allocation_request(
    buffer_info& info,
    core::memory::buffer_manager::const_buffer_ptr_t buffer,
    buffer_info::size_t size
)
{
    SIGHT_NOT_USED(buffer);
    this->apply((size > info.size) ? size - info.size : 0);
}

//------------------------------------------------------------------------------

template<typename TOOL>
void valve_dump<TOOL>::set_request(
    buffer_info& info,
    core::memory::buffer_manager::const_buffer_ptr_t buffer,
    buffer_info::size_t size
)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
    SIGHT_NOT_USED(size);
    this->apply();
}

//------------------------------------------------------------------------------

template<typename TOOL>
void valve_dump<TOOL>::reallocate_request(
    buffer_info& info,
    core::memory::buffer_manager::const_buffer_ptr_t buffer,
    buffer_info::size_t new_size
)
{
    SIGHT_NOT_USED(buffer);
    this->apply((new_size > info.size) ? new_size - info.size : 0);
}

//------------------------------------------------------------------------------

template<typename TOOL>
void valve_dump<TOOL>::destroy_request(buffer_info& info, core::memory::buffer_manager::const_buffer_ptr_t buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
}

//------------------------------------------------------------------------------

template<typename TOOL>
void valve_dump<TOOL>::lock_request(buffer_info& info, core::memory::buffer_manager::const_buffer_ptr_t buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
}

//------------------------------------------------------------------------------

template<typename TOOL>
void valve_dump<TOOL>::unlock_request(buffer_info& info, core::memory::buffer_manager::const_buffer_ptr_t buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
    this->apply();
}

//------------------------------------------------------------------------------

template<typename TOOL>
void valve_dump<TOOL>::dump_success(buffer_info& info, core::memory::buffer_manager::const_buffer_ptr_t buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
}

//------------------------------------------------------------------------------

template<typename TOOL>
void valve_dump<TOOL>::restore_success(buffer_info& info, core::memory::buffer_manager::const_buffer_ptr_t buffer)
{
    SIGHT_NOT_USED(info);
    SIGHT_NOT_USED(buffer);
}

//------------------------------------------------------------------------------

template<typename TOOL>
bool valve_dump<TOOL>::need_dump(std::size_t supplement) const
{
    return TOOL::get_free_system_memory() <= (m_min_free_mem + supplement);
}

//------------------------------------------------------------------------------

template<typename TOOL>
std::size_t valve_dump<TOOL>::dump(std::size_t nb_of_bytes)
{
    std::size_t dumped = 0;

    core::memory::buffer_manager::sptr manager = core::memory::buffer_manager::get();
    if(manager)
    {
        const core::memory::buffer_manager::buffer_info_map_t buffer_infos = manager->get_buffer_infos().get();

        typedef std::pair<
                core::memory::buffer_manager::buffer_info_map_t::key_type,
                core::memory::buffer_manager::buffer_info_map_t::mapped_type
        > buffer_infos_pair_type;
        typedef std::vector<buffer_infos_pair_type> buffer_vector_type;

        buffer_vector_type buffers;

        for(const auto& elt : buffer_infos)
        {
            const core::memory::buffer_info& info = elt.second;
            if(!(info.size == 0 || info.lock_count() > 0 || !info.loaded))
            {
                buffers.push_back(elt);
            }
        }

        for(const auto& pair : buffer_infos)
        {
            if(dumped < nb_of_bytes)
            {
                if(manager->dump_buffer(pair.first).get())
                {
                    dumped += pair.second.size;
                }
            }
            else
            {
                break;
            }
        }
    }

    return dumped;
}

//------------------------------------------------------------------------------

template<typename TOOL>
void valve_dump<TOOL>::apply(std::size_t supplement)
{
    if(this->need_dump(supplement))
    {
        this->dump(
            (m_min_free_mem + m_hysteresis_offset + supplement)
            - TOOL::get_free_system_memory()
        );
    }
}

//------------------------------------------------------------------------------

template<typename TOOL>
void valve_dump<TOOL>::refresh()
{
    this->apply();
}

//------------------------------------------------------------------------------

template<typename TOOL>
bool valve_dump<TOOL>::set_param(const std::string& name, const std::string& value)
{
    SIGHT_INFO("Set " << name << " to " << value);
    try
    {
        if(name == "min_free_mem")
        {
            m_min_free_mem = core::memory::byte_size(value).size();
            return true;
        }

        if(name == "hysteresis_offset")
        {
            m_hysteresis_offset = core::memory::byte_size(value).size();
            return true;
        }
    }
    catch(core::memory::exception::bad_cast const&)
    {
        SIGHT_ERROR("Bad value for " << name << " : " << value);
        return false;
    }
    SIGHT_ERROR("Bad parameter name " << name);
    return false;
}

//------------------------------------------------------------------------------

template<typename TOOL>
const core::memory::policy::base::param_names_type& valve_dump<TOOL>::get_param_names() const
{
    static const core::memory::policy::base::param_names_type s_PARAMS {"min_free_mem", "hysteresis_offset"};
    return s_PARAMS;
}

//------------------------------------------------------------------------------

template<typename TOOL>
std::string valve_dump<TOOL>::get_param(const std::string& name, bool* ok) const
{
    bool is_ok = false;
    std::string value;
    if(name == "min_free_mem")
    {
        value = std::string(core::memory::byte_size(core::memory::byte_size::size_t(m_min_free_mem)));
        is_ok = true;
    }
    else if(name == "hysteresis_offset")
    {
        value = std::string(core::memory::byte_size(core::memory::byte_size::size_t(m_hysteresis_offset)));
        is_ok = true;
    }

    if(ok != nullptr)
    {
        *ok = is_ok;
    }

    return value;
}

//------------------------------------------------------------------------------

} // namespace sight::core::memory::policy
