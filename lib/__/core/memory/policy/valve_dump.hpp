/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "core/memory/buffer_info.hpp"
#include "core/memory/buffer_manager.hpp"
#include "core/memory/byte_size.hpp"
#include "core/memory/exception/bad_cast.hpp"
#include "core/memory/policy/base.hpp"
#include "core/memory/tools/memory_monitor_tools.hpp"

#include <cstddef>

namespace sight::core::memory::policy
{

/**
 * @brief Valve dump policy
 *
 * This policy is monitoring system memory usage and will trigger memory dump
 * when the amount of system free memory is lower than the minFreeMem parameter.
 *
 * An hysteresis parameter exists to free more memory when the process is
 * triggered. If the free system memory amount is lower than the minFreeMem,
 * this policy will try to reach minFreeMem + hysteresisOffset bytes of free
 * memory.
 */
template<typename TOOL = tools::memory_monitor_tools>
class valve_dump : public core::memory::policy::base
{
public:

    SIGHT_DECLARE_CLASS(valve_dump, core::memory::policy::base);

    valve_dump() = default;

    void allocation_request(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer,
        buffer_info::size_t _size
    ) override;
    void set_request(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer,
        buffer_info::size_t _size
    ) override;
    void reallocate_request(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer,
        buffer_info::size_t _new_size
    ) override;

    void destroy_request(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer
    ) override;

    void lock_request(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer
    ) override;
    void unlock_request(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer
    ) override;

    void dump_success(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer
    ) override;
    void restore_success(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer
    ) override;

    void refresh() override;

    bool set_param(const std::string& _name, const std::string& _value) override;
    std::string get_param(const std::string& _name, bool* _ok = nullptr) const override;
    const core::memory::policy::base::param_names_type& get_param_names() const override;

private:

    bool need_dump(std::size_t _supplement) const;

    std::size_t dump(std::size_t _nb_of_bytes);

    void apply(std::size_t _supplement = 0);

    std::size_t m_min_free_mem {1024LL * 1024 * 500LL};
    std::size_t m_hysteresis_offset {0};
};

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

template<typename TOOL>
void valve_dump<TOOL>::allocation_request(
    buffer_info& _info,
    core::memory::buffer_manager::const_buffer_ptr_t _buffer,
    buffer_info::size_t _size
)
{
    SIGHT_NOT_USED(_buffer);
    this->apply((_size > _info.size) ? _size - _info.size : 0);
}

//------------------------------------------------------------------------------

template<typename TOOL>
void valve_dump<TOOL>::set_request(
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

template<typename TOOL>
void valve_dump<TOOL>::reallocate_request(
    buffer_info& _info,
    core::memory::buffer_manager::const_buffer_ptr_t _buffer,
    buffer_info::size_t _new_size
)
{
    SIGHT_NOT_USED(_buffer);
    this->apply((_new_size > _info.size) ? _new_size - _info.size : 0);
}

//------------------------------------------------------------------------------

template<typename TOOL>
void valve_dump<TOOL>::destroy_request(buffer_info& _info, core::memory::buffer_manager::const_buffer_ptr_t _buffer)
{
    SIGHT_NOT_USED(_info);
    SIGHT_NOT_USED(_buffer);
}

//------------------------------------------------------------------------------

template<typename TOOL>
void valve_dump<TOOL>::lock_request(buffer_info& _info, core::memory::buffer_manager::const_buffer_ptr_t _buffer)
{
    SIGHT_NOT_USED(_info);
    SIGHT_NOT_USED(_buffer);
}

//------------------------------------------------------------------------------

template<typename TOOL>
void valve_dump<TOOL>::unlock_request(buffer_info& _info, core::memory::buffer_manager::const_buffer_ptr_t _buffer)
{
    SIGHT_NOT_USED(_info);
    SIGHT_NOT_USED(_buffer);
    this->apply();
}

//------------------------------------------------------------------------------

template<typename TOOL>
void valve_dump<TOOL>::dump_success(buffer_info& _info, core::memory::buffer_manager::const_buffer_ptr_t _buffer)
{
    SIGHT_NOT_USED(_info);
    SIGHT_NOT_USED(_buffer);
}

//------------------------------------------------------------------------------

template<typename TOOL>
void valve_dump<TOOL>::restore_success(buffer_info& _info, core::memory::buffer_manager::const_buffer_ptr_t _buffer)
{
    SIGHT_NOT_USED(_info);
    SIGHT_NOT_USED(_buffer);
}

//------------------------------------------------------------------------------

template<typename TOOL>
bool valve_dump<TOOL>::need_dump(std::size_t _supplement) const
{
    return TOOL::get_free_system_memory() <= (m_min_free_mem + _supplement);
}

//------------------------------------------------------------------------------

template<typename TOOL>
std::size_t valve_dump<TOOL>::dump(std::size_t _nb_of_bytes)
{
    std::size_t dumped = 0;

    core::memory::buffer_manager::sptr manager = core::memory::buffer_manager::get();
    if(manager)
    {
        const core::memory::buffer_manager::buffer_info_map_t buffer_infos = manager->get_buffer_infos().get();

        using buffer_infos_pair_type = std::pair<
            core::memory::buffer_manager::buffer_info_map_t::key_type,
            core::memory::buffer_manager::buffer_info_map_t::mapped_type
        >;
        using buffer_vector_type = std::vector<buffer_infos_pair_type>;

        buffer_vector_type buffers;

        for(const auto& elt : buffer_infos)
        {
            const core::memory::buffer_info& info = elt.second;
            if(!(info.size == 0 || info.lock_count() > 0 || !info.loaded))
            {
                buffers.emplace_back(elt);
            }
        }

        for(const auto& pair : buffer_infos)
        {
            if(dumped < _nb_of_bytes)
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
void valve_dump<TOOL>::apply(std::size_t _supplement)
{
    if(this->need_dump(_supplement))
    {
        this->dump(
            (m_min_free_mem + m_hysteresis_offset + _supplement)
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
bool valve_dump<TOOL>::set_param(const std::string& _name, const std::string& _value)
{
    SIGHT_INFO("Set " << _name << " to " << _value);
    try
    {
        if(_name == "min_free_mem")
        {
            m_min_free_mem = core::memory::byte_size(_value).size();
            return true;
        }

        if(_name == "hysteresis_offset")
        {
            m_hysteresis_offset = core::memory::byte_size(_value).size();
            return true;
        }
    }
    catch(core::memory::exception::bad_cast const&)
    {
        SIGHT_ERROR("Bad value for " << _name << " : " << _value);
        return false;
    }
    SIGHT_ERROR("Bad parameter name " << _name);
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
std::string valve_dump<TOOL>::get_param(const std::string& _name, bool* _ok) const
{
    bool is_ok = false;
    std::string value;
    if(_name == "min_free_mem")
    {
        value = std::string(core::memory::byte_size(static_cast<core::memory::byte_size::size_t>(m_min_free_mem)));
        is_ok = true;
    }
    else if(_name == "hysteresis_offset")
    {
        value = std::string(core::memory::byte_size(static_cast<core::memory::byte_size::size_t>(m_hysteresis_offset)));
        is_ok = true;
    }

    if(_ok != nullptr)
    {
        *_ok = is_ok;
    }

    return value;
}

} // namespace sight::core::memory::policy
