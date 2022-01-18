/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "data/Composite.hpp"
#include "data/config.hpp"
#include "data/factory/new.hpp"
#include "data/Series.hpp"

SIGHT_DECLARE_DATA_REFLECTION((sight) (data) (ActivitySeries))

namespace sight::data
{

/**
 * @brief Holds activity information.
 */
class DATA_CLASS_API ActivitySeries : public Series
{
public:

    SIGHT_DECLARE_CLASS(ActivitySeries, Series, factory::New<ActivitySeries>);

    SIGHT_MAKE_FRIEND_REFLECTION((sight) (data) (ActivitySeries))

    typedef std::string ConfigIdType;

    /**
     * @brief Creates the activity series.
     * @param _key private construction key.
     */
    DATA_API ActivitySeries(Object::Key _key);

    /// Destroys the activity series.
    DATA_API virtual ~ActivitySeries();

    /**
     * @brief Defines shallow copy.
     * @param _source the source object to copy into this one.
     */
    DATA_API void shallowCopy(const Object::csptr& _source) override;

    /// Gets the activity container.
    Composite::sptr getData() const;

    /// Sets the activity container.
    void setData(const Composite::sptr& _val);

    /// Gets the activity configuration ID.
    const ConfigIdType& getActivityConfigId() const;

    /// Sets the activity configuration ID.
    void setActivityConfigId(const ConfigIdType& _val);

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const ActivitySeries& other) const noexcept;
    DATA_API bool operator!=(const ActivitySeries& other) const noexcept;
    /// @}

protected:

    /**
     * @brief Defines deep copy.
     * @param _source the source object to copy into this one.
     * @param _cache contains all copied objects to avoid duplication.
     */
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& _cache) override;

    /// Defines the activity configuration ID.
    ConfigIdType m_activityConfigId;

    /// Stores the composite used to stores activities.
    Composite::sptr m_data;
};

//-----------------------------------------------------------------------------

inline Composite::sptr ActivitySeries::getData() const
{
    return m_data;
}

//-----------------------------------------------------------------------------

inline void ActivitySeries::setData(const Composite::sptr& _val)
{
    m_data = _val;
}

//-----------------------------------------------------------------------------

inline const ActivitySeries::ConfigIdType& ActivitySeries::getActivityConfigId() const
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
