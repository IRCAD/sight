/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include <filter/image/AutomaticRegistration.hpp>
#include <filter/image/Metric.hpp>

#include <service/IOperator.hpp>

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
   <service type="::opITK::SAutomaticRegistration">
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
 * - \b target [sight::data::Image]: Fixed image.
 * - \b reference [sight::data::Image]: Image that will be deformed to match the target.
 *
 * @subsection In-Out In-Out
 * - \b transform [sight::data::Matrix4]: The initial transform used during registration, will be updated
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
class MODULE_FILTER_IMAGE_CLASS_API SAutomaticRegistration : public service::IOperator
{
public:

    SIGHT_DECLARE_SERVICE(SAutomaticRegistration, service::IOperator)

    /// Constructor, does nothing.
    MODULE_FILTER_IMAGE_API SAutomaticRegistration();

    /// Destructor, does nothing.
    MODULE_FILTER_IMAGE_API ~SAutomaticRegistration();

protected:

    /// Configure registration parameters.
    MODULE_FILTER_IMAGE_API virtual void configuring() override;

    /// Does nothing.
    MODULE_FILTER_IMAGE_API virtual void starting() override;

    /// Do the registration.
    MODULE_FILTER_IMAGE_API virtual void updating() override;

    /// Does nothing.
    MODULE_FILTER_IMAGE_API virtual void stopping() override;

    /**
     * @brief Auto connections
     *
     * - Update service when one of the two images is modified.
     * - Update service when the transform matrix is modified.
     */
    MODULE_FILTER_IMAGE_API virtual KeyConnectionsMap getAutoConnections() const override;

private:

    /// Set the metric to be used. Key must be 'metric', values are the same as for the configuration.
    virtual void setEnumParameter(std::string val, std::string key) override;

    /// Set the minimum and maximum step sizes. keys are "minStep" and "maxStep".
    virtual void setDoubleParameter(double val, std::string key) override;

    /// Set the maximum number of iterations, key must be "maxIterations".
    virtual void setIntParameter(int val, std::string key) override;

    /// Sets the metric, possible values are : MeanSquares, NormalizedCorrelation, MutualInformation.
    void setMetric(const std::string& metricName);

    /// Extract the level at the end of the parameter name.
    /// Create the level if it doesn't exist
    unsigned long extractLevelFromParameterName(const std::string& name );

    /// Smallest step that can be taken by the optimizer.
    double m_minStep;

    /// Maximum number of iterations allowed.
    unsigned long m_maxIterations;

    /// Flag enabling the registration log.
    bool m_log = { false };

    /// Metric used by the optimizer.
    sight::filter::image::MetricType m_metric;

    /// Shrink factors per level and smoothing sigmas per level.
    sight::filter::image::AutomaticRegistration::MultiResolutionParametersType m_multiResolutionParameters;

    /// Percentage of samples used for registration.
    sight::filter::image::AutomaticRegistration::RealType m_samplingPercentage;
};

} // namespace sight::module::filter::image
