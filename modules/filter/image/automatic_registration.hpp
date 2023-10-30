/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include "modules/filter/image/config.hpp"

#include <data/image.hpp>
#include <data/matrix4.hpp>

#include <filter/image/automatic_registration.hpp>

#include <service/filter.hpp>

#include <ui/__/has_parameters.hpp>

namespace sight::module::filter::image
{

/**
 * @brief Service computing a rigid transform matching an image to another.
 *
 * @section Signals Signals
 * - \b computed() : Signal sent after registration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::filter::image::automatic_registration">
       <in key="target" uid="..." />
       <in key="reference" uid="..." />
       <inout key="transform" uid="..." />
       <minStep>0.0001</minStep>
       <maxIterations>500</maxIterations>
       <metric>MeanSquare</metric>
       <levels>4:10;2:6;1:0</levels>
       <log>false</log>
   </service>
   @endcode
 * @subsection Input Input
 * - \b target [sight::data::image]: Fixed image.
 * - \b reference [sight::data::image]: image that will be deformed to match the target.
 *
 * @subsection In-Out In-Out
 * - \b transform [sight::data::matrix4]: The initial transform used during registration, will be updated
 * with the new value after registration.
 *
 * @subsection Configuration Configuration
 * - \b minStep : smallest step that can be taken by the optimizer. A smaller step gives a more precise result but
 * will converge slower.
 * - \b maxIterations : the maximum number of steps allowed to the optimizer. The optimizer will stop beyond this point
 * even if it didn't find a suitable result.
 * - \b metric : the metric used to compare the two images. Possible values are :
 * MeanSquares : fastest metric, only works when matching images with the same intensity values.
 * - \b levels (optional, default=1:0): multi-resolution levels separated by semicolons
 * with their parameters separated by colons.
 * - \b log (optional, default=false): enable/disable logging, outputs stats in a CSV file at each registration step.
 *
 * NormalizedCorrelation : works when the intensity values are within a linear transform from each other.
 * MutualInformation : most generic metric, based on entropy. Can match images with different modalities.
 */
class MODULE_FILTER_IMAGE_CLASS_API automatic_registration : public service::filter,
                                                             public ui::has_parameters
{
public:

    SIGHT_DECLARE_SERVICE(automatic_registration, service::filter);

    /// Constructor, does nothing.
    MODULE_FILTER_IMAGE_API automatic_registration();

    /// Destructor, does nothing.
    MODULE_FILTER_IMAGE_API ~automatic_registration() override;

protected:

    /// Configure registration parameters.
    MODULE_FILTER_IMAGE_API void configuring() override;

    /// Does nothing.
    MODULE_FILTER_IMAGE_API void starting() override;

    /// Do the registration.
    MODULE_FILTER_IMAGE_API void updating() override;

    /// Does nothing.
    MODULE_FILTER_IMAGE_API void stopping() override;

    /**
     * @brief Auto connections
     *
     * - Update service when one of the two images is modified.
     * - Update service when the transform matrix is modified.
     */
    MODULE_FILTER_IMAGE_API connections_t auto_connections() const override;

private:

    /// Set the metric to be used. Key must be 'metric', values are the same as for the configuration.
    void set_enum_parameter(std::string _val, std::string _key) override;

    /// Set the minimum and maximum step sizes. keys are "minStep" and "maxStep".
    void set_double_parameter(double _val, std::string _key) override;

    /// Set the maximum number of iterations, key must be "maxIterations".
    void set_int_parameter(int _val, std::string _key) override;

    /// Sets the metric, possible values are : MeanSquares, NormalizedCorrelation, MutualInformation.
    void set_metric(const std::string& _metric_name);

    /// Extract the level at the end of the parameter name.
    /// Create the level if it doesn't exist
    std::uint64_t extract_level_from_parameter_name(const std::string& _name);

    /// Smallest step that can be taken by the optimizer.
    double m_min_step {};

    /// Maximum number of iterations allowed.
    std::uint64_t m_max_iterations {};

    /// Flag enabling the registration log.
    bool m_log = {false};

    /// metric used by the optimizer.
    sight::filter::image::metric_t m_metric {sight::filter::image::metric_t::mean_squares};

    /// Shrink factors per level and smoothing sigmas per level.
    sight::filter::image::automatic_registration::multi_resolution_parameters_t m_multi_resolution_parameters;

    /// Percentage of samples used for registration.
    sight::filter::image::automatic_registration::real_t m_sampling_percentage {};

    static constexpr std::string_view TRANSFORM_INOUT = "transform";
    static constexpr std::string_view TARGET_IN       = "target";
    static constexpr std::string_view REFERENCE_IN    = "reference";

    sight::data::ptr<sight::data::matrix4, sight::data::access::inout> m_transform {this, TRANSFORM_INOUT};
    sight::data::ptr<sight::data::image, sight::data::access::in> m_target {this, TARGET_IN};
    sight::data::ptr<sight::data::image, sight::data::access::in> m_reference {this, REFERENCE_IN};
};

} // namespace sight::module::filter::image
