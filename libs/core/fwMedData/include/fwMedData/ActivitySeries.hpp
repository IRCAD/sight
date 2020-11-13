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

fwCampAutoDeclareDataMacro((fwMedData)(ActivitySeries))

namespace fwData
{
class Composite;
}

namespace fwMedData
{

class Series;

/**
 * @brief Holds activity information.
 */
class FWMEDDATA_CLASS_API ActivitySeries : public ::fwMedData::Series
{

public:
    fwCoreClassMacro(ActivitySeries, ::fwMedData::Series, ::fwData::factory::New< ActivitySeries >)

    fwCampMakeFriendDataMacro((fwMedData)(ActivitySeries))

    typedef std::string ConfigIdType;

    /**
     * @brief Creates the activity series.
     * @param _key private construction key.
     */
    FWMEDDATA_API ActivitySeries(::fwData::Object::Key _key);

    /// Destroys the activity series.
    FWMEDDATA_API virtual ~ActivitySeries();

    /**
     * @brief Defines shallow copy.
     * @param _source the source object to copy into this one.
     */
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr& _source) override;

    /**
     * @brief Defines deep copy.
     * @param _source the source object to copy into this one.
     * @param _cache contains all copied objects to avoid duplication.
     */
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& _cache) override;

    /// Gets the activity container.
    SPTR(::fwData::Composite) getData() const;

    /// Sets the activity container.
    void setData(const SPTR(::fwData::Composite)& _val);

    /// Gets the activity configuration ID.
    const ConfigIdType& getActivityConfigId () const;

    /// Sets the activity configuration ID.
    void setActivityConfigId (const ConfigIdType& _val);

protected:

    /// Defines the activity configuration ID.
    ConfigIdType m_activityConfigId;

    /// Stores the composite used to stores activities.
    SPTR(::fwData::Composite) m_data;

};

//-----------------------------------------------------------------------------

inline SPTR(::fwData::Composite) ActivitySeries::getData() const
{
    return m_data;
}

//-----------------------------------------------------------------------------

inline void ActivitySeries::setData(const SPTR(::fwData::Composite)& _val)
{
    m_data = _val;
}

//-----------------------------------------------------------------------------

inline const ActivitySeries::ConfigIdType& ActivitySeries::getActivityConfigId () const
{
    return m_activityConfigId;
}

//-----------------------------------------------------------------------------

inline void ActivitySeries::setActivityConfigId(const ActivitySeries::ConfigIdType& _val)
{
    m_activityConfigId = _val;
}

//-----------------------------------------------------------------------------

} // Namespace fwMedData.
