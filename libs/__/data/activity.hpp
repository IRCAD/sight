/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/data/config.hpp>

#include "data/factory/new.hpp"
#include "data/map.hpp"

namespace sight::data
{

/**
 * @brief Holds activity information.
 */
class SIGHT_DATA_CLASS_API activity final : public data::map
{
public:

    SIGHT_DECLARE_CLASS(activity, data::map);

    using config_id_t = std::string;

    /// Constructor / Destructor
    /// @{
    SIGHT_DATA_API activity() = default;
    SIGHT_DATA_API ~activity() noexcept override = default;
    /// @}

    /// Gets the activity configuration ID.
    const config_id_t& get_activity_config_id() const;

    /// Sets the activity configuration ID.
    void set_activity_config_id(const config_id_t& _val);

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const activity& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const activity& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    SIGHT_DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache  cache used to deduplicate pointers
    SIGHT_DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

protected:

    /// Defines the activity configuration ID.
    config_id_t m_activity_config_id;
};

//-----------------------------------------------------------------------------

inline const activity::config_id_t& activity::get_activity_config_id() const
{
    return m_activity_config_id;
}

//-----------------------------------------------------------------------------

inline void activity::set_activity_config_id(const activity::config_id_t& _val)
{
    m_activity_config_id = _val;
}

//-----------------------------------------------------------------------------

} // Namespace fwMedData.
