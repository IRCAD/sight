/************************************************************************
 *
 * Copyright (C) 2017-2018 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
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

#include "itkRegistrationOp/config.hpp"
#include "itkRegistrationOp/Metric.hpp"

#include <fwData/Image.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <itkEuler3DTransform.h>
#include <itkImageRegistrationMethodv4.h>
#include <itkIntTypes.h>
#include <itkRegularStepGradientDescentOptimizerv4.h>

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

    typedef typename ::itk::Image< float, 3 > RegisteredImageType;

    typedef typename ::itk::RegularStepGradientDescentOptimizerv4<RealType> OptimizerType;

    /// Shrink factors per level and smoothing sigmas per level
    typedef std::vector< std::pair< ::itk::SizeValueType, RealType > > MultiResolutionParametersType;

    typedef std::function< void () > IterationCallbackType;

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
                                             RealType _samplingPercentage    = 1.0,
                                             double _minStep                 = 0.0001,
                                             unsigned long _maxIterations    = 200,
                                             IterationCallbackType _callback = nullptr);

    ITKREGISTRATIONOP_API void stopRegistration();

    /// Current metric evaluated by the optimizer.
    ITKREGISTRATIONOP_API RealType getCurrentMetricValue() const;

    /// Current set of parameters used to evaluate the metric in the optimizer.
    ITKREGISTRATIONOP_API const OptimizerType::ParametersType& getCurrentParameters() const;

    /// Gradient descent relaxation factor.
    ITKREGISTRATIONOP_API RealType getRelaxationFactor() const;

    /// Gradient descent learning rate.
    ITKREGISTRATIONOP_API RealType getLearningRate() const;

    /// Gradient magnitude tolerance.
    ITKREGISTRATIONOP_API RealType getGradientMagnitudeTolerance() const;

    /// Current optimizer iteration.
    ITKREGISTRATIONOP_API ::itk::SizeValueType getCurrentIteration() const;

    /// Current multi-resolution level.
    ITKREGISTRATIONOP_API ::itk::SizeValueType getCurrentLevel() const;

    /// Current registration result.
    ITKREGISTRATIONOP_API void getCurrentMatrix(const ::fwData::TransformationMatrix3D::sptr& _trf) const;

private:
    typedef typename ::itk::Euler3DTransform< RealType > TransformType;

    typedef typename ::itk::ImageRegistrationMethodv4< RegisteredImageType, RegisteredImageType, TransformType >
        RegistrationMethodType;

    OptimizerType::Pointer m_optimizer { nullptr };

    RegistrationMethodType::Pointer m_registrator { nullptr };

    bool m_invert { false };

    void convertToF4sMatrix(const TransformType* _itkMat, const ::fwData::TransformationMatrix3D::sptr& _f4sMat) const;

    static double computeVolume(const fwData::Image::csptr& _img);
};

} // itkRegistrationOp
