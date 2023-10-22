/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "geometry/data/matrix4.hpp"

#include <data/matrix4.hpp>
#include <data/point.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>

namespace sight::geometry::data
{

//------------------------------------------------------------------------------

bool invert(
    const sight::data::matrix4& _input,
    sight::data::matrix4& _output
)
{
    // Normally we should transpose matrices since GLM uses a column-major layout and Sight uses row-major layout
    // However the transposition has a cost and inversion does not care about the layout, so we skip it
    const glm::dmat4x4 mat         = glm::make_mat4<double>(_input.data());
    const glm::dmat4x4 mat_inverse = glm::inverse(mat);

    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        const std::size_t row_dst = i * 4;
        const auto row_src        = static_cast<glm::length_t>(i);
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            const auto col_src = static_cast<glm::length_t>(j);
            _output[row_dst + j] = mat_inverse[row_src][col_src];
        }
    }

    return true;
}

// ----------------------------------------------------------------------------

void multiply(
    const sight::data::matrix4& _trf_a,
    const sight::data::matrix4& _trf_b,
    sight::data::matrix4& _output
)
{
    // Normally we should transpose matrices since GLM uses a column-major layout and Sight uses row-major layout
    // However the transposition has a cost, so it is faster to not transpose them
    // and perform the inverse multiplication
    const glm::dmat4x4 mat_a = glm::make_mat4<double>(_trf_a.data());
    const glm::dmat4x4 mat_b = glm::make_mat4<double>(_trf_b.data());

    const glm::dmat4x4 mat_c = mat_b * mat_a;

    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        const std::size_t row_dst = i * 4;
        const auto row_src        = static_cast<glm::length_t>(i);
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            const auto col_src = static_cast<glm::length_t>(j);
            _output[row_dst + j] = mat_c[row_src][col_src];
        }
    }
}

// ----------------------------------------------------------------------------

void identity(sight::data::matrix4& _trf)
{
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            _trf(i, j) = i == j ? 1 : 0;
        }
    }
}

// ----------------------------------------------------------------------------

void multiply(
    const sight::data::matrix4& _trf,
    const sight::data::point& _input,
    sight::data::point& _output
)
{
    // Normally we should transpose matrices since GLM uses a column-major layout and Sight uses row-major layout
    // However the transposition has a cost, so it is faster to not transpose them
    // and perform the inverse multiplication
    const glm::dmat4x4 mat = glm::make_mat4<double>(_trf.data());

    const auto& in_coord = _input.getCoord();
    glm::dvec4 in;
    in[0] = in_coord[0];
    in[1] = in_coord[1];
    in[2] = in_coord[2];
    in[3] = 1;

    glm::dvec4 out            = in * mat;
    std::array<double, 3> res = {{out[0], out[1], out[2]}};
    _output.setCoord(res);
}

// ----------------------------------------------------------------------------

bool is_identity(const sight::data::matrix4& _trf, const double _epsilon)
{
    static const sight::data::matrix4 s_IDENTITY;

    for(std::size_t i = 0 ; i < s_IDENTITY.size() ; ++i)
    {
        if(std::abs(s_IDENTITY[i] - _trf[i]) > _epsilon)
        {
            return false;
        }
    }

    return true;
}

// ----------------------------------------------------------------------------

} // namespace sight::geometry::data
