/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "data/object.hpp"

#include <array>

namespace sight::data
{

/**
 * @brief   Stores a map of optical 2D markers.
 *
 * The number of points for each marker is free, so the marker_t is just a vector of 2D points and there is no
 * coherency checks between the markers, so you could have one marker with one point, another one with four points,
 * etc...
 */
class SIGHT_DATA_CLASS_API marker_map final : public object
{
public:

    SIGHT_DECLARE_CLASS(marker_map, object);

    using key_t    = std::string;
    using point_t  = std::array<float, 2>;
    using marker_t = std::vector<point_t>;

    /**
     * @brief Destructor
     */
    SIGHT_DATA_API ~marker_map() noexcept override = default;

    /// Returns a marker given its identifier, null_ptr if not found
    SIGHT_DATA_API const marker_t* get_marker(const key_t& _id) const;

    /// Returns a marker given its identifier, null_ptr if not found
    SIGHT_DATA_API marker_t* get_marker(const key_t& _id);

    /// Returns a marker given its index in the container, asserts if not found
    SIGHT_DATA_API const marker_t& get_marker(std::size_t _index) const;

    /// Returns a marker given its index in the container, asserts if not found
    SIGHT_DATA_API marker_t& get_marker(std::size_t _index);

    /// Returns the number of markers in the container
    SIGHT_DATA_API std::size_t count() const;

    /// Adds a new marker in the container
    SIGHT_DATA_API void set_marker(const key_t& _id, const marker_t& _marker);

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const marker_map& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const marker_map& _other) const noexcept;
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

private:

    using container_t = std::map<key_t, marker_t>;

    /// Map containing the markers, sorted by their identifier
    container_t m_markers;
};

} // namespace sight::data
