/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
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

#include "arData/config.hpp"

#include <fwData/Object.hpp>

#include <array>

namespace arData
{

/**
 * @brief   Stores a map of optical 2D markers.
 *
 * The number of points for each marker is free, so the MarkerType is just a vector of 2D points and there is no
 * coherency checks between the markers, so you could have one marker with one point, another one with four points,
 * etc...
 */
class ARDATA_CLASS_API MarkerMap : public ::fwData::Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (MarkerMap)(::fwData::Object), ::fwData::factory::New< MarkerMap >)

    typedef std::string KeyType;
    typedef std::array<float, 2> PointType;
    typedef std::vector< PointType > MarkerType;

    /**
     * @brief Constructor
     * @param[in] _key Private construction key
     */
    ARDATA_API MarkerMap(::fwData::Object::Key _key);
    /**
     * @brief Destructor
     */
    ARDATA_API virtual ~MarkerMap() override;

    /// Defines shallow copy
    ARDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    ARDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& _cache) override;

    /// Returns a marker given its identifier, null_ptr if not found
    ARDATA_API const MarkerType* getMarker(const KeyType& _id) const;

    /// Returns a marker given its identifier, null_ptr if not found
    ARDATA_API MarkerType* getMarker(const KeyType& _id);

    /// Returns a marker given its index in the container, asserts if not found
    ARDATA_API const MarkerType& getMarker(size_t index) const;

    /// Returns a marker given its index in the container, asserts if not found
    ARDATA_API MarkerType& getMarker(size_t index);

    /// Returns the number of markers in the container
    ARDATA_API size_t count() const;

    /// Adds a new marker in the container
    ARDATA_API void setMarker(const KeyType& _id, const MarkerType& _marker );

private:
    typedef std::map< KeyType, MarkerType > ContainerType;

    /// Map containing the markers, sorted by their identifier
    ContainerType m_markers;
};

} // namespace arData
