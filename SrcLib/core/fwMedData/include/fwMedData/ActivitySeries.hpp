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
    fwCoreClassMacro(ActivitySeries, ::fwMedData::Series, ::fwData::factory::New< ActivitySeries >);

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
