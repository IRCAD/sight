/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
    fwCoreClassDefinitionsWithFactoryMacro( (MarkerMap)(::fwData::Object), (()), ::fwData::factory::New< MarkerMap >)

    typedef std::string KeyType;
    typedef std::array<float, 2> PointType;
    typedef std::vector< PointType > MarkerType;

    /**
     * @brief Constructor
     * @param[in] _key Private construction key
     */
    FWDATA_API MarkerMap(::fwData::Object::Key _key);
    /**
     * @brief Destructor
     */
    FWDATA_API virtual ~MarkerMap() override;

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& _cache) override;

    /// Returns a marker given its identifier, null_ptr if not found
    FWDATA_API const MarkerType* getMarker(const KeyType& _id) const;

    /// Returns a marker given its identifier, null_ptr if not found
    FWDATA_API MarkerType* getMarker(const KeyType& _id);

    /// Returns a marker given its index in the container, asserts if not found
    FWDATA_API const MarkerType& getMarker(size_t index) const;

    /// Returns a marker given its index in the container, asserts if not found
    FWDATA_API MarkerType& getMarker(size_t index);

    /// Returns the number of markers in the container
    FWDATA_API size_t count() const;

    /// Adds a new marker in the container
    FWDATA_API void setMarker(const KeyType& _id, const MarkerType& _marker );

private:
    typedef std::map< KeyType, MarkerType > ContainerType;

    /// Map containing the markers, sorted by their identifier
    ContainerType m_markers;
};

} // namespace arData
