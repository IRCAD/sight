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

#include "viz/scene2d/data/viewport.hpp"

#include <data/registry/macros.hpp>

SIGHT_REGISTER_DATA(sight::viz::scene2d::data::viewport);

namespace sight::viz::scene2d::data
{

//------------------------------------------------------------------------------

void viewport::shallow_copy(const sight::data::object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const viewport>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        sight::data::exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_x      = other->m_x;
    m_y      = other->m_y;
    m_width  = other->m_width;
    m_height = other->m_height;

    base_class_t::shallow_copy(other);
}

//------------------------------------------------------------------------------

void viewport::deep_copy(
    const sight::data::object::csptr& _source,
    const std::unique_ptr<sight::data::object::deep_copy_cache_t>& _cache
)
{
    const auto& other = std::dynamic_pointer_cast<const viewport>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        sight::data::exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_x      = other->m_x;
    m_y      = other->m_y;
    m_width  = other->m_width;
    m_height = other->m_height;

    base_class_t::deep_copy(other, _cache);
}

} // namespace sight::viz::scene2d::data
