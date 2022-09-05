/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "data/Line.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>

SIGHT_REGISTER_DATA(sight::data::Line);

namespace sight::data
{

//------------------------------------------------------------------------------

Line::Line(data::Object::Key /*unused*/)
{
}

//------------------------------------------------------------------------------

Line::~Line()
= default;

//------------------------------------------------------------------------------

void Line::shallowCopy(const Object::csptr& _source)
{
    Line::csptr other = Line::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldShallowCopy(_source);
    m_position  = other->m_position;
    m_direction = other->m_direction;
}

//------------------------------------------------------------------------------

void Line::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Line::csptr other = Line::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldDeepCopy(_source, cache);
    m_position  = data::Object::copy(other->m_position, cache);
    m_direction = data::Object::copy(other->m_direction, cache);
}

//------------------------------------------------------------------------------

void Line::setValue(const data::Point::sptr& _position, const data::Point::sptr& _direction)
{
    m_position  = _position;
    m_direction = _direction;
}

//------------------------------------------------------------------------------

bool Line::operator==(const Line& other) const noexcept
{
    if(!core::tools::is_equal(m_position, other.m_position)
       || !core::tools::is_equal(m_direction, other.m_direction))
    {
        return false;
    }

    // Super class last
    return Object::operator==(other);
}

//------------------------------------------------------------------------------

bool Line::operator!=(const Line& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
