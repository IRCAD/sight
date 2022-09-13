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

namespace sight::data
{

/**
 * @brief Holds activity information.
 */
class DATA_CLASS_API Activity final : public Object
{
public:

    SIGHT_DECLARE_CLASS(Activity, Object, factory::New<Activity>);

    typedef std::string ConfigIdType;

    /// Constructor / Destructor
    /// @{
    DATA_API Activity(Object::Key);
    DATA_API ~Activity() noexcept override = default;
    /// @}

    /// Gets the activity container.
    Composite::sptr getData();

    /// Gets the activity container.
    Composite::csptr getData() const;

    /// Sets the activity container.
    void setData(const Composite::sptr& _val);

    /// Gets the activity configuration ID.
    const ConfigIdType& getActivityConfigId() const;

    /// Sets the activity configuration ID.
    void setActivityConfigId(const ConfigIdType& _val);

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const Activity& other) const noexcept;
    DATA_API bool operator!=(const Activity& other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallowCopy(const Object::csptr& source) override;

    /// Defines deep copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deepCopy(
        const Object::csptr& source,
        const std::unique_ptr<DeepCopyCacheType>& cache = std::make_unique<DeepCopyCacheType>()
    ) override;

protected:

    /// Defines the activity configuration ID.
    ConfigIdType m_activityConfigId;

    /// Stores the composite used to stores activities.
    Composite::sptr m_data;
};

//-----------------------------------------------------------------------------

inline Composite::sptr Activity::getData()
{
    return m_data;
}

//-----------------------------------------------------------------------------

inline Composite::csptr Activity::getData() const
{
    return m_data;
}

//-----------------------------------------------------------------------------

inline void Activity::setData(const Composite::sptr& _val)
{
    m_data = _val;
}

//-----------------------------------------------------------------------------

inline const Activity::ConfigIdType& Activity::getActivityConfigId() const
{
    return m_activityConfigId;
}

//-----------------------------------------------------------------------------

inline void Activity::setActivityConfigId(const Activity::ConfigIdType& _val)
{
    m_activityConfigId = _val;
}

//-----------------------------------------------------------------------------

} // Namespace fwMedData.
