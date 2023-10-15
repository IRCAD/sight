/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "data/config.hpp"
#include "data/object.hpp"

#include <array>

namespace sight::data
{

/**
 * @brief   Stores a map of optical 2D markers.
 *
 * The number of points for each marker is free, so the MarkerType is just a vector of 2D points and there is no
 * coherency checks between the markers, so you could have one marker with one point, another one with four points,
 * etc...
 */
class DATA_CLASS_API marker_map final : public object
{
public:

    SIGHT_DECLARE_CLASS(marker_map, object);

    using key_t      = std::string;
    using PointType  = std::array<float, 2>;
    using MarkerType = std::vector<PointType>;

    /**
     * @brief Destructor
     */
    DATA_API ~marker_map() noexcept override = default;

    /// Returns a marker given its identifier, null_ptr if not found
    DATA_API const MarkerType* getMarker(const key_t& _id) const;

    /// Returns a marker given its identifier, null_ptr if not found
    DATA_API MarkerType* getMarker(const key_t& _id);

    /// Returns a marker given its index in the container, asserts if not found
    DATA_API const MarkerType& getMarker(std::size_t index) const;

    /// Returns a marker given its index in the container, asserts if not found
    DATA_API MarkerType& getMarker(std::size_t index);

    /// Returns the number of markers in the container
    DATA_API std::size_t count() const;

    /// Adds a new marker in the container
    DATA_API void setMarker(const key_t& _id, const MarkerType& _marker);

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const marker_map& other) const noexcept;
    DATA_API bool operator!=(const marker_map& other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const object::csptr& source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& source,
        const std::unique_ptr<deep_copy_cache_t>& cache = std::make_unique<deep_copy_cache_t>()
    ) override;

private:

    typedef std::map<key_t, MarkerType> ContainerType;

    /// Map containing the markers, sorted by their identifier
    ContainerType m_markers;
};

} // namespace sight::data
