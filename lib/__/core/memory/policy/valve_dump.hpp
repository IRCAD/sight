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
#include "core/memory/tools/memory_monitor_tools.hpp"

#include <core/base.hpp>

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

    valve_dump();

    void allocation_request(
        buffer_info& info,
        core::memory::buffer_manager::const_buffer_ptr_t buffer,
        buffer_info::size_t size
    ) override;
    void set_request(
        buffer_info& info,
        core::memory::buffer_manager::const_buffer_ptr_t buffer,
        buffer_info::size_t size
    ) override;
    void reallocate_request(
        buffer_info& info,
        core::memory::buffer_manager::const_buffer_ptr_t buffer,
        buffer_info::size_t new_size
    ) override;

    void destroy_request(
        buffer_info& info,
        core::memory::buffer_manager::const_buffer_ptr_t buffer
    ) override;

    void lock_request(
        buffer_info& info,
        core::memory::buffer_manager::const_buffer_ptr_t buffer
    ) override;
    void unlock_request(
        buffer_info& info,
        core::memory::buffer_manager::const_buffer_ptr_t buffer
    ) override;

    void dump_success(
        buffer_info& info,
        core::memory::buffer_manager::const_buffer_ptr_t buffer
    ) override;
    void restore_success(
        buffer_info& info,
        core::memory::buffer_manager::const_buffer_ptr_t buffer
    ) override;

    void refresh() override;

    bool set_param(const std::string& name, const std::string& value) override;
    std::string get_param(const std::string& name, bool* ok = nullptr) const override;
    const core::memory::policy::base::param_names_type& get_param_names() const override;

protected:

    bool need_dump(std::size_t supplement) const;

    std::size_t dump(std::size_t nb_of_bytes);

    void apply(std::size_t supplement = 0);

    std::size_t m_min_free_mem;
    std::size_t m_hysteresis_offset {0};
};

} // namespace sight::core::memory::policy

#include "valve_dump.hxx"
