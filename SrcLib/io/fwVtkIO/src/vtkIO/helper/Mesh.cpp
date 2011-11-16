/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkPoints.h>
#include <vtkDataArray.h>
#include <vtkPointData.h>
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
        mesh->clear();
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
        mesh->adjustAllocatedMemory();
    }
}

//------------------------------------------------------------------------------

void Mesh::toVTKMesh( ::fwData::Mesh::sptr mesh, vtkSmartPointer<vtkPolyData> polyData)
{
    vtkSmartPointer< vtkPoints > pts = vtkSmartPointer< vtkPoints >::New();
    polyData->SetPoints(pts);
    Mesh::updatePolyDataPoints(polyData, mesh);

    unsigned int nbCells = mesh->getNumberOfCells() ;

    ::fwData::Mesh::CellTypesMultiArrayType cellTypes = mesh->getCellTypes();
    ::fwData::Mesh::CellDataMultiArrayType cellData = mesh->getCellData();
    ::fwData::Mesh::CellDataOffsetsMultiArrayType cellDataOffsets = mesh->getCellDataOffsets();

    polyData->Allocate(4, nbCells);

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
            polyData->InsertNextCell( typeVtkCell, 3, cell );
        }
        else if ( cellType == ::fwData::Mesh::QUAD )
        {
            typeVtkCell = VTK_QUAD;
            cell[0] = cellData[offset];
            cell[1] = cellData[offset+1];
            cell[2] = cellData[offset+2];
            cell[3] = cellData[offset+3];
            polyData->InsertNextCell( typeVtkCell, 4, cell );
        }
    }
    if(mesh->getPointColorsArray())
    {
        unsigned char col[3] = {255, 0, 0};
        ::fwData::Mesh::PointColorsMultiArrayType pointsColor = mesh->getPointColors();

        vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        colors->SetNumberOfComponents(3);
        colors->SetName("Colors");
        colors->InsertNextTupleValue(col);

        typedef ::fwData::Mesh::PointsMultiArrayType::index PointTypesIndex;
        ::fwData::Mesh::Id nbPoints = mesh->getNumberOfPoints() ;
        for (PointTypesIndex i = 0; i != nbPoints; ++i)
        {
            col[0] = pointsColor[i][0];
            col[1] = pointsColor[i][1];
            col[2] = pointsColor[i][2];
            colors->InsertNextTupleValue(col);
        }

        polyData->GetPointData()->SetScalars(colors);
    }

}

//------------------------------------------------------------------------------

vtkSmartPointer<vtkPolyData> Mesh::updatePolyDataPoints(vtkSmartPointer<vtkPolyData> polyDataDst, ::fwData::Mesh::sptr meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    vtkPoints *polyDataPoints = polyDataDst->GetPoints();
    ::fwData::Mesh::Id nbPoints = meshSrc->getNumberOfPoints() ;
    ::fwData::Mesh::PointsMultiArrayType points = meshSrc->getPoints();

    if (nbPoints != polyDataPoints->GetNumberOfPoints())
    {
        polyDataPoints->SetNumberOfPoints(nbPoints);
    }

    vtkIdType id = 0;
    typedef ::fwData::Mesh::PointsMultiArrayType::index PointTypesIndex;
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
