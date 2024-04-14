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

#include "data/plane_list.hpp"

#include "data/exception.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>

SIGHT_REGISTER_DATA(sight::data::plane_list);

namespace sight::data
{

const core::com::signals::key_t plane_list::PLANE_ADDED_SIG         = "planeAdded";
const core::com::signals::key_t plane_list::PLANE_REMOVED_SIG       = "planeRemoved";
const core::com::signals::key_t plane_list::VISIBILITY_MODIFIED_SIG = "visibilityModified";

//------------------------------------------------------------------------------

plane_list::plane_list()
{
    new_signal<plane_added_signal_t>(PLANE_ADDED_SIG);
    new_signal<plane_removed_signal_t>(PLANE_REMOVED_SIG);
    new_signal<visibility_modified_signal_t>(VISIBILITY_MODIFIED_SIG);
}

//------------------------------------------------------------------------------

void plane_list::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const plane_list>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !other
    );

    this->m_v_planes = other->m_v_planes;

    base_class_t::shallow_copy(other);
}

//------------------------------------------------------------------------------

void plane_list::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const plane_list>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !other
    );

    this->m_v_planes.clear();
    for(const auto& plane : other->m_v_planes)
    {
        this->m_v_planes.push_back(data::object::copy(plane, _cache));
    }

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

bool plane_list::operator==(const plane_list& _other) const noexcept
{
    if(!core::tools::is_equal(m_v_planes, _other.m_v_planes))
    {
        return false;
    }

    // Super class last
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool plane_list::operator!=(const plane_list& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data
