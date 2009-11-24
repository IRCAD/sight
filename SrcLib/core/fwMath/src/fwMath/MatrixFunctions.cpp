#include "fwMath/MatrixFunctions.hpp"
#include "fwMath/VectorFunctions.hpp"

namespace fwMath {

void multVecMatrix(const fwMatrix4x4 & _matrix, const fwVec3d& _source ,fwVec3d& _dest)
{
    SLM_TRACE_FUNC();
    double x,y,z;
    x = _source[0];
    y = _source[1];
    z = _source[2];

    _dest[0] = _matrix[0][0] * x + _matrix[1][0] * y + _matrix[2][0] * z + _matrix[3][0];
    _dest[1] = _matrix[0][1] * x + _matrix[1][1] * y + _matrix[2][1] * z + _matrix[3][1];
    _dest[2] = _matrix[0][2] * x + _matrix[1][2] * y + _matrix[2][2] * z + _matrix[3][2];
}

//------------------------------------------------------------------------------

fwMatrix4x4  getRotationMatrix( const fwVec3d &_vecNorm )
{
    fwMatrix4x4 R;

    const float FV0 = _vecNorm[0];
    const float FV1 = _vecNorm[1];
    const double YP  = sqrt ( FV0 * FV0 + FV1 * FV1 );
    const double RZ  = - atan2 (FV0, FV1);
    const double RX  = - atan2 (YP, _vecNorm[2]);


//     Rotation Matrix
//     [     cos(z)           sin(z)         0     0 ]
//     [                                             ]
//     [ - cos(x) sin(z)   cos(x) cos(z)   sin(x)  0 ]
//     [                                             ]
//     [  sin(x) sin(z)   - sin(x) cos(z)  cos(x)  0 ]
//     [                                             ]
//     [        0                0           0     1 ]

    R[0][0] =  cos(RZ);
    R[0][1] =  sin(RZ);
    R[0][2] =  0;
    R[0][3] =  0.;

    R[1][0] = -cos(RX) * sin(RZ);
    R[1][1] =  cos(RX) * cos(RZ);
    R[1][2] =  sin(RX);
    R[1][3] =  0.;
    
    R[2][0] =  sin(RX) * sin(RZ);
    R[2][1] = -sin(RX) * cos(RZ);
    R[2][2] =  cos(RX);
    R[2][3] =  0.;
    
    R[3][2] =  0.;
    R[3][3] =  1.;
    R[3][0] =  0.;
    R[3][1] =  0.;



    return R;
}

}
