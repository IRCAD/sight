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

#include "filter/image/config.hpp"

#include <data/matrix4.hpp>
#include <data/vector.hpp>

#include <glm/mat4x4.hpp>

namespace sight::filter::image
{

/**
 * @brief Computes an 'average' matrix from a matrix list.
 */
class FILTER_IMAGE_CLASS_API matrix_regressor
{
public:

    using point_t = glm::dvec4;

    /**
     * @brief Constructor
     * @pre The current implementation assumes that the matrices have a uniform scale.
     *
     * @param _matrix_list list of matrix to 'average'.
     * @param _points list of points used for solving.
     */
    FILTER_IMAGE_API matrix_regressor(const data::vector::csptr& _matrix_list, std::vector<point_t> _points);

    /**
     * @brief computes the matrix that best fits our matrix list.
     *
     * @param _init_value the initial value passed to the solver.
     * @param _step_length the initial step distance in a line direction.
     * @param _step_tolerance terminates the optimization when the parameter is within this distance of the local
     * minima.
     * @param _value_tolerance terminates the optimization when the transformed distance between the matrices and our
     * current solution and the local extreme are likely to be within this tolerance.
     * @param _max_iter the maximum number of iterations for the solver.
     * @return the 'average' matrix
     */
    FILTER_IMAGE_API data::matrix4::sptr minimize(
        const data::matrix4& _init_value,
        double _step_length     = 1.,
        double _step_tolerance  = 1e-2,
        double _value_tolerance = 1e-2,
        unsigned int _max_iter  = 10000
    );

private:

    std::vector<glm::dmat4> m_mat_list;

    std::vector<point_t> m_point_list;
};

} // namespace sight::filter::image.
