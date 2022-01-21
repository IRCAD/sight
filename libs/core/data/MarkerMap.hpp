/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
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

#include <data/Object.hpp>

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
class DATA_CLASS_API MarkerMap : public data::Object
{
public:

    SIGHT_DECLARE_CLASS(MarkerMap, data::Object, data::factory::New<MarkerMap>);

    typedef std::string KeyType;
    typedef std::array<float, 2> PointType;
    typedef std::vector<PointType> MarkerType;

    /**
     * @brief Constructor
     * @param[in] _key Private construction key
     */
    DATA_API MarkerMap(data::Object::Key _key);
    /**
     * @brief Destructor
     */
    DATA_API ~MarkerMap() override;

    /// Defines shallow copy
    DATA_API void shallowCopy(const Object::csptr& _source) override;

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& _cache) override;

    /// Returns a marker given its identifier, null_ptr if not found
    DATA_API const MarkerType* getMarker(const KeyType& _id) const;

    /// Returns a marker given its identifier, null_ptr if not found
    DATA_API MarkerType* getMarker(const KeyType& _id);

    /// Returns a marker given its index in the container, asserts if not found
    DATA_API const MarkerType& getMarker(std::size_t index) const;

    /// Returns a marker given its index in the container, asserts if not found
    DATA_API MarkerType& getMarker(std::size_t index);

    /// Returns the number of markers in the container
    DATA_API std::size_t count() const;

    /// Adds a new marker in the container
    DATA_API void setMarker(const KeyType& _id, const MarkerType& _marker);

private:

    typedef std::map<KeyType, MarkerType> ContainerType;

    /// Map containing the markers, sorted by their identifier
    ContainerType m_markers;
};

} // namespace sight::data
