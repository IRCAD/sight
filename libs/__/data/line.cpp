/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/line.hpp"

#include "data/exception.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>

SIGHT_REGISTER_DATA(sight::data::line);

namespace sight::data
{

//------------------------------------------------------------------------------

void line::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const line>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_position  = other->m_position;
    m_direction = other->m_direction;

    base_class_t::shallow_copy(other);
}

//------------------------------------------------------------------------------

void line::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const line>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_position  = data::object::copy(other->m_position, _cache);
    m_direction = data::object::copy(other->m_direction, _cache);

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

void line::set_value(const data::point::sptr& _position, const data::point::sptr& _direction)
{
    m_position  = _position;
    m_direction = _direction;
}

//------------------------------------------------------------------------------

bool line::operator==(const line& _other) const noexcept
{
    if(!core::tools::is_equal(m_position, _other.m_position)
       || !core::tools::is_equal(m_direction, _other.m_direction))
    {
        return false;
    }

    // Super class last
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool line::operator!=(const line& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data
