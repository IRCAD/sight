/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"

#include "fwData/TriangularMesh.hpp"


fwDataRegisterMacro( ::fwData::TriangularMesh );
namespace fwData
{

TriangularMesh::TriangularMesh()
{}

TriangularMesh::~TriangularMesh()
{
    m_points.clear();
    m_cells.clear();
}

//-----------------------------------------------------------------------------

void TriangularMesh::shallowCopy( TriangularMesh::csptr _source )
{
    this->fieldShallowCopy( _source );
    this->m_points = _source->m_points;
    this->m_cells = _source->m_cells;
}

//-----------------------------------------------------------------------------

void TriangularMesh::deepCopy( TriangularMesh::csptr _source )
{
    this->fieldDeepCopy( _source );
    this->m_points = _source->m_points;
    this->m_cells = _source->m_cells;
}

//-----------------------------------------------------------------------------

TriangularMesh::PointContainer &TriangularMesh::points()
{
    return m_points ;
}

//-----------------------------------------------------------------------------

TriangularMesh::ConstPointContainer &TriangularMesh::points() const
{
    return m_points ;
}

//-----------------------------------------------------------------------------

TriangularMesh::CellContainer &TriangularMesh::cells()
{
    return m_cells ;
}

//-----------------------------------------------------------------------------

TriangularMesh::ConstCellContainer &TriangularMesh::cells() const
{
    return m_cells ;
}

//-----------------------------------------------------------------------------

void TriangularMesh::setOneVectorInPointList(int _iIndex, double _fX, double _fY, double _fZ)
{
    assert( (0 <= _iIndex) );
    if ( ((PointContainer::size_type)_iIndex) >= m_points.size() )// Resize if necessary
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
    if ( ((CellContainer::size_type)_iIndex) >= m_cells.size() )// Resize if necessary
        m_cells.resize( _iIndex + 1 );
    std::vector<int> v(3);
    m_cells[_iIndex] = v;
    m_cells[_iIndex][0] = _p1;
    m_cells[_iIndex][1] = _p2;
    m_cells[_iIndex][2] = _p3;
}

double* TriangularMesh::getOneVectorFromPointList(int _pt) const
{
    double *vec  = new double[3];
    vec[0] = (double) m_points[_pt][0];
    vec[1] =  (double) m_points[_pt][1];
    vec[2] =  (double) m_points[_pt][2];
    return vec;
}

const int TriangularMesh::getOneIndexFromIndexList(void) const { return 0; } //FIXME

void TriangularMesh::setOneVectorInNormalList(int x, double, double, double) { x = 0; } //FIXME

const int TriangularMesh::getOneIndexFromNormalList(void) const { return 1; } //FIXME

const int TriangularMesh::getNumPoints() const
{
    return ( (int)m_points.size() );
}

const int TriangularMesh::getNumCells() const
{
    return ( (int)m_cells.size() );
}

void TriangularMesh::clearPoints()
{
    PointContainer emptyVector; // Size=0 and capacity=0
    m_points.clear();
    // The swap reinitialize the size value and capacity to 0
    m_points.swap(emptyVector);
}

void TriangularMesh::clearCells()
{
    CellContainer emptyVector; //Size=0 and capacity=0
    m_cells.clear();
    // The swap reinitialize the size value and capacity to 0
    m_cells.swap(emptyVector);
}

} // namespace fwData
