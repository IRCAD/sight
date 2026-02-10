/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "data/point_list.hpp"

#include "data/exception.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>

SIGHT_REGISTER_DATA(sight::data::point_list);

namespace sight::data
{

//------------------------------------------------------------------------------

point_list::point_list()
{
    new_signal<signals::point_added_t>(signals::POINT_ADDED);
    new_signal<signals::point_removed_t>(signals::POINT_REMOVED);
}

//------------------------------------------------------------------------------

void point_list::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const point_list>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    base_class_t::shallow_copy(other);
}

//------------------------------------------------------------------------------

void point_list::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const point_list>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

bool point_list::operator==(const point_list& _other) const noexcept
{
    if(!core::is_equal(*this, _other))
    {
        return false;
    }

    // Super class last
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool point_list::operator!=(const point_list& _other) const noexcept
{
    return !(*this == _other);
}

//------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& _out, const sight::data::point_list& _pl)
{
    _out << "{";
    bool first = true;
    for(const auto& e : _pl)
    {
        if(!first)
        {
            _out << ", ";
        }

        _out << *e;
        first = false;
    }

    return _out << "}";
}

} // namespace sight::data
