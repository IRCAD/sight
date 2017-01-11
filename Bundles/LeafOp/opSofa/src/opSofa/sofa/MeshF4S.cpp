/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "opSofa/sofa/MeshF4S.hpp"

#include <fwDataTools/helper/Mesh.hpp>


using namespace sofa::defaulttype;

/**
 *  Method in charge of the load of the triangular mesh data from F4S in the
 *  mesh object from SOFA
 *  @param pMesh : pointer to the triangular mesh
 */
void MeshF4S::loadMesh(::fwData::Mesh::sptr pMesh)
{
    // Copy points to Sofa
    const size_t nbPoints = pMesh->getNumberOfPoints();
    ::fwDataTools::helper::Mesh meshHelper(pMesh);
    ::fwData::Mesh::PointsMultiArrayType points = meshHelper.getPoints();
    vertices.resize(nbPoints);
    for (int p = 0; p<nbPoints; p++)
    {
        vertices[p][0] = (SReal)points[p][0];
        vertices[p][1] = (SReal)points[p][1];
        vertices[p][2] = (SReal)points[p][2];
    }

    // Copy cells to Sofa
    const size_t nbCells = pMesh->getNumberOfCells();
    ::fwData::Mesh::CellDataMultiArrayType cells = meshHelper.getCellData();
    facets.resize(nbCells);
    for (int i = 0, f = 0; f<nbCells; f++,i += 3)
    {
        facets[f].resize(3);
        facets[f][0].resize(3);
        facets[f][1].resize(3);
        facets[f][2].resize(3);
        facets[f][0][0] = cells[i];
        facets[f][0][1] = cells[i+1];
        facets[f][0][2] = cells[i+2];
    }
}
