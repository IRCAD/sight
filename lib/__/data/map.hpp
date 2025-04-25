/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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

#include "data/container.hpp"
#include "data/object.hpp"

namespace sight::data
{

/// This class is an activity container
class SIGHT_DATA_CLASS_API map : public data::container<std::map<std::string,
                                                                 object::sptr> >
{
public:

    SIGHT_DECLARE_CLASS(map, container<map::container_t>);

    /// Destructor / Assignment operators
    /// @{
    SIGHT_DATA_API ~map() noexcept override = default;

    /// This will enable common collection constructors / assignment operators
    using container<map::container_t>::container;
    using container<map::container_t>::operator=;
    /// @}

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const map& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const map& _other) const noexcept;
    /// @}

    template<typename C = object>
    inline typename C::sptr get(const std::string& _key) const noexcept;

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    SIGHT_DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    SIGHT_DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;
};

//------------------------------------------------------------------------------

template<class C>
inline typename C::sptr map::get(const std::string& _key) const noexcept
{
    try
    {
        return std::dynamic_pointer_cast<C>(at(_key));
    }
    catch(...)
    {
        // Just ignore the error and return nullptr
    }

    return SPTR(C)();
}

} // namespace sight::data
