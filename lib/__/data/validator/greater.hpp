/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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
 * @brief Defines a validator which checks if a numeric data (real or integer) is greater than a specific threshold.
 * The threshold value is set by the configuration with the "value" parameter:
 *  @code{.xml}
 *   <value>10.5</value>
 *  @endcode
 *
 * @note Only accepts sight::data::real or sight::data::integer objects.
 */
class SIGHT_DATA_CLASS_API greater final : public sight::data::validator::base
{
public:

    SIGHT_DECLARE_CLASS(
        greater,
        sight::data::validator::base,
        sight::data::validator::factory::make<greater>
    );
    SIGHT_DATA_API ~greater() final = default;

    /** Configures the validator with the given configuration. */
    SIGHT_DATA_API void configure(const config_t& _config) final;

    /**
     * @brief Checks that the numeric object is greater than the configured threshold
     * @note Given object should be a sight::data::real or sight::data::integer
     * @see data::validator::base::validate
     */
    SIGHT_DATA_API sight::data::validator::return_t validate(const CSPTR(data::object)& _object) const final;

    /** Connects with data::MODIFIED signal **/
    SIGHT_DATA_API auto_connect_signals_t auto_connect_signals() const final;

private:

    /// Threshold value to compare with, this is set by the configuration.
    double m_threshold {0.0};
};

} // namespace sight::data::validator
