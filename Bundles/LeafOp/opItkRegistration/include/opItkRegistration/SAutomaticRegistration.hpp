/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPITKREGISTRATION_SAUTOMATICREGISTRATION_HPP__
#define __OPITKREGISTRATION_SAUTOMATICREGISTRATION_HPP__

#include "opItkRegistration/config.hpp"

#include <fwServices/IOperator.hpp>

#include <itkRegistrationOp/AutomaticRegistration.hpp>
#include <itkRegistrationOp/Metric.hpp>

namespace opItkRegistration
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
       <log>false</log>
   </service>
   @endcode
 * @subsection Input Input
 * - \b target [::fwData::Image]: Fixed image.
 * - \b reference [::fwData::Image]: Image that will be deformed to match the target.
 *
 * @subsection In-Out In-Out
 * - \b transform [::fwData::TransformationMatrix3D]: The initial tranform used during registration, will be updated
 * with the new value after registration.
 *
 * @subsection Configuration Configuration
 * - \b minStep : smallest step that can be taken by the optimizer. A smaller step gives a more precise result but
 * will converge slower.
 * - \b maxIterations : the maximum number of steps allowed to the optimizer. The optimizer will stop beyond this point
 * even if it didn't find a suitable result.
 * - \b metric : the metric used to compare the two images. Possible values are :
 * MeanSquares : fastest metric, only works when matching images with the same intensity values.
 * - \b log (optional, defaul=false): enable/disable logging, outputs stats in a CSV file at each registration step.
 *
 * NormalizedCorrelation : works when the intensity values are within a linear transform from each other.
 * MutualInformation : most generic metric, based on entropy. Can match images with different modalities.
 */
class OPITKREGISTRATION_CLASS_API SAutomaticRegistration : public ::fwServices::IOperator
{
public:

    fwCoreServiceClassDefinitionsMacro( (SAutomaticRegistration)(::fwServices::IOperator) )

    /// Constructor, does nothing.
    OPITKREGISTRATION_API SAutomaticRegistration();

    /// Destructor, does nothing.
    OPITKREGISTRATION_API ~SAutomaticRegistration();

protected:

    /// Configure registration parameters.
    OPITKREGISTRATION_API virtual void configuring() override;

    /// Does nothing.
    OPITKREGISTRATION_API virtual void starting() override;

    /// Do the registration.
    OPITKREGISTRATION_API virtual void updating() override;

    /// Does nothing.
    OPITKREGISTRATION_API virtual void stopping() override;

    /**
     * @brief Auto connections
     *
     * - Update service when one of the two images is modified.
     * - Update service when the transform matrix is modified.
     */
    OPITKREGISTRATION_API virtual KeyConnectionsMap getAutoConnections() const override;

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
    ::itkRegistrationOp::MetricType m_metric;

    /// Shrink factors per level and smoothing sigmas per level.
    ::itkRegistrationOp::AutomaticRegistration::MultiResolutionParametersType m_multiResolutionParameters;

    /// Percentage of samples used for registration.
    ::itkRegistrationOp::AutomaticRegistration::RealType m_samplingPercentage;
};

} // namespace opItkRegistration

#endif // __OPITKREGISTRATION_SAUTOMATICREGISTRATION_HPP__
