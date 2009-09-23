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


} // namespace fwData
