/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

fwCampAutoDeclareDataMacro((fwMedData)(NavigationSeries), FWMEDDATA_API);

namespace fwMedData
{

class Series;

/**
 * @brief Holds navigation information
 */
class FWMEDDATA_CLASS_API NavigationSeries : public ::fwMedData::Series
{

public:
    fwCoreClassMacro(NavigationSeries, ::fwMedData::Series, ::fwData::factory::New< NavigationSeries >);

    fwCampMakeFriendDataMacro((fwMedData)(NavigationSeries));

    typedef std::array<double, 3> CoordinateType;
    typedef double TimestampType;
    typedef std::map<TimestampType, CoordinateType> ContainerType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWMEDDATA_API NavigationSeries(::fwData::Object::Key key);

    /// Destructor
    FWMEDDATA_API virtual ~NavigationSeries();

    /// Defines shallow copy
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr& _source ) override;

    /// Defines deep copy
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache ) override;

    /// Adds a position associate with the specified timestamp
    FWMEDDATA_API void addPosition(TimestampType time, const CoordinateType& pos);

    /// Adds a move direction associate with the specified timestamp
    FWMEDDATA_API void addMove(TimestampType time, const CoordinateType& dir);

    /// Adds a lookAt point associate with the specified timestamp
    FWMEDDATA_API void addLookAt(TimestampType time, const CoordinateType& point);

    /// Removes the position (if one exists) associate with the specified timestamp
    FWMEDDATA_API void erasePosition(TimestampType time);

    /// Removes the move direction (if one exists) associate with the specified timestamp
    FWMEDDATA_API void eraseMove(TimestampType time);

    /// Removes the lookAt point (if one exists) associate with the specified timestamp
    FWMEDDATA_API void eraseLookAt(TimestampType time);

    /// Clear all positions
    FWMEDDATA_API void clearPosition();

    /// Clear all move directions
    FWMEDDATA_API void clearMove();

    /// Clear all lookAt points
    FWMEDDATA_API void clearLookAt();

    /// Returns positions map container
    FWMEDDATA_API const ContainerType& getPositionMap() const;

    /// Returns move map container
    FWMEDDATA_API const ContainerType& getMoveMap() const;

    /// Returns lokAt map container
    FWMEDDATA_API const ContainerType& getLookAtMap() const;

protected:

    ContainerType m_position;
    ContainerType m_move;
    ContainerType m_lookAt;
};

}   //end namespace fwMedData
