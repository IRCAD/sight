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

#include "Composite.hpp"

#include "data/registry/macros.hpp"

#include <core/com/Signal.hxx>

SIGHT_REGISTER_DATA(sight::data::Composite);

namespace sight::data
{

Composite::Composite(Object::Key key) :
    IContainer<Composite::container_type>(key)
{
}

//------------------------------------------------------------------------------

void Composite::shallowCopy(const Object::csptr& source)
{
    const auto& other = Composite::dynamicCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>")) + " to " + getClassname()
        ),
        !other
    );

    BaseClass::shallowCopy(other);
}

//------------------------------------------------------------------------------

bool Composite::operator==(const Composite& other) const noexcept
{
    return BaseClass::operator==(other);
}

//------------------------------------------------------------------------------

bool Composite::operator!=(const Composite& other) const noexcept
{
    return BaseClass::operator!=(other);
}

//------------------------------------------------------------------------------

void Composite::deepCopy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = Composite::dynamicCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>")) + " to " + getClassname()
        ),
        !other
    );

    BaseClass::deepCopy(other, cache);
}

} // namespace sight::data
