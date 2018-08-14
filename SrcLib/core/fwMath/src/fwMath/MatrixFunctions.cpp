/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMath/MatrixFunctions.hpp"
#include "fwMath/VectorFunctions.hpp"

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace fwMath
{

//------------------------------------------------------------------------------

void multVecMatrix(const fwMatrix4x4& matrix, const fwVec3d& source, fwVec3d& dest)
{
    // fwMatrix4x4 is stored row-major
    // glm matrices are stored column-major

    ::glm::dmat4x4 mat(matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0],
                       matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1],
                       matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2],
                       matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3]);

    ::glm::dvec4 vec(source[0], source[1], source[2], 1.0);
    ::glm::dvec4 res = mat * vec;

    dest[0] = res[0];
    dest[1] = res[1];
    dest[2] = res[2];
}

//------------------------------------------------------------------------------

fwMatrix4x4  getInverse( const fwMatrix4x4& matrix )
{
    // fwMatrix4x4 is stored row-major
    // glm matrices are stored column-major

    ::glm::dmat4x4 mat(matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0],
                       matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1],
                       matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2],
                       matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3]);

    ::glm::dmat4x4 matInv = ::glm::inverse(mat);

    fwMatrix4x4 inverse = {matInv[0][0], matInv[1][0], matInv[2][0], matInv[3][0],
                           matInv[0][1], matInv[1][1], matInv[2][1], matInv[3][1],
                           matInv[0][2], matInv[1][2], matInv[2][2], matInv[3][2],
                           matInv[0][3], matInv[1][3], matInv[2][3], matInv[3][3]};
    return inverse;
}

//------------------------------------------------------------------------------

fwMatrix4x4  getRotationMatrix( const fwVec3d& _vecNorm )
{
    fwMatrix4x4 R;

    const double FV0 = _vecNorm[0];
    const double FV1 = _vecNorm[1];
    const double YP  = sqrt( FV0 * FV0 + FV1 * FV1 );
    const double RZ  = -atan2(FV0, FV1);
    const double RX  = -atan2(YP, _vecNorm[2]);

    //     Rotation Matrix
    //     [     cos(z)           sin(z)         0     0 ]
    //     [                                             ]
    //     [ - cos(x) sin(z)   cos(x) cos(z)   sin(x)  0 ]
    //     [                                             ]
    //     [  sin(x) sin(z)   - sin(x) cos(z)  cos(x)  0 ]
    //     [                                             ]
    //     [        0                0           0     1 ]

    R[0][0] = cos(RZ);
    R[0][1] = sin(RZ);
    R[0][2] = 0;
    R[0][3] = 0.;

    R[1][0] = -cos(RX) * sin(RZ);
    R[1][1] = cos(RX) * cos(RZ);
    R[1][2] = sin(RX);
    R[1][3] = 0.;

    R[2][0] = sin(RX) * sin(RZ);
    R[2][1] = -sin(RX) * cos(RZ);
    R[2][2] = cos(RX);
    R[2][3] = 0.;

    R[3][0] = 0.;
    R[3][1] = 0.;
    R[3][2] = 0.;
    R[3][3] = 1.;

    return R;
}

//------------------------------------------------------------------------------
}

//------------------------------------------------------------------------------

fwMatrix4x4 operator*( const fwMatrix4x4& matrix1, const fwMatrix4x4& matrix2 )
{
    ::glm::dmat4x4 mat1(matrix1[0][0], matrix1[1][0], matrix1[2][0], matrix1[3][0],
                        matrix1[0][1], matrix1[1][1], matrix1[2][1], matrix1[3][1],
                        matrix1[0][2], matrix1[1][2], matrix1[2][2], matrix1[3][2],
                        matrix1[0][3], matrix1[1][3], matrix1[2][3], matrix1[3][3]);

    ::glm::dmat4x4 mat2(matrix2[0][0], matrix2[1][0], matrix2[2][0], matrix2[3][0],
                        matrix2[0][1], matrix2[1][1], matrix2[2][1], matrix2[3][1],
                        matrix2[0][2], matrix2[1][2], matrix2[2][2], matrix2[3][2],
                        matrix2[0][3], matrix2[1][3], matrix2[2][3], matrix2[3][3]);

    ::glm::dmat4x4 prod = mat1 * mat2;

    fwMatrix4x4 product = {prod[0][0], prod[1][0], prod[2][0], prod[3][0],
                           prod[0][1], prod[1][1], prod[2][1], prod[3][1],
                           prod[0][2], prod[1][2], prod[2][2], prod[3][2],
                           prod[0][3], prod[1][3], prod[2][3], prod[3][3]};

    return product;
}

//------------------------------------------------------------------------------
