/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "Set.hpp"

#include "data/container.hxx"
#include "data/registry/macros.hpp"

#include <core/com/signal.hxx>

SIGHT_REGISTER_DATA(sight::data::Set);

namespace sight::data
{

//------------------------------------------------------------------------------

void Set::shallow_copy(const Object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const Set>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>")) + " to " + get_classname()
        ),
        !other
    );

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

bool Set::operator==(const Set& other) const noexcept
{
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool Set::operator!=(const Set& other) const noexcept
{
    return base_class::operator!=(other);
}

//------------------------------------------------------------------------------

void Set::deep_copy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const Set>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>")) + " to " + get_classname()
        ),
        !other
    );

    base_class::deep_copy(other, cache);
}

} // namespace sight::data
