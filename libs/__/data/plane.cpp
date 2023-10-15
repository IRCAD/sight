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

#include "data/plane.hpp"

#include "data/exception.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>

SIGHT_REGISTER_DATA(sight::data::plane);

namespace sight::data
{

const core::com::signals::key_t plane::SELECTED_SIG = "selected";

//------------------------------------------------------------------------------

plane::plane()
{
    m_vPoints[0] = std::make_shared<data::point>();
    m_vPoints[1] = std::make_shared<data::point>();
    m_vPoints[2] = std::make_shared<data::point>();

    new_signal<SelectedSignalType>(SELECTED_SIG);
}

//------------------------------------------------------------------------------

void plane::shallow_copy(const object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const plane>(source);

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

void plane::deep_copy(const object::csptr& source, const std::unique_ptr<deep_copy_cache_t>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const plane>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_vPoints[0] = data::object::copy(other->m_vPoints[0], cache);
    m_vPoints[1] = data::object::copy(other->m_vPoints[1], cache);
    m_vPoints[2] = data::object::copy(other->m_vPoints[2], cache);

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

void plane::setValue(data::point::sptr _point1, data::point::sptr _point2, data::point::sptr _point3)
{
    m_vPoints[0] = _point1;
    m_vPoints[1] = _point2;
    m_vPoints[2] = _point3;
}

//------------------------------------------------------------------------------

bool plane::operator==(const plane& other) const noexcept
{
    if(m_isIntersection != other.m_isIntersection
       || !core::tools::is_equal(m_vPoints, other.m_vPoints))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool plane::operator!=(const plane& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
