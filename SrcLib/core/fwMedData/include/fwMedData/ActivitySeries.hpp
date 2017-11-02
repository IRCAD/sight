/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_ACTIVITYSERIES_HPP__
#define __FWMEDDATA_ACTIVITYSERIES_HPP__

#include "fwMedData/config.hpp"
#include "fwMedData/Series.hpp"

#include <fwData/factory/new.hpp>

fwCampAutoDeclareDataMacro((fwMedData)(ActivitySeries), FWMEDDATA_API);

namespace fwData
{
class Composite;
}

namespace fwMedData
{

class Series;

/**
 * @brief Holds activity information
 */
class FWMEDDATA_CLASS_API ActivitySeries : public ::fwMedData::Series
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (ActivitySeries)(::fwMedData::Series),
                                            (()),
                                            ::fwData::factory::New< ActivitySeries >);

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
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr& _source ) override;

    /// Defines deep copy
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache ) override;

    /**
     * @name Getters / Setters
     * @{ */

    /**
     * @brief Data container
     * @{ */
    SPTR(::fwData::Composite) getData() const;
    void setData(const SPTR(::fwData::Composite)& val);
    /**  @} */

    /**
     * @brief Activity configuration identifier
     * @{ */
    const ConfigIdType& getActivityConfigId () const;
    void setActivityConfigId (const ConfigIdType& val);
    /**  @} */

    /**  @} */

protected:

    /// Activity configuration identifier
    ConfigIdType m_activityConfigId;

    /// ... container
    SPTR(::fwData::Composite) m_data;

};

//-----------------------------------------------------------------------------

inline SPTR(::fwData::Composite) ActivitySeries::getData() const
{
    return m_data;
}

//-----------------------------------------------------------------------------

inline void ActivitySeries::setData(const SPTR(::fwData::Composite)& val)
{
    m_data = val;
}

//-----------------------------------------------------------------------------

inline const ActivitySeries::ConfigIdType& ActivitySeries::getActivityConfigId () const
{
    return m_activityConfigId;
}

//-----------------------------------------------------------------------------

inline void ActivitySeries::setActivityConfigId (const ActivitySeries::ConfigIdType& val)
{
    m_activityConfigId = val;
}

//-----------------------------------------------------------------------------

}   //end namespace fwMedData

#endif // __FWMEDDATA_ACTIVITYSERIES_HPP__

