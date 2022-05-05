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

#pragma once

#include "data/IContainer.hpp"
#include "data/Object.hpp"

namespace sight::data
{

/// This class is an activity container
class DATA_CLASS_API Vector final : public IContainer<std::vector<Object::sptr> >
{
public:

    SIGHT_DECLARE_CLASS(Vector, IContainer<Vector::container_type>, factory::New<Vector>);

    /// Constructors / Destructor / Assignment operators
    /// @{
    DATA_API Vector(Object::Key key);
    DATA_API ~Vector() override = default;

    /// This will enable common collection constructors / assignment operators
    using IContainer<Vector::container_type>::IContainer;
    using IContainer<Vector::container_type>::operator=;
    /// @}

    /// Defines shallow copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param source the source object to copy
    DATA_API void shallowCopy(const Object::csptr& source) override;

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const Vector& other) const noexcept;
    DATA_API bool operator!=(const Vector& other) const noexcept;
    /// @}

protected:

    /// Defines deep copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void cachedDeepCopy(const Object::csptr& source, DeepCopyCacheType& cache) override;
};

} // namespace sight::data
