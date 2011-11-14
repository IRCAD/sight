/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkPoints.h>
#include <vtkDataArray.h>
#include <vtkCell.h>

#include "vtkIO/helper/Mesh.hpp"

namespace vtkIO
{
namespace helper
{

//------------------------------------------------------------------------------

void Mesh::fromVTKMesh(  vtkSmartPointer<vtkPolyData> polyData, ::fwData::Mesh::sptr mesh )
{
    vtkPoints *points = polyData->GetPoints();
    if (points)
    {
        vtkIdType numberOfPoints = points->GetNumberOfPoints();
        vtkIdType numberOfCells = polyData->GetNumberOfCells();

        mesh->allocate(numberOfPoints, numberOfCells, numberOfCells*3);

        double* point;
        ::fwData::Mesh::Id idx;
        for (vtkIdType i = 0; i < numberOfPoints; ++i)
        {
            point = points->GetPoint(i);
            idx = mesh->insertNextPoint(point[0], point[1], point[2]);
            SLM_ASSERT("Mesh index not correspond to VTK index point", idx == i);
        }

        vtkCell* cell;
        vtkIdList* idList;
        int cellType;
        for (vtkIdType i = 0 ; i < numberOfCells ; ++i)
        {
            cell = polyData->GetCell(i);
            idList = cell->GetPointIds();
            cellType = cell->GetCellType();

            if(cellType == VTK_TRIANGLE)
            {
                SLM_ASSERT("Wrong number of ids: "<<idList->GetNumberOfIds(), idList->GetNumberOfIds()==3);
                mesh->insertNextCell( idList->GetId(0), idList->GetId(1), idList->GetId(2));
            }
            else if(cellType == VTK_QUAD)
            {
                SLM_ASSERT("Wrong number of ids: "<<idList->GetNumberOfIds(), idList->GetNumberOfIds()==4);
                mesh->insertNextCell( idList->GetId(0), idList->GetId(1), idList->GetId(2), idList->GetId(3));
            }
            else
            {
                FW_RAISE("VTK Mesh type "<<cellType<< " not supported.");
            }
        }
    }
}

//------------------------------------------------------------------------------

void Mesh::toVTKMesh( ::fwData::Mesh::sptr mesh, vtkSmartPointer<vtkPolyData> _polyData)
{
    vtkSmartPointer< vtkPoints > pts = vtkSmartPointer< vtkPoints >::New();
    _polyData->SetPoints(pts);
    Mesh::updatePolyDataPoints(_polyData, mesh);

    unsigned int nbCells = mesh->getNumberOfCells() ;

    ::fwData::Mesh::CellTypesMultiArrayType cellTypes = mesh->getCellTypes();
    ::fwData::Mesh::CellDataMultiArrayType cellData = mesh->getCellData();
    ::fwData::Mesh::CellDataOffsetsMultiArrayType cellDataOffsets = mesh->getCellDataOffsets();

    _polyData->Allocate(4, nbCells);

    vtkIdType typeVtkCell;
    vtkIdType cell[4];
    for(unsigned int i=0 ; i<nbCells ; ++i )
    {
        ::fwData::Mesh::CellTypes cellType = cellTypes[i];
        ::fwData::Mesh::Id offset = cellDataOffsets[i];
        if ( cellType == ::fwData::Mesh::TRIANGLE )
        {
            typeVtkCell = VTK_TRIANGLE;
            cell[0] = cellData[offset];
            cell[1] = cellData[offset+1];
            cell[2] = cellData[offset+2];
            _polyData->InsertNextCell( typeVtkCell, 3, cell );
        }
        else if ( cellType == ::fwData::Mesh::QUAD )
        {
            typeVtkCell = VTK_QUAD;
            cell[0] = cellData[offset];
            cell[1] = cellData[offset+1];
            cell[2] = cellData[offset+2];
            cell[3] = cellData[offset+3];
            _polyData->InsertNextCell( typeVtkCell, 4, cell );
        }
    }
}

//------------------------------------------------------------------------------

vtkSmartPointer<vtkPolyData> Mesh::updatePolyDataPoints(vtkSmartPointer<vtkPolyData> polyDataDst, ::fwData::Mesh::sptr meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    vtkPoints *polyDataPoints = polyDataDst->GetPoints();
    size_t nbPoints = polyDataPoints->GetNumberOfPoints();
    ::fwData::Mesh::PointMultiArrayType &points = meshSrc->getPoints();

    if (meshSrc->getNumberOfPoints() != polyDataPoints->GetNumberOfPoints())
    {
        polyDataPoints->SetNumberOfPoints(nbPoints);
    }

    vtkIdType id = 0;
    typedef ::fwData::Mesh::PointMultiArrayType::index PointTypesIndex;
    for (PointTypesIndex i = 0; i != nbPoints; ++i)
    {
        polyDataPoints->SetPoint(id++, points[i][0], points[i][1], points[i][2]);
    }

    polyDataPoints->Modified();
    return polyDataDst;
}

//------------------------------------------------------------------------------

} // namespace helper
} // namespace vtkIO
