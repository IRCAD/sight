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

#include "data/point_list.hpp"

#include "data/exception.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>

SIGHT_REGISTER_DATA(sight::data::point_list);

namespace sight::data
{

const core::com::signals::key_t point_list::POINT_ADDED_SIG   = "pointAdded";
const core::com::signals::key_t point_list::POINT_REMOVED_SIG = "pointRemoved";

//------------------------------------------------------------------------------

point_list::point_list()
{
    new_signal<PointAddedSignalType>(POINT_ADDED_SIG);
    new_signal<PointRemovedSignalType>(POINT_REMOVED_SIG);
}

//------------------------------------------------------------------------------

void point_list::shallow_copy(const object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const point_list>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_vPoints = other->m_vPoints;

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void point_list::deep_copy(const object::csptr& source, const std::unique_ptr<deep_copy_cache_t>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const point_list>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_vPoints.clear();
    for(const PointListContainer::value_type& point : other->m_vPoints)
    {
        m_vPoints.push_back(data::object::copy(point, cache));
    }

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

bool point_list::operator==(const point_list& other) const noexcept
{
    if(!core::tools::is_equal(m_vPoints, other.m_vPoints))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool point_list::operator!=(const point_list& other) const noexcept
{
    return !(*this == other);
}

//------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, const sight::data::point_list& pl)
{
    out << "{";
    bool first = true;
    for(const auto& e : pl.getPoints())
    {
        if(!first)
        {
            out << ", ";
        }

        out << *e;
        first = false;
    }

    return out << "}";
}

} // namespace sight::data
