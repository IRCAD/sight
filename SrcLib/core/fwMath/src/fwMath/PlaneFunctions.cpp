/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMath/PlaneFunctions.hpp"

namespace fwMath {

//------------------------------------------------------------------------------

fwPlane getPlane(const fwVec3d & _point1, const fwVec3d & _point2, const fwVec3d & _point3) {
	SLM_TRACE_FUNC();
	fwPlane plane;
	setValues(plane, _point1, _point2, _point3);

	return plane;
}

//------------------------------------------------------------------------------

void setValues(fwPlane& _plane, const fwVec3d & _point1, const fwVec3d & _point2, const fwVec3d & _point3)
{
	SLM_TRACE_FUNC();
	fwVec3d  normalVec= cross(_point2 - _point1, _point3 -_point1);
	if((float)(vecLength(normalVec)) <= 0.0F) {
		normalVec[0] =0.0F;
		normalVec[1] =0.0F;
		normalVec[2] =1.0F;
	}
	normalize(normalVec);
	// Normal
	setNormal(_plane, normalVec);
	// Distance
	double distance = normalVec[0]*_point1[0] + normalVec[1]*_point1[1] + normalVec[2]*_point1[2];
	setDistance(_plane, distance);
}

//------------------------------------------------------------------------------
fwVec3d getNormal(const fwPlane& _plane) {
	SLM_TRACE_FUNC();
	fwVec3d normalVec;
	normalVec[0] = _plane[0];
	normalVec[1] = _plane[1];
	normalVec[2] = _plane[2];

	return normalVec;
}

//------------------------------------------------------------------------------
void  setNormal(fwPlane& _plane, const fwVec3d& _normal) {
	SLM_TRACE_FUNC();
	fwVec3d normalNormalized(_normal);
	normalize(normalNormalized);
	_plane[0] = normalNormalized[0];
	_plane[1] = normalNormalized[1];
	_plane[2] = normalNormalized[2];
	;
}
//------------------------------------------------------------------------------

double getDistance(const fwPlane& _plane) {
	SLM_TRACE_FUNC();
	return _plane[3];
}

//------------------------------------------------------------------------------

void  setDistance(fwPlane& _plane, const double _distance) {
	SLM_TRACE_FUNC();
	_plane[3] = _distance;
}

//------------------------------------------------------------------------------
bool intersect( const fwPlane& _plane, const fwLine & _line,  fwVec3d & _point) {
	SLM_TRACE_FUNC();

	const fwVec3d pos = _line.first;
	fwVec3d dirNormaliser = _line.second - _line.first;
	::fwMath::normalize(dirNormaliser);

	fwVec3d normalVec = getNormal(_plane);
	double d = dot(normalVec, dirNormaliser);
	if(abs((float)d) < 0.0F) return false;
	double distance = getDistance(_plane);
	double t = (distance - dot(normalVec, pos))/d;
	_point = pos + (t * dirNormaliser);

	return true;
}

//------------------------------------------------------------------------------

bool isInHalfSpace(const fwPlane& _plane, const fwVec3d& _point)
{
	SLM_TRACE_FUNC();
	fwVec3d normalVec = getNormal(_plane);
	fwVec3d pos = normalVec * getDistance(_plane);
	return ((float)(dot(normalVec, _point-pos)) >= 0.0F ? true : false);
}

//------------------------------------------------------------------------------

void transform(fwPlane& _plane, const fwMatrix4x4& _matrix)
{
	SLM_TRACE_FUNC();
	fwVec3d normalVec = getNormal(_plane);
	fwVec3d beg = normalVec * getDistance(_plane);
	fwVec3d end = beg + normalVec;
	multVecMatrix(_matrix,beg,beg);
	multVecMatrix(_matrix,end,end);
	normalVec = end - beg;
	normalize(normalVec);
	setNormal(_plane, normalVec);
	setDistance(_plane, dot(normalVec, beg));

}

//------------------------------------------------------------------------------

void offset(fwPlane& _plane, double _offset)
{
	SLM_TRACE_FUNC();
	double distance = getDistance(_plane);
	distance += _offset;
	setDistance(_plane, distance);

}

fwPlane getPlane(const fwVec3d&  _normal,const fwVec3d& _point)
{
	SLM_TRACE_FUNC();
	fwPlane plane;
	fwVec3d normalVec = _normal;
	normalize(normalVec);
	setNormal(plane, normalVec);
	setDistance(plane, normalVec[0]*_point[0] +normalVec[1]*_point[1] + normalVec[2]*_point[2]);
	return plane;
}
}
