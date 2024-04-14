/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include <data/matrix4.hpp>

#include <service/filter.hpp>

#include <ui/__/parameter.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace sight::module::geometry
{

/**
 * @brief Computed a weighted mean using the input matrix over time
 * the output will be = current_input * weight + current_weighted * (1 - weight).
 *
 * @section Slots Slots
 * - \b set_parameter(ui::parameter_t, std::string): set the weight value or enable/disable from the UI.
 *
 * @section XML XML configuration
 * @code{.xml}
   <service uid="..." type="sight::module::geometry::weighted_mean_matrix" >
        <in key="raw" uid="..." />
        <inout key="damped" uid="..." />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b raw: the current input matrix (auto-connected)
 *
 * @subsection In-Out In-Out
 * - \b damped: the current weighted output
 *
 * @subsection Config Config
 * - \b weight: between 0.1 and 1.0 the weight used to ponderate current matrix (1.0 means no averaging,
 * 0.1 means that we use only 10% of the current matrix).
 * - \b enabled: enable the filter, output = input, equivalent to weight = 1.0.
 *
 */
class weighted_mean_matrix final : public sight::service::filter
{
public:

    SIGHT_DECLARE_SERVICE(weighted_mean_matrix, sight::service::filter)

    struct slots
    {
        using slots_t = sight::core::com::slots::key_t;
        static inline const slots_t SET_PARAMETER = "set_parameter";
    };

    struct config
    {
        static inline const std::string WEIGHT  = "weight";
        static inline const std::string ENABLED = "enabled";
    };

    /// Defines slots.
    weighted_mean_matrix();

    /// Does nothing.
    ~weighted_mean_matrix() final = default;

protected:

    /// Configures the service.
    void configuring(const config_t& _config) final;

    /// Does nothing.
    void starting() final;

    /// Computes the weighted mean of matrix
    void updating() final;

    /// Does nothing.
    void stopping() final;

    /// Connects the input matrix modified to the update slot.
    sight::service::connections_t auto_connections() const final;

    /// Handles parameters (weight, enabled) updates.
    void set_parameter(sight::ui::parameter_t _value, std::string _key);

private:

    static constexpr std::string_view MATRIX_INPUT = "raw";
    static constexpr std::string_view MATRIX_INOUT = "damped";

    sight::data::ptr<sight::data::matrix4, sight::data::access::in> m_matrix_in {this, MATRIX_INPUT, true};
    sight::data::ptr<sight::data::matrix4, sight::data::access::inout> m_matrix_out {this, MATRIX_INOUT};

    /// Weight, default 0.5
    double m_weight {0.5};

    /// Enabled or passthrough mode
    bool m_enabled {true};

    /// The first matrix can not be averaged, but need to be saved to use it a next update.
    /// The boolean store this state.
    bool m_initialized {false};

    /// Current weighted orientation.
    glm::dquat m_current_weighted_orientation {};

    /// Current weighted translation.
    glm::dvec3 m_current_weighted_translation {};
};

} // namespace sight::module::geometry
