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

#pragma once

#include "data/IContainer.hpp"
#include "data/Object.hpp"

namespace sight::data
{

/// This class is an activity container
class DATA_CLASS_API Composite : public IContainer<std::map<std::string,
                                                            Object::sptr> >
{
public:

    SIGHT_DECLARE_CLASS(Composite, IContainer<Composite::container_type>, factory::New<Composite>);

    /// Constructors / Destructor / Assignment operators
    /// @{
    DATA_API Composite(Object::Key key);
    DATA_API ~Composite() noexcept override = default;

    /// This will enable common collection constructors / assignment operators
    using IContainer<Composite::container_type>::IContainer;
    using IContainer<Composite::container_type>::operator=;
    /// @}

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const Composite& other) const noexcept;
    DATA_API bool operator!=(const Composite& other) const noexcept;
    /// @}

    template<typename C = Object>
    inline typename C::sptr get(const std::string& key) const noexcept;

    /// Defines shallow copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallowCopy(const Object::csptr& source) override;

    /// Defines deep copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deepCopy(
        const Object::csptr& source,
        const std::unique_ptr<DeepCopyCacheType>& cache = std::make_unique<DeepCopyCacheType>()
    ) override;
};

//------------------------------------------------------------------------------

template<class C>
inline typename C::sptr Composite::get(const std::string& key) const noexcept
{
    try
    {
        return C::dynamicCast(at(key));
    }
    catch(...)
    {
        // Just ignore the error and return nullptr
    }

    return SPTR(C)();
}

} // namespace sight::data
