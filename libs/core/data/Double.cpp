/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "data/Double.hpp"

#include "data/Exception.hpp"
#include "data/GenericField.hpp"
#include "data/registry/macros.hpp"

SIGHT_REGISTER_DATA(sight::data::Double);

namespace sight::data
{

//------------------------------------------------------------------------------

Double::Double(data::Object::Key) noexcept
{
}

//------------------------------------------------------------------------------
void Double::shallowCopy(const Object::csptr& _source)
{
    Double::csptr other = Double::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldShallowCopy(_source);
    m_value = other->m_value;
}

//------------------------------------------------------------------------------

void Double::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Double::csptr other = Double::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldDeepCopy(_source, cache);
    m_value = other->m_value;
}

} // namespace sight::data
