/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMath/VectorFunctions.hpp"

namespace fwMath {

double normalize(fwVec3d & vec) 
{
	SLM_TRACE_FUNC();
	double norme = sqrt (vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	if((float)(norme)==0.0F) return 0.0F;
	vec /= norme;
	return norme;
}

//------------------------------------------------------------------------------

fwVec3d normalized(fwVec3d & vec) 
{
	SLM_TRACE_FUNC();
    fwVec3d normalized = vec;
    normalize(normalized);
	return normalized;
}

//------------------------------------------------------------------------------

double dot(const fwVec3d& v1, const fwVec3d& v2) 
{
	SLM_TRACE_FUNC();
	return (v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]);
}

//------------------------------------------------------------------------------

fwVec3d cross(const fwVec3d& v1, const fwVec3d& v2) 
{
	SLM_TRACE_FUNC();
	fwVec3d v;
	v[0] = v1[1] * v2[2] - v1[2] * v2[1];
	v[1] = v1[2] * v2[0] - v1[0] * v2[2];
	v[2] = v1[0] * v2[1] - v1[1] * v2[0];

	return v;
}

//------------------------------------------------------------------------------

double vecLength(fwVec3d &_vec) 
{
	SLM_TRACE_FUNC();
	return sqrt(_vec[0] * _vec[0] + _vec[1] * _vec[1] + _vec[2] * _vec[2]);
}

void negate(fwVec3d &_vec)
{
	SLM_TRACE_FUNC();
	_vec[0]=-_vec[0];
	_vec[1]=-_vec[1];
	_vec[2]=-_vec[2];
}

}

//------------------------------------------------------------------------------

fwVec3d& operator*=(fwVec3d& vec1, double val) {
	SLM_TRACE_FUNC();
	vec1[0] *= val;
	vec1[1] *= val;
	vec1[2] *= val;
	return vec1;

}

//------------------------------------------------------------------------------

fwVec3d& operator/=(fwVec3d& vec,  double val ) {
	if((double)(val)!=0.0F) {
		vec[0] /= val;
		vec[1] /= val;
		vec[2] /= val;
	}
	return vec;
}

//------------------------------------------------------------------------------

fwVec3d& operator+=(fwVec3d& vec1, fwVec3d& vec2) {
	vec1[0] += vec2[0];
	vec1[1] += vec2[1];
	vec1[2] += vec2[2];
	return vec1;
}

//------------------------------------------------------------------------------

fwVec3d& operator-=(fwVec3d& vec1, fwVec3d& vec2) {
	vec1[0] -= vec2[0];
	vec1[1] -= vec2[1];
	vec1[2] -= vec2[2];
	return vec1;
}

//------------------------------------------------------------------------------

fwVec3d operator*(const fwVec3d& _vec, const double _val)
{
	SLM_TRACE_FUNC();
	fwVec3d v;
	v[0] = _vec[0] * _val;
	v[1] = _vec[1] * _val;
	v[2] = _vec[2] * _val;
	return v;
}

//------------------------------------------------------------------------------

fwVec3d operator*(const double _val, const fwVec3d& _vec)
{
	SLM_TRACE_FUNC();
	return _vec * _val;
}

//------------------------------------------------------------------------------

fwVec3d operator/(const fwVec3d& _vec, double _val)
{
	fwVec3d v;
	v[0] = _vec[0] / _val;
	v[1] = _vec[1] / _val;
	v[2] = _vec[2] / _val;

	return v;
}

//------------------------------------------------------------------------------

fwVec3d operator+(const fwVec3d& _vec1, const fwVec3d& _vec2)
{
	SLM_TRACE_FUNC();
	fwVec3d v;
	v[0] = _vec1[0] + _vec2[0];
	v[1] = _vec1[1] + _vec2[1];
	v[2] = _vec1[2] + _vec2[2];

	return v;
}

//------------------------------------------------------------------------------

fwVec3d operator-(const fwVec3d& _vec1, const fwVec3d& _vec2)
{
	fwVec3d v;
	v[0] = _vec1[0] - _vec2[0];
	v[1] = _vec1[1] - _vec2[1];
	v[2] = _vec1[2] - _vec2[2];

	return v;
}

//------------------------------------------------------------------------------

int operator==(const fwVec3d& _vec1, const fwVec3d& _vec2)
{
	return (((float)(_vec1[0]) == (float)(_vec2[0])) && 
            ((float)(_vec1[1]) == (float)(_vec2[1])) && 
            ((float)(_vec1[2]) == (float)(_vec2[2]))
            ) ;
}

//------------------------------------------------------------------------------

int operator!=(const fwVec3d& _vec1, const fwVec3d& _vec2)
{
	return !(_vec1 == _vec2);
}
