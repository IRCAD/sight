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
       <maxStep>0.2</maxStep>
       <maxIterations>500</maxIterations>
       <metric>MeanSquare</metric>
       <legacyMode>off</legacyMode>
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
 * - \b maxStep : biggest step that can be taken by the optimizer. This value should be set according to
 * the distance approximately separating the two images. It affects the time needed to converge.
 * - \b maxIterations : the maximum number of steps allowed to the optimizer. The optimizer will stop beyond this point
 * even if it didn't find a suitable result.
 * - \b metric : the metric used to compare the two images. Possible values are :
 * MeanSquares : fastest metric, only works when matching images with the same intensity values.
 * NormalizedCorrelation : works when the intensity values are within a linear transform from each other.
 * MutualInformation : most generic metric, based on entropy. Can match images with different modalities.
 * - \b legacyMode (optional) (on|off) : use the legacy ITK framework (off by default).
 */
class OPITKREGISTRATION_CLASS_API SAutomaticRegistration : public ::fwServices::IOperator
{
public:

    fwCoreServiceClassDefinitionsMacro( (SAutomaticRegistration)(::fwServices::IOperator) );

    /// Constructor, does nothing.
    OPITKREGISTRATION_API SAutomaticRegistration();

    /// Destructor, does nothing.
    OPITKREGISTRATION_API ~SAutomaticRegistration();

protected:

    /// Configure registration parameters.
    OPITKREGISTRATION_API virtual void configuring();

    /// Does nothing.
    OPITKREGISTRATION_API virtual void starting();

    /// Do the registration.
    OPITKREGISTRATION_API virtual void updating();

    /// Does nothing.
    OPITKREGISTRATION_API virtual void stopping();

    /**
     * @brief Auto connections
     *
     * - Update service when one of the two images is modified.
     * - Update service when the transform matrix is modified.
     */
    OPITKREGISTRATION_API virtual KeyConnectionsMap getAutoConnections() const;

private:

    /// Set the metric to be used. Key must be 'metric', values are the same as for the configuration.
    virtual void setEnumParameter(std::string val, std::string key);

    /// Set the minimum and maximum step sizes. keys are "minStep" and "maxStep".
    virtual void setDoubleParameter(double val, std::string key);

    /// Set the maximum number of iterations, key must be "maxIterations".
    virtual void setIntParameter(int val, std::string key);

    /// Sets the metric, possible values are : MeanSquares, NormalizedCorrelation, MutualInformation.
    void setMetric(const std::string& metricName);

    /// Smallest step that can be taken by the optimizer.
    double m_minStep;

    /// Biggest step that can be taken by the optimizer.
    double m_maxStep;

    /// Maximum number of iterations allowed.
    unsigned long m_maxIterations;

    /// Metric used by the optimizer.
    ::itkRegistrationOp::MetricType m_metric;

    /// If true, use legacy registration instead of v4 registration.
    bool m_legacyMode;
};

} // namespace opItkRegistration

#endif // __OPITKREGISTRATION_SAUTOMATICREGISTRATION_HPP__
