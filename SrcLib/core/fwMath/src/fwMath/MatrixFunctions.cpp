#include "fwMath/VectorFunctions.hpp"

namespace fwMath {

void multVecMatrix(const fwMatrix & _matrix, const fwVec3d& _source ,fwVec3d& _dest)
{
	double x,y,z;
	x = _source[0];
	y = _source[1];
	z = _source[2];

	_dest[0] = _matrix[0][0] * x + _matrix[1][0] * y + _matrix[2][0] * z + _matrix[3][0];
	_dest[1] = _matrix[0][1] * x + _matrix[1][1] * y + _matrix[2][1] * z + _matrix[3][1];
	_dest[2] = _matrix[0][2] * x + _matrix[1][2] * y + _matrix[2][2] * z + _matrix[3][2];
}
}
