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

	const double FV0 = _vecNorm[0];
    const double FV1 = _vecNorm[1];
    const double YP = sqrt ( FV0 * FV0 + FV1 * FV1 );
	const double AngleY = 0.f;
    const double AngleZ = - atan2 (FV0, FV1);
    const double AngleX = - atan2 (YP, _vecNorm[2]);

	//R = [cos(z)cos(y)		sin(z)cos(x)-cos(z)sin(y)sin(x)		sin(z)sin(x)+cos(z)sin(y)cos(x)	0
	//     -sin(z)cos(y)	cos(z)cos(x)+sin(z)sin(y)sin(x)		cos(z)sin(x)-sin(z)sin(y)cos(x)	0
	//     -sin(y)			-cos(y)sin(x)						cos(y)cos(x)					0
	//		0				0									0								1]
	R[0][0] = cos(AngleZ)*cos(AngleY);
	R[0][1] = sin(AngleZ)*cos(AngleX)-cos(AngleZ)*sin(AngleY)*sin(AngleX);
	R[0][2] = sin(AngleZ)*sin(AngleX)+cos(AngleZ)*sin(AngleY)*cos(AngleX);
    R[0][3] = 0.f;

	R[1][0] = -sin(AngleZ)*cos(AngleY);
	R[1][1] = cos(AngleZ)*cos(AngleX)+sin(AngleZ)*sin(AngleY)*sin(AngleX);
	R[1][2] = cos(AngleZ)*sin(AngleX)-sin(AngleZ)*sin(AngleY)*cos(AngleX);
    R[1][3] = 0.f;

	R[2][0] = -sin(AngleY);
	R[2][1] = -cos(AngleY)*sin(AngleX);
	R[2][2] = cos(AngleY)*cos(AngleX);
	R[2][3] = 0.f;

	R[3][0] = 0.f;
	R[3][1] = 0.f;
	R[3][2] = 0.f;
	R[3][3] = 1.f;

	return R;
}

}
