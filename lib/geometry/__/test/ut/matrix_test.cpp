/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <geometry/__/matrix.hpp>

#include <doctest/doctest.h>

#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_transform.hpp>

TEST_SUITE("sight::geometry::matrix")
{
//------------------------------------------------------------------------------

    TEST_CASE("inverse_translation_rotation")
    {
        glm::dmat4 matrix(1.);
        matrix = glm::translate(matrix, glm::dvec3(10., 20., -30));
        matrix = glm::rotate(matrix, glm::pi<double>() / 2., glm::dvec3(0., 0., 1.));

        const glm::dmat4 fast_inverse_matrix = sight::geometry::inverse_translation_rotation(matrix);
        const glm::dmat4 inverse_matrix      = glm::inverse(matrix);

        for(int i = 0 ; i < 4 ; ++i)
        {
            for(int j = 0 ; j < 4 ; ++j)
            {
                CHECK(fast_inverse_matrix[i][j] == doctest::Approx(inverse_matrix[i][j]).epsilon(1e-5));
            }
        }
    }

//-----------------------------------------------------------------------------
}
