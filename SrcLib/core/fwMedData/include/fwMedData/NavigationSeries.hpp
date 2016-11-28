/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_NAVIGATIONSERIES_HPP__
#define __FWMEDDATA_NAVIGATIONSERIES_HPP__

#include "fwMedData/Series.hpp"
#include "fwMedData/config.hpp"

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
    fwCoreClassDefinitionsWithFactoryMacro( (NavigationSeries)(::fwMedData::Series),
                                            (()),
                                            ::fwData::factory::New< NavigationSeries >);

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
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr& _source );

    /// Defines deep copy
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache );

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

#endif // __FWMEDDATA_NAVIGATIONSERIES_HPP__


