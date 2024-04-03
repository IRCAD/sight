/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include <itkPowellOptimizerv4.h>

#include <functional>

namespace sight::filter::image
{

/**
 * @brief Finds the local minimum of a function using Powell's method.
 *
 * This class is wrapper for ITK's Powell optimizer.
 */
class SIGHT_FILTER_IMAGE_CLASS_API powell_optimizer
{
public:

    using powell_optimizer_t = itk::PowellOptimizerv4<double>;

    using function_parameters_t = typename powell_optimizer_t::ParametersType;

    using optimized_function_t = std::function<double (const function_parameters_t&)>;

    /**
     * @brief Constructor. Initializes the ITK optimizer.
     * @param _f function whose minimum is to find.
     * @param _step_tolerance step value below which we consider the optimizer converged.
     * @param _value_tolerance terminates optimization once the function return value difference is within this range.
     * @param _step_length first and maximum step length.
     * @param _maximum_iterations maximum number of iterations allowed.
     */
    SIGHT_FILTER_IMAGE_API powell_optimizer(
        const optimized_function_t& _f,
        double _step_tolerance,
        double _value_tolerance,
        double _step_length,
        std::uint32_t _maximum_iterations
    );

    /**
     * @brief Runs the optimization function.
     * @param _init_parameters initial set of values to evaluate.
     * @return the set of parameters at the end of the optimization process.
     */
    SIGHT_FILTER_IMAGE_API function_parameters_t optimize(const function_parameters_t& _init_parameters);

private:

    /// ITK Powell optimization algorithm.
    typename powell_optimizer_t::Pointer m_powell_optimizer;

    /// Function whose local minimum is computed.
    const optimized_function_t& m_function;
};

} // namespace sight::filter::image.
