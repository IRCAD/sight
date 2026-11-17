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

#include <string>
#include <utility>
#include <vector>

namespace sight::data::validator
{

/**
 * @brief Defines a validator which checks if a model_series contains specific organs.
 * The organs to check for are set by the configuration with "organ" elements:
 *  @code{.xml}
 *   <organ name="Masses" type="Liver"  />
 *   <organ name="Vessels" type="Liver"  />
 *  @endcode
 */
class has_model final : public sight::data::validator::base
{
public:

    SIGHT_DECLARE_CLASS(
        has_model,
        sight::data::validator::base,
        sight::data::validator::factory::make<has_model>
    );
    ~has_model() final = default;

    /** Configures the validator with the given configuration. */
    void configure(const config_t& _config) final;

    /**
     * @brief Checks that the model_series contains the configured organs
     * @note Given object should be a model_series data
     * @see data::validator::base::validate
     */
    sight::data::validator::return_t validate(const sight::csptr<data::object>& _object) const final;

    /** Connects with data::MODIFIED signal **/
    SIGHT_DATA_API auto_connect_signals_t auto_connect_signals() const final;

private:

    /// Organ constraint (name, type) pairs
    std::vector<std::pair<std::string, std::string> > m_organs;
};

} // namespace sight::data::validator
