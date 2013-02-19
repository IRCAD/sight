/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWACTIVITIES_ACTIVITYSERIES_HPP__
#define __FWACTIVITIES_ACTIVITYSERIES_HPP__

#include <fwData/factory/new.hpp>
#include <fwData/macros.hpp>

#include <fwMedData/Series.hpp>

#include "fwActivities/config.hpp"

fwCampAutoDeclareDataMacro((fwActivities)(ActivitySeries), FWACTIVITIES_API);

namespace fwData
{
    class Composite;
}

namespace fwActivities
{

class Series;

/**
 * @class ActivitySeries
 * Holds activity information
 *
 */
class FWACTIVITIES_CLASS_API ActivitySeries : public ::fwMedData::Series
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (ActivitySeries)(::fwMedData::Series),
                                            (()),
                                            ::fwData::factory::New< ActivitySeries >) ;

    fwCampMakeFriendDataMacro((fwActivities)(ActivitySeries));

    fwDataObjectMacro();

    typedef std::string ConfigIdType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWACTIVITIES_API ActivitySeries(::fwData::Object::Key key);

    /// Destructor
    FWACTIVITIES_API virtual ~ActivitySeries();

    /// Defines shallow copy
    FWACTIVITIES_API void shallowCopy( ActivitySeries::csptr source );

    /// Defines deep copy
    FWACTIVITIES_API void deepCopy( ActivitySeries::csptr source );

    /**
     * @name Getters / Setters
     * @{ */

    /**
     * @brief Data container
     * @{ */
    fwDataGetSetSptrMacro(Data, SPTR(::fwData::Composite));
    /**  @} */

    /**
     * @brief Activity configuration identifier
     * @{ */
    fwDataGetSetCRefMacro(ActivityConfigId, ConfigIdType);
    /**  @} */

    /**  @} */
protected:

    /// Activity configuration identifier
    ConfigIdType m_attrActivityConfigId;

    /// ... container
    SPTR(::fwData::Composite) m_attrData;

};

}   //end namespace fwActivities

#endif // __FWACTIVITIES_ACTIVITYSERIES_HPP__


