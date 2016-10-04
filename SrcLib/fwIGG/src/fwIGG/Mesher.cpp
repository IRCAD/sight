/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwIGG/Mesher.hpp"
#include "fwIGG/Mesher.hxx"

#include <Algo/MC/marchingcube.h>

#include <Mesher/parataubin.h>
#include <Mesher/prep_mesh.h>
#include <Mesher/simplifvoxmesh.h>

#include <Topology/generic/parameters.h>
#include <Topology/map/embeddedMap2.h>

#include <boost/assign/list_of.hpp>

namespace fwIGG
{

//-----------------------------------------------------------------------------

void Mesher::convertCgognMesh( ::boost::shared_ptr<CGoGN::EmbeddedMap2> pfpMap,
                               ::fwData::Mesh::sptr mesh,
                               ::fwData::Image::OriginType origin )
{
    unsigned int nbf                           = pfpMap->getNbOrbits<CGoGN::FACE>();
    CGoGN::VertexAttribute<PFP::VEC3> position = pfpMap->getAttribute<PFP::VEC3,CGoGN::VERTEX>("position");

    mesh->allocate( position.nbElements(), nbf, nbf*3 );
    mesh->setCellDataSize(nbf*3);
    mesh->setNumberOfCells( nbf );
    mesh->setNumberOfPoints( position.nbElements() );

    auto helperPoints          = ::fwDataTools::helper::Array::New(mesh->getPointsArray());
    auto helperCellTypes       = ::fwDataTools::helper::Array::New(mesh->getCellTypesArray());
    auto helperCellData        = ::fwDataTools::helper::Array::New(mesh->getCellDataArray());
    auto helperCellDataOffsets = ::fwDataTools::helper::Array::New(mesh->getCellDataOffsetsArray());

    // Set points
    unsigned int j = 0;

    CGoGN::VertexAttribute<unsigned int> indices = pfpMap->getAttribute<unsigned int,CGoGN::VERTEX>("indices");

    if (!indices.isValid())
    {
        indices = pfpMap->addAttribute<unsigned int,CGoGN::VERTEX>("indices");
    }


    j                                      = 0;
    ::fwData::Mesh::PointValueType* points = helperPoints->begin< ::fwData::Mesh::PointValueType >();
    for (unsigned int i = position.begin(); i< position.end(); position.next(i))
    {
        const PFP::VEC3& P = position[i];
        indices[i] = j++;

        points[0] = P[0] + origin[0];
        points[1] = P[1] + origin[1];
        points[2] = P[2] + origin[2];
        points   += 3;
    }

    // Set cells
    ::fwData::Mesh::CellValueType* cells = helperCellData->begin< ::fwData::Mesh::CellValueType >();
    CGoGN::TraversorCell<PFP::MAP,CGoGN::FACE> tra(*pfpMap);
    for (CGoGN::Dart d = tra.begin(); d!=tra.end(); d = tra.next())
    {
        unsigned int idx1 = indices[d];
        d = pfpMap->phi1(d);
        unsigned int idx2 = indices[d];
        d = pfpMap->phi1(d);
        unsigned int idx3 = indices[d];

        cells[0] = idx1;
        cells[1] = idx2;
        cells[2] = idx3;
        cells   += 3;
    }

    ::fwData::Mesh::CellTypes* cellTypes = helperCellTypes->begin< ::fwData::Mesh::CellTypes >();
    while( cellTypes < helperCellTypes->end< ::fwData::Mesh::CellTypes >() )
    {
        *cellTypes = ::fwData::Mesh::TRIANGLE;
        cellTypes++;
    }

    ::fwData::Mesh::CellDataOffsetType off              = 0;
    ::fwData::Mesh::CellDataOffsetType* cellDataOffsets =
        helperCellDataOffsets->begin< ::fwData::Mesh::CellDataOffsetType >();

    while( cellDataOffsets < helperCellDataOffsets->end< ::fwData::Mesh::CellDataOffsetType >() )
    {
        *cellDataOffsets = off;
        cellDataOffsets++;
        off += 3;
    }
}

//-----------------------------------------------------------------------------

} // namespace fwIGG

