/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "viz/scene2d/data/Viewport.hpp"

#include <data/registry/macros.hpp>

SIGHT_REGISTER_DATA(sight::viz::scene2d::data::Viewport);

namespace sight::viz::scene2d
{

namespace data
{

//------------------------------------------------------------------------------

void Viewport::shallowCopy(const sight::data::Object::csptr& _source)
{
    Viewport::csptr other = Viewport::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        sight::data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldShallowCopy(_source);

    m_x      = other->m_x;
    m_y      = other->m_y;
    m_width  = other->m_width;
    m_height = other->m_height;
}

//------------------------------------------------------------------------------

void Viewport::cachedDeepCopy(const sight::data::Object::csptr& _source, DeepCopyCacheType& cache)
{
    Viewport::csptr other = Viewport::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        sight::data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldDeepCopy(_source, cache);

    m_x      = other->m_x;
    m_y      = other->m_y;
    m_width  = other->m_width;
    m_height = other->m_height;
}

} // namespace data

} // namespace sight::viz::scene2d
