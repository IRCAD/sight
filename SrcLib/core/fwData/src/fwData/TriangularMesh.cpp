/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/TriangularMesh.hpp"


REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::TriangularMesh, ::fwData::TriangularMesh );
namespace fwData
{
TriangularMesh::TriangularMesh()
{}

TriangularMesh::~TriangularMesh()
{
	m_points.clear();
	m_cells.clear();
}

TriangularMesh::PointContainer &TriangularMesh::points()
{
	return m_points ;
}
TriangularMesh::CellContainer &TriangularMesh::cells()
{
	return m_cells ;
}

void TriangularMesh::setOneVectorInPointList(int _iIndex, double _fX, double _fY, double _fZ)
{
        assert( (0 <= _iIndex) );
        if ( _iIndex >= m_points.size() )// Resize if necessary
                m_points.resize( _iIndex + 1 );
	std::vector<float> v(3);
	m_points[_iIndex] = v;
	m_points[_iIndex][0] = _fX;
	m_points[_iIndex][1] = _fY;
	m_points[_iIndex][2] = _fZ;
}
void TriangularMesh::setOneIndexInIndexList(int _iIndex, int _p1, int _p2, int _p3)
{
        assert( (0 <= _iIndex) );
        if ( _iIndex >= m_cells.size() )// Resize if necessary
                m_cells.resize( _iIndex + 1 );
	std::vector<int> v(3);
	m_cells[_iIndex] = v;
	m_cells[_iIndex][0] = _p1;
	m_cells[_iIndex][1] = _p2;
	m_cells[_iIndex][2] = _p3;
}
double* TriangularMesh::getOneVectorFromPointList(int _pt) /* const */  {
	double *vec  = new double[3];
	vec[0] = (double) m_points[_pt][0];
	vec[1] =  (double) m_points[_pt][1];
	vec[2] =  (double) m_points[_pt][2];
	return vec;
}

int TriangularMesh::getOneIndexFromIndexList(void) /*const*/ { return 0; } //FIXME
void TriangularMesh::setOneVectorInNormalList(int x, double, double, double) { x = 0; } //FIXME
int TriangularMesh::getOneIndexFromNormalList(void) /* const */ { return 1; } //FIXME


} // namespace fwData
