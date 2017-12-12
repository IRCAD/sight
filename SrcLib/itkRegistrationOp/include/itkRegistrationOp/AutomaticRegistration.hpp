/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ITKREGISTRATIONOP_AUTOMATICREGISTRATION_HPP__
#define __ITKREGISTRATIONOP_AUTOMATICREGISTRATION_HPP__

#include "itkRegistrationOp/config.hpp"
#include "itkRegistrationOp/Metric.hpp"

#include <fwData/Image.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <itkIntTypes.h>
#include <itkRegularStepGradientDescentOptimizerv4.h>
#include <itkVersorRigid3DTransform.h>

#include <functional>

namespace itkRegistrationOp
{

/**
 * @brief Static class for automatic image registration. Uses the newer ITKv4 registration framework.
 */
class ITKREGISTRATIONOP_CLASS_API AutomaticRegistration
{
public:
    /// Numeric type used for internal computations.
    typedef double RealType;
    typedef typename ::itk::RegularStepGradientDescentOptimizerv4<RealType> OptimizerType;

    /// Shrink factors per level and smoothing sigmas per level
    typedef std::vector< std::pair< ::itk::SizeValueType, RealType > > MultiResolutionParametersType;

    typedef std::function< void (unsigned int, unsigned int) > IterationCallbackType;

    /**
     * @brief find a rigid transform matching the reference image with the target image.
     * @param[in] _target target, i.e. the static image.
     * @param[in] _reference reference, i.e. the image that will be transformed into the target.
     * @param[out] _trf the resulting rigid transform.
     * @param[in] _metric the metric to be used for registration.
     * @param[in] _multiResolutionParameters Shrink factors per level and smoothing sigmas per level
     * @param[in] _samplingPercentage the percentage of sample to use for registration
     * @param[in] _minStep minimum step for used by optimizer for each iteration.
     * @param[in] _maxIterations the maximum number of iterations
     */
    ITKREGISTRATIONOP_API void registerImage(const ::fwData::Image::csptr& _target,
                                             const ::fwData::Image::csptr& _reference,
                                             const ::fwData::TransformationMatrix3D::sptr& _trf,
                                             MetricType _metric,
                                             const MultiResolutionParametersType& _multiResolutionParameters,
                                             RealType _samplingPercentage = 1.0,
                                             double _minStep = 0.0001,
                                             unsigned long _maxIterations = 200,
                                             IterationCallbackType _callback = nullptr);

    ITKREGISTRATIONOP_API void stopRegistration();

    /// Current metric evaluated by the optimizer.
    ITKREGISTRATIONOP_API RealType getCurrentMetricValue() const;

    /// Current set of parameters used to evaluate the metric in the optimizer.
    ITKREGISTRATIONOP_API OptimizerType::ParametersType getCurrentParameters() const;

    /// Gradient descent relaxation factor.
    ITKREGISTRATIONOP_API RealType getRelaxationFactor() const;

    /// Gradient descent learning rate.
    ITKREGISTRATIONOP_API RealType getLearningRate() const;

    /// Gradient magnitude tolerance.
    ITKREGISTRATIONOP_API RealType getGradientMagnitudeTolerance() const;

private:

    typedef typename ::itk::VersorRigid3DTransform< RealType > TransformType;

    OptimizerType::Pointer m_optimizer;

    static void convertToF4sMatrix(const TransformType* _itkMat, const ::fwData::TransformationMatrix3D::sptr& _f4sMat);
};

} // itkRegistrationOp

#endif // __ITKREGISTRATIONOP_AUTOMATICREGISTRATION_HPP__
