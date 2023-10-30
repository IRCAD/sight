/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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
#include "core/memory/buffer_manager.hpp"
#include "core/memory/policy/factory/new.hpp"

#include <core/base.hpp>

namespace sight::core::memory::policy
{

/**
 * @brief Defines the memory dump policy interface
 */
class CORE_CLASS_API base : public core::base_object
{
public:

    using sptr             = std::shared_ptr<base>;
    using param_names_type = std::vector<std::string>;

    virtual void allocation_request(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer,
        buffer_info::size_t _size
    ) = 0;

    virtual void set_request(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer,
        buffer_info::size_t _size
    ) = 0;

    virtual void reallocate_request(
        buffer_info& _info,
        core::memory::buffer_manager::const_buffer_ptr_t _buffer,
        buffer_info::size_t _new_size
    ) = 0;

    virtual void destroy_request(buffer_info& _info, core::memory::buffer_manager::const_buffer_ptr_t _buffer) = 0;

    virtual void lock_request(buffer_info& _info, core::memory::buffer_manager::const_buffer_ptr_t _buffer)   = 0;
    virtual void unlock_request(buffer_info& _info, core::memory::buffer_manager::const_buffer_ptr_t _buffer) = 0;

    virtual void dump_success(buffer_info& _info, core::memory::buffer_manager::const_buffer_ptr_t _buffer)    = 0;
    virtual void restore_success(buffer_info& _info, core::memory::buffer_manager::const_buffer_ptr_t _buffer) = 0;

    virtual void refresh() = 0;

    virtual bool set_param(const std::string& _name, const std::string& _value) = 0;
    virtual std::string get_param(const std::string& _name, bool* _ok           = nullptr) const = 0;
    virtual const param_names_type& get_param_names() const                     = 0;

    template<typename T>
    class registry
    {
    public:

        registry()
        {
            core::memory::policy::registry::get()->add_factory(
                T::leaf_classname(),
                &core::memory::policy::factory::create<T>
            );
        }
    };
};

} // namespace sight::core::memory::policy
