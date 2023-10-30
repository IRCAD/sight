/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "filter/image/matrix_regressor.hpp"

#include <filter/image/powell_optimizer.hpp>

#include <geometry/data/matrix4.hpp>

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#undef GLM_ENABLE_EXPERIMENTAL
#include <glm/mat4x4.hpp>
#include <utility>

namespace sight::filter::image
{

//-----------------------------------------------------------------------------

matrix_regressor::matrix_regressor(const data::vector::csptr& _matrix_list, std::vector<point_t> _points) :
    m_point_list(std::move(_points))
{
    for(const auto& elt : *_matrix_list)
    {
        data::matrix4::sptr mat = std::dynamic_pointer_cast<data::matrix4>(elt);

        m_mat_list.push_back(geometry::data::to_glm_mat(*mat));
    }
}

//-----------------------------------------------------------------------------

data::matrix4::sptr matrix_regressor::minimize(
    const data::matrix4& _init_value,
    double _step_length,
    double _step_tolerance,
    double _value_tolerance,
    unsigned int _max_iter
)
{
    glm::dmat4 init_mat = geometry::data::to_glm_mat(_init_value);
    double scale        = std::pow(glm::determinant(init_mat), 1. / 3.);

    glm::dvec3 angles = glm::eulerAngles(glm::toQuat(init_mat / scale));

    glm::dvec3 translation = glm::dvec3(glm::column(init_mat, 3));

    powell_optimizer::function_parameters_t init_params(7);
    init_params.put(0, translation[0]);
    init_params.put(1, translation[1]);
    init_params.put(2, translation[2]);
    init_params.put(3, angles[0]);
    init_params.put(4, angles[1]);
    init_params.put(5, angles[2]);
    init_params.put(6, scale);

    powell_optimizer::optimized_function_t distance_sum =
        [this](const powell_optimizer::function_parameters_t& _parameters)
        {
            glm::dmat4 mat = glm::eulerAngleYXZ(_parameters[4], _parameters[3], _parameters[5]);
            mat = glm::translate(mat, glm::dvec3(_parameters[0], _parameters[1], _parameters[2]));
            mat = glm::scale(mat, glm::dvec3(_parameters[6], _parameters[6], _parameters[6]));

            double distance = 0;

            for(const glm::dmat4& m : this->m_mat_list)
            {
                for(const matrix_regressor::point_t& p : this->m_point_list)
                {
                    distance += glm::distance2(m * p, mat * p);
                }
            }

            return distance;
        };

    powell_optimizer optimizer(distance_sum, _step_tolerance, _value_tolerance, _step_length, _max_iter);

    powell_optimizer::function_parameters_t final_position = optimizer.optimize(init_params);

    glm::dmat4 result = glm::eulerAngleYXZ(final_position[4], final_position[3], final_position[5]);
    result = glm::translate(result, glm::dvec3(final_position[0], final_position[1], final_position[2]));
    result = glm::scale(result, glm::dvec3(final_position[6], final_position[6], final_position[6]));

    data::matrix4::sptr res_mat = std::make_shared<data::matrix4>();
    geometry::data::from_glm_mat(*res_mat, result);

    return res_mat;
}

//-----------------------------------------------------------------------------

} // namespace sight::filter::image.
