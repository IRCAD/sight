/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
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

#include <sight/filter/image/config.hpp>

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
class SIGHT_FILTER_IMAGE_CLASS_API automatic_registration
{
public:

    /// Numeric type used for internal computations.
    using real_t = double;

    using registered_image_t = itk::Image<float, 3>;

    using optimizer_t = itk::RegularStepGradientDescentOptimizerv4<real_t>;

    /// Shrink factors per level and smoothing sigmas per level
    using multi_resolution_parameters_t = std::vector<std::pair<itk::SizeValueType, real_t> >;

    using iteration_callback_t = std::function<void ()>;

    SIGHT_FILTER_IMAGE_API automatic_registration() noexcept;
    SIGHT_FILTER_IMAGE_API virtual ~automatic_registration() noexcept;

    /**
     * @brief find a rigid transform matching the reference image with the target image.
     * @param[in] _target target, i.e. the static image.
     * @param[in] _reference reference, i.e. the image that will be transformed into the target.
     * @param[out] _trf the resulting rigid transform.
     * @param[in] _metric the metric to be used for registration.
     * @param[in] _multi_resolution_parameters Shrink factors per level and smoothing sigmas per level
     * @param[in] _sampling_percentage the percentage of sample to use for registration
     * @param[in] _min_step minimum step for used by optimizer for each iteration.
     * @param[in] _max_iterations the maximum number of iterations
     */
    SIGHT_FILTER_IMAGE_API void register_image(
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

    SIGHT_FILTER_IMAGE_API void stop_registration();

    /// Current metric evaluated by the optimizer.
    [[nodiscard]] SIGHT_FILTER_IMAGE_API real_t get_current_metric_value() const;

    /// Current set of parameters used to evaluate the metric in the optimizer.
    [[nodiscard]] SIGHT_FILTER_IMAGE_API const optimizer_t::ParametersType& get_current_parameters() const;

    /// Gradient descent relaxation factor.
    [[nodiscard]] SIGHT_FILTER_IMAGE_API real_t get_relaxation_factor() const;

    /// Gradient descent learning rate.
    [[nodiscard]] SIGHT_FILTER_IMAGE_API real_t get_learning_rate() const;

    /// Gradient magnitude tolerance.
    [[nodiscard]] SIGHT_FILTER_IMAGE_API real_t get_gradient_magnitude_tolerance() const;

    /// Current optimizer iteration.
    [[nodiscard]] SIGHT_FILTER_IMAGE_API itk::SizeValueType get_current_iteration() const;

    /// Current multi-resolution level.
    [[nodiscard]] SIGHT_FILTER_IMAGE_API itk::SizeValueType get_current_level() const;

    /// Current registration result.
    SIGHT_FILTER_IMAGE_API void get_current_matrix(const data::matrix4::sptr& _trf) const;

private:

    /// PImpl
    class automatic_registrationImpl;
    std::unique_ptr<automatic_registrationImpl> m_pimpl;
};

} // namespace sight::filter::image
