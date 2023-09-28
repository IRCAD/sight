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

#include "core/memory/policy/barrier_dump.hpp"

#include "core/memory/byte_size.hpp"
#include "core/memory/exception/bad_cast.hpp"
#include "core/memory/policy/registry/macros.hpp"

namespace sight::core::memory::policy
{

SIGHT_REGISTER_MEMORY_POLICY(core::memory::policy::barrier_dump);

//------------------------------------------------------------------------------

barrier_dump::barrier_dump() :

    m_barrier(1024LL * 1024 * 500)
{
}

//------------------------------------------------------------------------------

void barrier_dump::allocation_request(
    buffer_info& info,
    core::memory::buffer_manager::const_buffer_ptr_t /*buffer*/,
    buffer_info::size_t size
)
{
    SIGHT_ASSERT("Memory allocation inconsistency", m_total_allocated >= info.size);
    m_total_allocated -= info.size;
    m_total_allocated += size;
    if(!info.loaded)
    {
        SIGHT_ASSERT("Memory dump inconsistency", m_total_dumped >= info.size);
        m_total_dumped -= info.size;
    }

    this->apply();
}

//------------------------------------------------------------------------------

void barrier_dump::set_request(
    buffer_info& info,
    core::memory::buffer_manager::const_buffer_ptr_t /*buffer*/,
    buffer_info::size_t size
)
{
    SIGHT_ASSERT("Memory allocation inconsistency", m_total_allocated >= info.size);
    m_total_allocated -= info.size;
    m_total_allocated += size;
    if(!info.loaded)
    {
        SIGHT_ASSERT("Memory dump inconsistency", m_total_dumped >= info.size);
        m_total_dumped -= info.size;
    }

    this->apply();
}

//------------------------------------------------------------------------------

void barrier_dump::reallocate_request(
    buffer_info& info,
    core::memory::buffer_manager::const_buffer_ptr_t /*buffer*/,
    buffer_info::size_t new_size
)
{
    SIGHT_ASSERT("Memory allocation inconsistency", m_total_allocated >= info.size);
    m_total_allocated -= info.size;
    m_total_allocated += new_size;
    if(!info.loaded)
    {
        SIGHT_ASSERT("Memory dump inconsistency", m_total_dumped >= info.size);
        m_total_dumped -= info.size;
    }

    this->apply();
}

//------------------------------------------------------------------------------

void barrier_dump::destroy_request(buffer_info& info, core::memory::buffer_manager::const_buffer_ptr_t /*buffer*/)
{
    if(!info.loaded)
    {
        SIGHT_ASSERT("Memory dump inconsistency", m_total_dumped >= info.size);
        m_total_dumped -= info.size;
    }

    SIGHT_ASSERT("Memory allocation inconsistency", m_total_allocated >= info.size);
    m_total_allocated -= info.size;
}

//------------------------------------------------------------------------------

void barrier_dump::lock_request(buffer_info& /*info*/, core::memory::buffer_manager::const_buffer_ptr_t /*buffer*/)
{
}

//------------------------------------------------------------------------------

void barrier_dump::unlock_request(buffer_info& /*info*/, core::memory::buffer_manager::const_buffer_ptr_t /*buffer*/)
{
    this->apply();
}

//------------------------------------------------------------------------------

void barrier_dump::dump_success(buffer_info& info, core::memory::buffer_manager::const_buffer_ptr_t /*buffer*/)
{
    m_total_dumped += info.size;
}

//------------------------------------------------------------------------------

void barrier_dump::restore_success(buffer_info& info, core::memory::buffer_manager::const_buffer_ptr_t /*buffer*/)
{
    SIGHT_ASSERT("Memory dump inconsistency", m_total_dumped >= info.size);
    m_total_dumped -= info.size;
}

//------------------------------------------------------------------------------

std::size_t barrier_dump::get_total_alive() const
{
    SIGHT_ASSERT("More dumped data than allocated data.", m_total_allocated >= m_total_dumped);
    std::size_t total_alive = m_total_allocated - m_total_dumped;
    return total_alive;
}

//------------------------------------------------------------------------------

bool barrier_dump::is_barrier_crossed() const
{
    return m_barrier < get_total_alive();
}

//------------------------------------------------------------------------------

std::size_t barrier_dump::dump(std::size_t nb_of_bytes)
{
    std::size_t dumped = 0;

    core::memory::buffer_manager::sptr manager = core::memory::buffer_manager::get();
    if(manager)
    {
        const core::memory::buffer_manager::buffer_info_map_t buffer_infos = manager->get_buffer_infos().get();

        using buffer_infos_pair_type = std::pair<core::memory::buffer_manager::buffer_info_map_t::key_type,
                                                 core::memory::buffer_manager::buffer_info_map_t::mapped_type>;
        using buffer_vector_type = std::vector<buffer_infos_pair_type>;

        buffer_vector_type buffers;

        for(const core::memory::buffer_manager::buffer_info_map_t::value_type& elt : buffer_infos)
        {
            const core::memory::buffer_info& info = elt.second;
            if(!(info.size == 0 || info.lock_count() > 0 || !info.loaded))
            {
                buffers.emplace_back(elt);
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

void barrier_dump::apply()
{
    if(this->is_barrier_crossed())
    {
        this->dump(this->get_total_alive() - m_barrier);
    }
}

//------------------------------------------------------------------------------

void barrier_dump::refresh()
{
    core::memory::buffer_manager::sptr manager       = core::memory::buffer_manager::get();
    core::memory::buffer_manager::buffer_stats stats = manager->get_buffer_stats().get();
    m_total_allocated = stats.total_managed;
    m_total_dumped    = stats.total_dumped;
    this->apply();
}

//------------------------------------------------------------------------------

bool barrier_dump::set_param(const std::string& name, const std::string& value)
{
    try
    {
        if(name == "barrier")
        {
            m_barrier = core::memory::byte_size(value).size();
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

const core::memory::policy::base::param_names_type& barrier_dump::get_param_names() const
{
    static const core::memory::policy::base::param_names_type params = {{"barrier"}};
    return params;
}

//------------------------------------------------------------------------------

std::string barrier_dump::get_param(const std::string& name, bool* ok) const
{
    bool is_ok = false;
    std::string value;
    if(name == "barrier")
    {
        value = std::string(core::memory::byte_size(core::memory::byte_size::size_t(m_barrier)));
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
