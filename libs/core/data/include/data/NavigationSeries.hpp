/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "data/config.hpp"
#include "data/Series.hpp"
#include <data/factory/new.hpp>

#include <array>
#include <map>

fwCampAutoDeclareDataMacro((sight)(data)(NavigationSeries))

namespace sight::data
{

class Series;

/**
 * @brief Holds a navigation information.
 */
class DATA_CLASS_API NavigationSeries : public data::Series
{

public:

    fwCoreClassMacro(NavigationSeries, data::Series, data::factory::New< NavigationSeries >)

    fwCampMakeFriendDataMacro((sight)(data)(NavigationSeries))

    typedef std::array<double, 3> CoordinateType;
    typedef double TimestampType;
    typedef std::map<TimestampType, CoordinateType> ContainerType;

    /**
     * @brief Creates the navigation series.
     * @param _key private construction key.
     */
    DATA_API NavigationSeries(data::Object::Key _key);

    /// Destroys the navigation series.
    DATA_API virtual ~NavigationSeries();

    /**
     * @brief Defines shallow copy.
     * @param _source the source object to copy into this one.
     */
    DATA_API void shallowCopy( const data::Object::csptr& _source ) override;

    /**
     * @brief Defines deep copy.
     * @param _source the source object to copy into this one.
     * @param _cache contains all copied objects to avoid duplication.
     */
    DATA_API void cachedDeepCopy( const data::Object::csptr& _source, DeepCopyCacheType& _cache) override;

    /**
     * @brief Adds a position associate with the specified timestamp.
     * @param _time the timestamp.
     * @param _pos the position.
     */
    DATA_API void addPosition(TimestampType _time, const CoordinateType& _pos);

    /**
     * @brief Adds a move direction associate with the specified timestamp.
     * @param _time the timestamp.
     * @param _dir the direction.
     */
    DATA_API void addMove(TimestampType _time, const CoordinateType& _dir);

    /**
     * @brief Adds a lookAt point associate with the specified timestamp.
     * @param _time the timestamp.
     * @param _point the lookAt point.
     */
    DATA_API void addLookAt(TimestampType _time, const CoordinateType& _point);

    /**
     * @brief Removes the position (if one exists) associate with the specified timestamp.
     * @param _time the timestamp.
     */
    DATA_API void erasePosition(TimestampType _time);

    /**
     * @brief Removes the move direction (if one exists) associate with the specified timestamp.
     * @param _time the timestamp.
     */
    DATA_API void eraseMove(TimestampType _time);

    /**
     * @brief Removes the lookAt point (if one exists) associate with the specified timestamp.
     * @param _time the timestamp.
     */
    DATA_API void eraseLookAt(TimestampType _time);

    /// Clears all positions.
    DATA_API void clearPosition();

    /// Clears all move directions.
    DATA_API void clearMove();

    /// Clears all lookAt points.
    DATA_API void clearLookAt();

    /// Gets the positions map container.
    DATA_API const ContainerType& getPositionMap() const;

    /// Gets the move map container.
    DATA_API const ContainerType& getMoveMap() const;

    /// Gets the lookAt map container.
    DATA_API const ContainerType& getLookAtMap() const;

protected:

    /// Stores position.
    ContainerType m_position;

    /// Stores move.
    ContainerType m_move;

    /// Stores lookAt.
    ContainerType m_lookAt;
};

} // Namespace fwMedData.
