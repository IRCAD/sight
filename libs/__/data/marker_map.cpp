/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "data/marker_map.hpp"

#include <data/exception.hpp>
#include <data/registry/macros.hpp>

namespace sight::data
{

SIGHT_REGISTER_DATA(sight::data::marker_map)

//------------------------------------------------------------------------------

void marker_map::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const marker_map>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_markers = other->m_markers;

    base_class_t::shallow_copy(other);
}

//------------------------------------------------------------------------------

void marker_map::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const marker_map>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_markers = other->m_markers;

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

const marker_map::marker_t* marker_map::get_marker(const key_t& _id) const
{
    auto it = m_markers.find(_id);
    if(it != m_markers.end())
    {
        return &it->second;
    }

    return nullptr;
}

//------------------------------------------------------------------------------

marker_map::marker_t* marker_map::get_marker(const key_t& _id)
{
    auto it = m_markers.find(_id);
    if(it != m_markers.end())
    {
        return &it->second;
    }

    return nullptr;
}

//------------------------------------------------------------------------------

const marker_map::marker_t& marker_map::get_marker(std::size_t _index) const
{
    SIGHT_ASSERT("Marker map is empty", !m_markers.empty());
    SIGHT_ASSERT("Index is bigger than map's size", _index < m_markers.size());

    auto it = m_markers.begin();
    std::advance(it, static_cast<container_t::iterator::difference_type>(_index));
    return it->second;
}

//------------------------------------------------------------------------------

marker_map::marker_t& marker_map::get_marker(std::size_t _index)
{
    SIGHT_ASSERT("Marker map is empty", !m_markers.empty());
    SIGHT_ASSERT("Index is bigger than map's size", _index < m_markers.size());

    auto it = m_markers.begin();
    std::advance(it, static_cast<container_t::iterator::difference_type>(_index));
    return it->second;
}

//------------------------------------------------------------------------------

std::size_t marker_map::count() const
{
    return m_markers.size();
}

//------------------------------------------------------------------------------

void marker_map::set_marker(const key_t& _id, const marker_map::marker_t& _marker)
{
    m_markers[_id] = _marker;
}

//------------------------------------------------------------------------------

bool marker_map::operator==(const marker_map& _other) const noexcept
{
    if(!core::is_equal(m_markers, _other.m_markers))
    {
        return false;
    }

    // Super class last
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool marker_map::operator!=(const marker_map& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data
