/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "fwMedData/config.hpp"
#include "fwMedData/Series.hpp"

#include <fwData/factory/new.hpp>

#include <array>
#include <map>

fwCampAutoDeclareDataMacro((fwMedData)(NavigationSeries), FWMEDDATA_API)

namespace fwMedData
{

class Series;

/**
 * @brief Holds a navigation information.
 */
class FWMEDDATA_CLASS_API NavigationSeries : public ::fwMedData::Series
{

public:

    fwCoreClassMacro(NavigationSeries, ::fwMedData::Series, ::fwData::factory::New< NavigationSeries >)

    fwCampMakeFriendDataMacro((fwMedData)(NavigationSeries))

    typedef std::array<double, 3> CoordinateType;
    typedef double TimestampType;
    typedef std::map<TimestampType, CoordinateType> ContainerType;

    /**
     * @brief Creates the navigation series.
     * @param _key private construction key.
     */
    FWMEDDATA_API NavigationSeries(::fwData::Object::Key _key);

    /// Destroys the navigation series.
    FWMEDDATA_API virtual ~NavigationSeries();

    /**
     * @brief Defines shallow copy.
     * @param _source the source object to copy into this one.
     */
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr& _source ) override;

    /**
     * @brief Defines deep copy.
     * @param _source the source object to copy into this one.
     * @param _cache contains all copied objects to avoid duplication.
     */
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& _cache) override;

    /**
     * @brief Adds a position associate with the specified timestamp.
     * @param _time the timestamp.
     * @param _pos the position.
     */
    FWMEDDATA_API void addPosition(TimestampType _time, const CoordinateType& _pos);

    /**
     * @brief Adds a move direction associate with the specified timestamp.
     * @param _time the timestamp.
     * @param _dir the direction.
     */
    FWMEDDATA_API void addMove(TimestampType _time, const CoordinateType& _dir);

    /**
     * @brief Adds a lookAt point associate with the specified timestamp.
     * @param _time the timestamp.
     * @param _point the lookAt point.
     */
    FWMEDDATA_API void addLookAt(TimestampType _time, const CoordinateType& _point);

    /**
     * @brief Removes the position (if one exists) associate with the specified timestamp.
     * @param _time the timestamp.
     */
    FWMEDDATA_API void erasePosition(TimestampType _time);

    /**
     * @brief Removes the move direction (if one exists) associate with the specified timestamp.
     * @param _time the timestamp.
     */
    FWMEDDATA_API void eraseMove(TimestampType _time);

    /**
     * @brief Removes the lookAt point (if one exists) associate with the specified timestamp.
     * @param _time the timestamp.
     */
    FWMEDDATA_API void eraseLookAt(TimestampType _time);

    /// Clears all positions.
    FWMEDDATA_API void clearPosition();

    /// Clears all move directions.
    FWMEDDATA_API void clearMove();

    /// Clears all lookAt points.
    FWMEDDATA_API void clearLookAt();

    /// Gets the positions map container.
    FWMEDDATA_API const ContainerType& getPositionMap() const;

    /// Gets the move map container.
    FWMEDDATA_API const ContainerType& getMoveMap() const;

    /// Gets the lookAt map container.
    FWMEDDATA_API const ContainerType& getLookAtMap() const;

protected:

    /// Stores position.
    ContainerType m_position;

    /// Stores move.
    ContainerType m_move;

    /// Stores lookAt.
    ContainerType m_lookAt;
};

} // Namespace fwMedData.
