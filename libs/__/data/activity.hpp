/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "data/composite.hpp"
#include "data/config.hpp"
#include "data/factory/new.hpp"

namespace sight::data
{

/**
 * @brief Holds activity information.
 */
class DATA_CLASS_API activity final : public data::composite
{
public:

    SIGHT_DECLARE_CLASS(activity, data::composite);

    typedef std::string config_id_t;

    /// Constructor / Destructor
    /// @{
    DATA_API activity() = default;
    DATA_API ~activity() noexcept override = default;
    /// @}

    /// Gets the activity configuration ID.
    const config_id_t& getActivityConfigId() const;

    /// Sets the activity configuration ID.
    void setActivityConfigId(const config_id_t& _val);

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const activity& _other) const noexcept;
    DATA_API bool operator!=(const activity& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

protected:

    /// Defines the activity configuration ID.
    config_id_t m_activityConfigId;
};

//-----------------------------------------------------------------------------

inline const activity::config_id_t& activity::getActivityConfigId() const
{
    return m_activityConfigId;
}

//-----------------------------------------------------------------------------

inline void activity::setActivityConfigId(const activity::config_id_t& _val)
{
    m_activityConfigId = _val;
}

//-----------------------------------------------------------------------------

} // Namespace fwMedData.
