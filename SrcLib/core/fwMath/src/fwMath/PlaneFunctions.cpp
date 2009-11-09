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
	if(vecLength(normalVec) <=0.0F) {
		normalVec[0] =0.;
		normalVec[1] =0.;
		normalVec[2] =1.;
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
	_plane[0] = _normal[0];
	_plane[1] = _normal[1];
	_plane[2] = _normal[2];
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
	const fwVec3d& pos = _line.first;
	const fwVec3d& dir = _line.second;
	fwVec3d normalVec = getNormal(_plane);
	double d = dot(normalVec, dir);
//	if(d<=EPSILON && d>=-EPSILON) return false;
	if(d == 0.0) return false;
	double distance = getDistance(_plane);
	double t = (distance - dot(normalVec, pos))/d;
	_point = pos + (t * dir);
	return true;
}

//------------------------------------------------------------------------------

bool isInHalfSpace(const fwPlane& _plane, const fwVec3d& _point)
{
	SLM_TRACE_FUNC();
	fwVec3d normalVec = getNormal(_plane);
	fwVec3d pos = normalVec * getDistance(_plane);
	return (dot(normalVec, _point-pos) >= 0.0 ? true : false);
}

//------------------------------------------------------------------------------

void transform(fwPlane& _plane, const fwMatrix& _matrix)
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
