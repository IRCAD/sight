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

#include "geometry/data/matrix_functions.hpp"

#include "geometry/data/vector_functions.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

namespace sight::geometry::data
{

//------------------------------------------------------------------------------

void mult_vec_matrix(const fwMatrix4x4& _matrix, const fwVec3d& _source, fwVec3d& _dest)
{
    // fwMatrix4x4 is stored row-major
    // glm matrices are stored column-major

    glm::dmat4x4 mat(_matrix[0][0], _matrix[1][0], _matrix[2][0], _matrix[3][0],
                     _matrix[0][1], _matrix[1][1], _matrix[2][1], _matrix[3][1],
                     _matrix[0][2], _matrix[1][2], _matrix[2][2], _matrix[3][2],
                     _matrix[0][3], _matrix[1][3], _matrix[2][3], _matrix[3][3]);

    glm::dvec4 vec(_source[0], _source[1], _source[2], 1.0);
    glm::dvec4 res = mat * vec;

    _dest[0] = res[0];
    _dest[1] = res[1];
    _dest[2] = res[2];
}

//------------------------------------------------------------------------------

fwMatrix4x4 get_inverse(const fwMatrix4x4& _matrix)
{
    // fwMatrix4x4 is stored row-major
    // glm matrices are stored column-major

    glm::dmat4x4 mat(_matrix[0][0], _matrix[1][0], _matrix[2][0], _matrix[3][0],
                     _matrix[0][1], _matrix[1][1], _matrix[2][1], _matrix[3][1],
                     _matrix[0][2], _matrix[1][2], _matrix[2][2], _matrix[3][2],
                     _matrix[0][3], _matrix[1][3], _matrix[2][3], _matrix[3][3]);

    glm::dmat4x4 mat_inv = glm::inverse(mat);

    fwMatrix4x4 inverse = {{{{mat_inv[0][0], mat_inv[1][0], mat_inv[2][0], mat_inv[3][0]}},
        {{mat_inv[0][1], mat_inv[1][1], mat_inv[2][1], mat_inv[3][1]}},
        {{mat_inv[0][2], mat_inv[1][2], mat_inv[2][2], mat_inv[3][2]}},
        {{mat_inv[0][3], mat_inv[1][3], mat_inv[2][3], mat_inv[3][3]
        }
        }
    }
    };
    return inverse;
}

//------------------------------------------------------------------------------

fwMatrix4x4 get_rotation_matrix(const fwVec3d& _vec_norm)
{
    fwMatrix4x4 r;

    const double f_v0 = _vec_norm[0];
    const double f_v1 = _vec_norm[1];
    const double yp   = sqrt(f_v0 * f_v0 + f_v1 * f_v1);
    const double rz   = -atan2(f_v0, f_v1);
    const double rx   = -atan2(yp, _vec_norm[2]);

    //     Rotation Matrix
    //     [     cos(z)           sin(z)         0     0 ]
    //     [                                             ]
    //     [ - cos(x) sin(z)   cos(x) cos(z)   sin(x)  0 ]
    //     [                                             ]
    //     [  sin(x) sin(z)   - sin(x) cos(z)  cos(x)  0 ]
    //     [                                             ]
    //     [        0                0           0     1 ]

    r[0][0] = cos(rz);
    r[0][1] = sin(rz);
    r[0][2] = 0;
    r[0][3] = 0.;

    r[1][0] = -cos(rx) * sin(rz);
    r[1][1] = cos(rx) * cos(rz);
    r[1][2] = sin(rx);
    r[1][3] = 0.;

    r[2][0] = sin(rx) * sin(rz);
    r[2][1] = -sin(rx) * cos(rz);
    r[2][2] = cos(rx);
    r[2][3] = 0.;

    r[3][0] = 0.;
    r[3][1] = 0.;
    r[3][2] = 0.;
    r[3][3] = 1.;

    return r;
}

//------------------------------------------------------------------------------

} // namespace sight::geometry::data

//------------------------------------------------------------------------------

fwMatrix4x4 operator*(const fwMatrix4x4& _matrix1, const fwMatrix4x4& _matrix2)
{
    glm::dmat4x4 mat1(_matrix1[0][0], _matrix1[1][0], _matrix1[2][0], _matrix1[3][0],
                      _matrix1[0][1], _matrix1[1][1], _matrix1[2][1], _matrix1[3][1],
                      _matrix1[0][2], _matrix1[1][2], _matrix1[2][2], _matrix1[3][2],
                      _matrix1[0][3], _matrix1[1][3], _matrix1[2][3], _matrix1[3][3]);

    glm::dmat4x4 mat2(_matrix2[0][0], _matrix2[1][0], _matrix2[2][0], _matrix2[3][0],
                      _matrix2[0][1], _matrix2[1][1], _matrix2[2][1], _matrix2[3][1],
                      _matrix2[0][2], _matrix2[1][2], _matrix2[2][2], _matrix2[3][2],
                      _matrix2[0][3], _matrix2[1][3], _matrix2[2][3], _matrix2[3][3]);

    glm::dmat4x4 prod = mat1 * mat2;

    fwMatrix4x4 product = {{{{prod[0][0], prod[1][0], prod[2][0], prod[3][0]}},
        {{prod[0][1], prod[1][1], prod[2][1], prod[3][1]}},
        {{prod[0][2], prod[1][2], prod[2][2], prod[3][2]}},
        {{prod[0][3], prod[1][3], prod[2][3], prod[3][3]
        }
        }
    }
    };

    return product;
}

//------------------------------------------------------------------------------
