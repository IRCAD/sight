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

#include <sight/core/config.hpp>

#include "core/memory/buffer_info.hpp"
#include "core/memory/buffer_manager.hpp"
#include "core/memory/policy/base.hpp"
#include "core/memory/policy/factory/new.hpp"

#include <core/base.hpp>

namespace sight::core::memory::policy
{

/**
 * @brief Barrier dump policy
 *
 * This policy defines a memory usage barrier and will try to keep the managed
 * buffers memory usage under this barrier.
 */
class SIGHT_CORE_CLASS_API barrier_dump : public core::memory::policy::base
{
public:

    SIGHT_DECLARE_CLASS(barrier_dump, core::memory::policy::base);

    SIGHT_CORE_API barrier_dump();

    SIGHT_CORE_API void allocation_request(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer,
        buffer_info::size_t _size
    ) override;

    SIGHT_CORE_API void set_request(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer,
        buffer_info::size_t _size
    ) override;

    SIGHT_CORE_API void reallocate_request(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer,
        buffer_info::size_t _new_size
    ) override;

    SIGHT_CORE_API void destroy_request(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer
    ) override;

    SIGHT_CORE_API void lock_request(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer
    ) override;
    SIGHT_CORE_API void unlock_request(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer
    ) override;

    SIGHT_CORE_API void dump_success(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer
    ) override;
    SIGHT_CORE_API void restore_success(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer
    ) override;

    SIGHT_CORE_API void refresh() override;

    //------------------------------------------------------------------------------

    void set_barrier(std::size_t _barrier)
    {
        m_barrier = _barrier;
    }

    //------------------------------------------------------------------------------

    std::size_t get_barrier() const
    {
        return m_barrier;
    }

    SIGHT_CORE_API std::string get_param(const std::string& _name, bool* _ok = nullptr) const override;
    SIGHT_CORE_API bool set_param(const std::string& _name, const std::string& _value) override;
    SIGHT_CORE_API const core::memory::policy::base::param_names_type& get_param_names() const override;

protected:

    SIGHT_CORE_API std::size_t get_total_alive() const;
    SIGHT_CORE_API bool is_barrier_crossed() const;

    static SIGHT_CORE_API std::size_t dump(std::size_t _nb_of_bytes);

    SIGHT_CORE_API void apply();

    std::size_t m_total_allocated {0};
    std::size_t m_total_dumped {0};
    std::size_t m_barrier;
};

} // namespace sight::core::memory::policy
