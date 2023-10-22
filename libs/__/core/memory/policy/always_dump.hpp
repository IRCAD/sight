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

#include "core/config.hpp"
#include "core/memory/buffer_info.hpp"
#include "core/memory/policy/base.hpp"
#include "core/memory/policy/factory/new.hpp"

#include <core/base.hpp>

namespace sight::core::memory::policy
{

/**
 * @brief Always dump policy
 *
 * This policy will dump buffers as often as possible. As soon as a buffer is
 * released (ie the lock count become zero), it will be dumped.
 */
class CORE_CLASS_API always_dump : public core::memory::policy::base
{
public:

    SIGHT_DECLARE_CLASS(always_dump, core::memory::policy::base);

    CORE_API void allocation_request(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer,
        buffer_info::size_t _size
    ) override;

    CORE_API void set_request(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer,
        buffer_info::size_t _size
    ) override;

    CORE_API void reallocate_request(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer,
        buffer_info::size_t _new_size
    ) override;

    CORE_API void destroy_request(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer
    ) override;

    CORE_API void lock_request(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer
    ) override;
    CORE_API void unlock_request(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer
    ) override;

    CORE_API void dump_success(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer
    ) override;
    CORE_API void restore_success(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer
    ) override;

    CORE_API void refresh() override;

    //------------------------------------------------------------------------------

    bool set_param(const std::string& _name, const std::string& _value) override
    {
        SIGHT_NOT_USED(_name);
        SIGHT_NOT_USED(_value);
        return false;
    }

    CORE_API std::string get_param(const std::string& _name, bool* _ok = nullptr) const override;

    //------------------------------------------------------------------------------

    const core::memory::policy::base::param_names_type& get_param_names() const override
    {
        static core::memory::policy::base::param_names_type names;
        return names;
    }

protected:

    static CORE_API std::size_t dump();
    CORE_API void apply();
};

} // namespace sight::core::memory::policy
