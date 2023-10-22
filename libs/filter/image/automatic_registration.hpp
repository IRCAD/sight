/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "filter/image/config.hpp"
#include "filter/image/metric.hpp"

#include <data/image.hpp>
#include <data/matrix4.hpp>

#include <itkIntTypes.h>
#include <itkRegularStepGradientDescentOptimizerv4.h>

#include <functional>

namespace sight::filter::image
{

/**
 * @brief Static class for automatic image registration. Uses the newer ITKv4 registration framework.
 */
class FILTER_IMAGE_CLASS_API automatic_registration
{
public:

    /// Numeric type used for internal computations.
    using real_t = double;

    using registered_image_t = itk::Image<float, 3>;

    using optimizer_t = itk::RegularStepGradientDescentOptimizerv4<real_t>;

    /// Shrink factors per level and smoothing sigmas per level
    using multi_resolution_parameters_t = std::vector<std::pair<itk::SizeValueType, real_t> >;

    using iteration_callback_t = std::function<void ()>;

    FILTER_IMAGE_API automatic_registration() noexcept;
    FILTER_IMAGE_API virtual ~automatic_registration() noexcept;

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
    FILTER_IMAGE_API void registerImage(
        const data::image::csptr& _target,
        const data::image::csptr& _reference,
        const data::matrix4::sptr& _trf,
        metric_t _metric,
        const multi_resolution_parameters_t& _multi_resolution_parameters,
        real_t _sampling_percentage    = 1.0,
        double _min_step               = 0.0001,
        std::uint64_t _max_iterations  = 200,
        iteration_callback_t _callback = nullptr
    );

    FILTER_IMAGE_API void stopRegistration();

    /// Current metric evaluated by the optimizer.
    [[nodiscard]] FILTER_IMAGE_API real_t getCurrentMetricValue() const;

    /// Current set of parameters used to evaluate the metric in the optimizer.
    [[nodiscard]] FILTER_IMAGE_API const optimizer_t::ParametersType& getCurrentParameters() const;

    /// Gradient descent relaxation factor.
    [[nodiscard]] FILTER_IMAGE_API real_t getRelaxationFactor() const;

    /// Gradient descent learning rate.
    [[nodiscard]] FILTER_IMAGE_API real_t getLearningRate() const;

    /// Gradient magnitude tolerance.
    [[nodiscard]] FILTER_IMAGE_API real_t getGradientMagnitudeTolerance() const;

    /// Current optimizer iteration.
    [[nodiscard]] FILTER_IMAGE_API itk::SizeValueType getCurrentIteration() const;

    /// Current multi-resolution level.
    [[nodiscard]] FILTER_IMAGE_API itk::SizeValueType getCurrentLevel() const;

    /// Current registration result.
    FILTER_IMAGE_API void getCurrentMatrix(const data::matrix4::sptr& _trf) const;

private:

    /// PImpl
    class automatic_registrationImpl;
    std::unique_ptr<automatic_registrationImpl> m_pimpl;
};

} // namespace sight::filter::image
