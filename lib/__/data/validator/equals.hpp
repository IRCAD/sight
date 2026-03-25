/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <data/validator/base.hpp>

namespace sight::data::validator
{

/**
 * @brief Defines a validator which checks if a string_serializable data equals a specific value.
 * The value to compare with is set by the configuration with the "value" parameter:
 *  @code{.xml}
 *   <value>my_value</value>
 *  @endcode
 */
class equals final : public sight::data::validator::base
{
public:

    SIGHT_DECLARE_CLASS(
        equals,
        sight::data::validator::base,
        sight::data::validator::factory::make<equals>
    );
    ~equals() final = default;

    /** Configures the validator with the given configuration. */
    void configure(const config_t& _config) final;

    /**
     * @brief Checks that the object equals the configured value
     * @note Given object should be a string_serializable data
     * @see data::validator::base::validate
     */
    sight::data::validator::return_t validate(const CSPTR(data::object)& _object) const final;

    /** Connects with data::MODIFIED signal **/
    SIGHT_DATA_API auto_connect_signals_t auto_connect_signals() const final;

private:

    /// Value to compare with the validated object, this is set by the configuration.
    std::string m_value;
};

} // namespace sight::data::validator
