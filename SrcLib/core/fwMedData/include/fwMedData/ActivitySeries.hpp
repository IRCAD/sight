/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_ACTIVITYSERIES_HPP__
#define __FWMEDDATA_ACTIVITYSERIES_HPP__

#include <fwData/factory/new.hpp>
#include <fwData/macros.hpp>

#include <fwMedData/Series.hpp>

#include "fwMedData/config.hpp"

fwCampAutoDeclareDataMacro((fwMedData)(ActivitySeries), FWMEDDATA_API);

namespace fwData
{
    class Composite;
}

namespace fwMedData
{

class Series;

/**
 * @class ActivitySeries
 * Holds activity information
 *
 */
class FWMEDDATA_CLASS_API ActivitySeries : public ::fwMedData::Series
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (ActivitySeries)(::fwMedData::Series),
                                            (()),
                                            ::fwData::factory::New< ActivitySeries >) ;

    fwCampMakeFriendDataMacro((fwMedData)(ActivitySeries));

    typedef std::string ConfigIdType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWMEDDATA_API ActivitySeries(::fwData::Object::Key key);

    /// Destructor
    FWMEDDATA_API virtual ~ActivitySeries();

    /// Defines shallow copy
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr &_source );

    /// Defines deep copy
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr &_source, DeepCopyCacheType &cache );

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

}   //end namespace fwMedData

#endif // __FWMEDDATA_ACTIVITYSERIES_HPP__


