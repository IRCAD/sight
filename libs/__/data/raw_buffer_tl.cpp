/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "data/raw_buffer_tl.hpp"

#include "data/timeline/raw_buffer.hpp"

#include <data/exception.hpp>
#include <data/registry/macros.hpp>

#include <functional>

SIGHT_REGISTER_DATA(sight::data::raw_buffer_tl);

namespace sight::data
{

//------------------------------------------------------------------------------

void raw_buffer_tl::shallow_copy(const object::csptr& /*source*/)
{
    SIGHT_FATAL("shallow_copy not implemented for : " + this->get_classname());
}

//------------------------------------------------------------------------------

void raw_buffer_tl::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const raw_buffer_tl>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    this->clear_timeline();
    this->alloc_pool_size(other->m_pool->get_requested_size());

    for(const auto& elt : other->m_timeline)
    {
        SPTR(data::timeline::raw_buffer) tl_obj = this->create_buffer(elt.first);
        tl_obj->deep_copy(*elt.second);
        m_timeline.insert(timeline_t::value_type(elt.first, tl_obj));
    }

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

CSPTR(data::timeline::raw_buffer) raw_buffer_tl::get_closest_buffer(
    core::clock::type _timestamp,
    timeline::direction_t _direction
) const
{
    CSPTR(data::timeline::object) buffer = this->get_closest_object(_timestamp, _direction);
    return std::dynamic_pointer_cast<const data::timeline::raw_buffer>(buffer);
}

//------------------------------------------------------------------------------

CSPTR(data::timeline::raw_buffer) raw_buffer_tl::get_buffer(core::clock::type _timestamp) const
{
    CSPTR(data::timeline::object) buffer = this->get_object(_timestamp);
    return std::dynamic_pointer_cast<const data::timeline::raw_buffer>(buffer);
}

//------------------------------------------------------------------------------

void raw_buffer_tl::init_pool_size(std::size_t _size)
{
    this->alloc_pool_size(_size);
}

//------------------------------------------------------------------------------

SPTR(data::timeline::object) raw_buffer_tl::create_object(core::clock::type _timestamp)
{
    return this->create_buffer(_timestamp);
}

//------------------------------------------------------------------------------

SPTR(data::timeline::raw_buffer) raw_buffer_tl::create_buffer(core::clock::type _timestamp)
{
    return std::make_shared<data::timeline::raw_buffer>(
        _timestamp,
        reinterpret_cast<data::timeline::buffer::buffer_data_t>(m_pool->malloc()),
        m_pool->get_requested_size(),
        [object_ptr = m_pool](auto&& _p_h1, auto&& ...){object_ptr->free(std::forward<decltype(_p_h1)>(_p_h1));});
}

//------------------------------------------------------------------------------

bool raw_buffer_tl::is_object_valid(const CSPTR(data::timeline::object)& _obj) const
{
    CSPTR(data::timeline::raw_buffer) src_obj =
        std::dynamic_pointer_cast<const data::timeline::raw_buffer>(_obj);
    return src_obj != nullptr;
}

//------------------------------------------------------------------------------

} // namespace sight::data
