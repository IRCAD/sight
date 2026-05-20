/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#pragma once

#include <sight/data/config.hpp>

#include "data/container.hpp"
#include "data/point.hpp"

#include <vector>

namespace sight::data
{

/**
 * @brief   This class defines a list of points.
 * @see     Point
 */
class SIGHT_DATA_CLASS_API point_list final : public container<std::vector<data::point::sptr> >
{
public:

    /// This will enable common collection constructors / assignment operators
    using container<point_list::container_t>::container;
    using container<point_list::container_t>::operator=;

    SIGHT_DECLARE_CLASS(point_list, container<point_list::container_t>);

    struct signals
    {
        static inline const core::com::signals::key_t POINT_ADDED   = "point_added";
        static inline const core::com::signals::key_t POINT_REMOVED = "point_removed";

        using point_added_t   = core::com::signal<void (point::sptr)>;
        using point_removed_t = core::com::signal<void (point::sptr)>;
    };

    SIGHT_DATA_API point_list();
    SIGHT_DATA_API ~point_list() noexcept override = default;

    /**
     * @brief: Deletes a point at the specified index
     * @param[in] _index Index of point to delete
     **/
    void remove(std::size_t _index);

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const point_list& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const point_list& _other) const noexcept;
    /// @}

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

//-----------------------------------------------------------------------------

inline void point_list::remove(std::size_t _index)
{
    const auto it = this->begin() + static_cast<ptrdiff_t>(_index);
    this->erase(it);
}

SIGHT_DATA_API std::ostream& operator<<(std::ostream& _out, const point_list& _pl);

} // end namespace sight::data
