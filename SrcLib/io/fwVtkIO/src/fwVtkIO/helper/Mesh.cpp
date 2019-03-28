/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "fwVtkIO/helper/Mesh.hpp"

#include <fwData/Array.hpp>

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/ArrayGetter.hpp>
#include <fwDataTools/helper/Mesh.hpp>
#include <fwDataTools/helper/MeshGetter.hpp>

#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkDataArray.h>
#include <vtkExtractUnstructuredGrid.h>
#include <vtkFillHolesFilter.h>
#include <vtkFloatArray.h>
#include <vtkGeometryFilter.h>
#include <vtkMassProperties.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyDataNormals.h>
#include <vtkSmartPointer.h>

namespace fwVtkIO
{
namespace helper
{

//------------------------------------------------------------------------------

void Mesh::fromVTKMesh(  vtkSmartPointer<vtkPolyData> polyData, ::fwData::Mesh::sptr mesh )
{
    vtkPoints* points = polyData->GetPoints();
    if (points)
    {
        mesh->clear();
        const vtkIdType numberOfPoints = points->GetNumberOfPoints();
        const vtkIdType numberOfCells  = polyData->GetNumberOfCells();

        mesh->allocate(numberOfPoints, numberOfCells, numberOfCells*3);
        ::fwDataTools::helper::Mesh meshHelper(mesh);

        const double* point;
        ::fwData::Mesh::Id idx;
        for (vtkIdType i = 0; i < numberOfPoints; ++i)
        {
            point = points->GetPoint(i);
            idx   = meshHelper.insertNextPoint(point[0], point[1], point[2]);
            SLM_ASSERT("Mesh index not correspond to VTK index point", idx == i);
        }

        vtkCell* cell;
        vtkIdList* idList;
        int cellType;
        for (vtkIdType i = 0; i < numberOfCells; ++i)
        {
            cell     = polyData->GetCell(i);
            idList   = cell->GetPointIds();
            cellType = cell->GetCellType();

            switch (cellType)
            {
                case VTK_VERTEX:
                    SLM_ASSERT("Wrong number of ids: "<<idList->GetNumberOfIds(), idList->GetNumberOfIds() == 1);
                    meshHelper.insertNextCell( idList->GetId(0));
                    break;
                case VTK_LINE:
                    SLM_ASSERT("Wrong number of ids: "<<idList->GetNumberOfIds(), idList->GetNumberOfIds() == 2);
                    meshHelper.insertNextCell( idList->GetId(0), idList->GetId(1));
                    break;
                case VTK_TRIANGLE:
                    SLM_ASSERT("Wrong number of ids: "<<idList->GetNumberOfIds(), idList->GetNumberOfIds() == 3);
                    meshHelper.insertNextCell( idList->GetId(0), idList->GetId(1), idList->GetId(2));
                    break;
                case VTK_QUAD:
                    SLM_ASSERT("Wrong number of ids: "<<idList->GetNumberOfIds(), idList->GetNumberOfIds() == 4);
                    meshHelper.insertNextCell( idList->GetId(0), idList->GetId(1), idList->GetId(2), idList->GetId(3));
                    break;
                case VTK_POLY_VERTEX:
                    for(vtkIdType i = 0; i < idList->GetNumberOfIds(); ++i)
                    {
                        meshHelper.insertNextCell( idList->GetId(i));
                    }
                    break;
                default:
                    FW_RAISE("VTK Mesh type "<<cellType<< " not supported.");
            }
        }

        if(polyData->GetPointData()->HasArray("Colors"))
        {
            vtkSmartPointer<vtkUnsignedCharArray> colors;
            colors = vtkUnsignedCharArray::SafeDownCast(polyData->GetPointData()->GetArray("Colors"));
            FW_RAISE_IF("Only vtkUnsignedCharArray is supported to manage color.", !colors);

            const size_t nbComponents = colors->GetNumberOfComponents();
            SLM_ASSERT("Wrong nb of components ("<<nbComponents<<")",
                       nbComponents == 3 || nbComponents == 4);
            mesh->allocatePointColors((::fwData::Mesh::ColorArrayTypes)nbComponents);
            meshHelper.updateLock();

            ::fwData::Mesh::Id nbPoints = mesh->getNumberOfPoints();
            for (size_t i = 0; i != nbPoints; ++i)
            {
                meshHelper.setPointColor(i, colors->GetPointer(i*nbComponents));
            }
        }

        if(polyData->GetCellData()->HasArray("Colors"))
        {
            vtkSmartPointer<vtkUnsignedCharArray> colors;
            colors = vtkUnsignedCharArray::SafeDownCast(polyData->GetCellData()->GetArray("Colors"));
            FW_RAISE_IF("Only vtkUnsignedCharArray is supported to manage color.", !colors);

            size_t nbComponents = colors->GetNumberOfComponents();
            SLM_ASSERT("Wrong nb of components ("<<nbComponents<<")",
                       nbComponents == 3 || nbComponents == 4);
            mesh->allocateCellColors((::fwData::Mesh::ColorArrayTypes)nbComponents);
            meshHelper.updateLock();

            const ::fwData::Mesh::Id nbCells = mesh->getNumberOfCells();
            for (size_t i = 0; i != nbCells; ++i)
            {
                meshHelper.setCellColor(i, colors->GetPointer(i*nbComponents));
            }
        }

        if(polyData->GetPointData()->GetAttribute(vtkDataSetAttributes::NORMALS))
        {
            vtkSmartPointer<vtkFloatArray> normals;
            normals = vtkFloatArray::SafeDownCast(polyData->GetPointData()->GetNormals());
            FW_RAISE_IF("Only vtkFloatArray is supported to manage normals.", !normals);

            size_t nbComponents = normals->GetNumberOfComponents();
            SLM_ASSERT("Wrong nb of components ("<<nbComponents<<")", nbComponents == 3);

            mesh->allocatePointNormals();
            meshHelper.updateLock();

            const ::fwData::Mesh::Id nbPoints = mesh->getNumberOfPoints();
            for (size_t i = 0; i != nbPoints; ++i)
            {
                meshHelper.setPointNormal(i, normals->GetPointer(i*nbComponents));
            }
        }

        if(polyData->GetCellData()->GetAttribute(vtkDataSetAttributes::NORMALS))
        {
            vtkSmartPointer<vtkFloatArray> normals;
            normals = vtkFloatArray::SafeDownCast(polyData->GetCellData()->GetNormals());
            FW_RAISE_IF("Only vtkFloatArray is supported to manage normals.", !normals);

            size_t nbComponents = normals->GetNumberOfComponents();
            SLM_ASSERT("Wrong nb of components ("<<nbComponents<<")", nbComponents == 3);

            mesh->allocateCellNormals();
            meshHelper.updateLock();

            const ::fwData::Mesh::Id nbCells = mesh->getNumberOfCells();
            for (size_t i = 0; i != nbCells; ++i)
            {
                meshHelper.setCellNormal(i, normals->GetPointer(i*nbComponents));
            }
        }

        if(polyData->GetPointData()->GetAttribute(vtkDataSetAttributes::TCOORDS))
        {
            vtkSmartPointer<vtkFloatArray> texCoords;
            texCoords = vtkFloatArray::SafeDownCast(polyData->GetPointData()->GetTCoords());
            FW_RAISE_IF("Only vtkFloatArray is supported to manage texCoords.", !texCoords);

            size_t nbComponents = texCoords->GetNumberOfComponents();
            SLM_ASSERT("Wrong nb of components ("<<nbComponents<<")", nbComponents == 2);

            mesh->allocatePointTexCoords();
            meshHelper.updateLock();

            const ::fwData::Mesh::Id nbPoints = mesh->getNumberOfPoints();
            for (size_t i = 0; i != nbPoints; ++i)
            {
                meshHelper.setPointTexCoord(i, texCoords->GetPointer(i*nbComponents));
            }
        }

        if(polyData->GetCellData()->GetAttribute(vtkDataSetAttributes::TCOORDS))
        {
            vtkSmartPointer<vtkFloatArray> texCoords;
            texCoords = vtkFloatArray::SafeDownCast(polyData->GetCellData()->GetTCoords());
            FW_RAISE_IF("Only vtkFloatArray is supported to manage texCoords.", !texCoords);

            size_t nbComponents = texCoords->GetNumberOfComponents();
            SLM_ASSERT("Wrong nb of components ("<<nbComponents<<")", nbComponents == 2);

            mesh->allocateCellTexCoords();
            meshHelper.updateLock();

            const ::fwData::Mesh::Id nbCells = mesh->getNumberOfCells();
            for (size_t i = 0; i != nbCells; ++i)
            {
                meshHelper.setCellTexCoord(i, texCoords->GetPointer(i*nbComponents));
            }
        }

        mesh->adjustAllocatedMemory();
    }
}

//------------------------------------------------------------------------------

void Mesh::fromVTKGrid(vtkSmartPointer<vtkUnstructuredGrid> grid, ::fwData::Mesh::sptr mesh)
{
    vtkPoints* points = grid->GetPoints();
    if(points)
    {
        mesh->clear();
        const vtkIdType numberOfPoints = points->GetNumberOfPoints();
        const vtkIdType numberOfCells  = grid->GetNumberOfCells();

        mesh->allocate(numberOfPoints, numberOfCells, numberOfCells*3);
        ::fwDataTools::helper::Mesh meshHelper(mesh);
        double* point;
        ::fwData::Mesh::Id idx;
        for (vtkIdType i = 0; i < numberOfPoints; ++i)
        {
            point = points->GetPoint(i);
            idx   = meshHelper.insertNextPoint(point[0], point[1], point[2]);
            SLM_ASSERT("Mesh index not correspond to VTK index point", idx == i);
        }

        vtkCell* cell;
        vtkIdList* idList;
        int cellType;
        for (vtkIdType i = 0; i < numberOfCells; ++i)
        {
            cell     = grid->GetCell(i);
            idList   = cell->GetPointIds();
            cellType = cell->GetCellType();

            switch (cellType)
            {
                case VTK_VERTEX:
                    SLM_ASSERT("Wrong number of ids: "<<idList->GetNumberOfIds(), idList->GetNumberOfIds() == 1);
                    meshHelper.insertNextCell( idList->GetId(0));
                    break;
                case VTK_LINE:
                    SLM_ASSERT("Wrong number of ids: "<<idList->GetNumberOfIds(), idList->GetNumberOfIds() == 2);
                    meshHelper.insertNextCell( idList->GetId(0), idList->GetId(1));
                    break;
                case VTK_TRIANGLE:
                    SLM_ASSERT("Wrong number of ids: "<<idList->GetNumberOfIds(), idList->GetNumberOfIds() == 3);
                    meshHelper.insertNextCell( idList->GetId(0), idList->GetId(1), idList->GetId(2));
                    break;
                case VTK_QUAD:
                    SLM_ASSERT("Wrong number of ids: "<<idList->GetNumberOfIds(), idList->GetNumberOfIds() == 4);
                    meshHelper.insertNextCell( idList->GetId(0), idList->GetId(1), idList->GetId(2), idList->GetId(3));
                    break;
                case VTK_TETRA:
                    SLM_ASSERT("Wrong number of ids: "<<idList->GetNumberOfIds(), idList->GetNumberOfIds() == 4);
                    meshHelper.insertNextCell( idList->GetId(0), idList->GetId(1), idList->GetId(2), idList->GetId(3),
                                               ::fwData::Mesh::TETRA);
                    break;
                default:
                    FW_RAISE("VTK Mesh type "<<cellType<< " not supported.");
            }
        }

        if(grid->GetPointData()->HasArray("Colors"))
        {
            vtkSmartPointer<vtkUnsignedCharArray> colors;
            colors = vtkUnsignedCharArray::SafeDownCast(grid->GetPointData()->GetArray("Colors"));
            FW_RAISE_IF("Only vtkUnsignedCharArray is supported to manage color.", !colors);

            size_t nbComponents = colors->GetNumberOfComponents();
            SLM_ASSERT("Wrong nb of components ("<<nbComponents<<")",
                       nbComponents == 3 || nbComponents == 4);
            mesh->allocatePointColors((::fwData::Mesh::ColorArrayTypes)nbComponents);
            meshHelper.updateLock();

            const ::fwData::Mesh::Id nbPoints = mesh->getNumberOfPoints();
            for (size_t i = 0; i != nbPoints; ++i)
            {
                meshHelper.setPointColor(i, colors->GetPointer(i*nbComponents));
            }
        }

        if(grid->GetCellData()->HasArray("Colors"))
        {
            vtkSmartPointer<vtkUnsignedCharArray> colors;
            colors = vtkUnsignedCharArray::SafeDownCast(grid->GetCellData()->GetArray("Colors"));
            FW_RAISE_IF("Only vtkUnsignedCharArray is supported to manage color.", !colors);

            size_t nbComponents = colors->GetNumberOfComponents();
            SLM_ASSERT("Wrong nb of components ("<<nbComponents<<")",
                       nbComponents == 3 || nbComponents == 4);
            mesh->allocateCellColors((::fwData::Mesh::ColorArrayTypes)nbComponents);
            meshHelper.updateLock();

            const ::fwData::Mesh::Id nbCells = mesh->getNumberOfCells();
            for (size_t i = 0; i != nbCells; ++i)
            {
                meshHelper.setCellColor(i, colors->GetPointer(i*nbComponents));
            }
        }

        if(grid->GetPointData()->GetAttribute(vtkDataSetAttributes::NORMALS))
        {
            vtkSmartPointer<vtkFloatArray> normals;
            normals = vtkFloatArray::SafeDownCast(grid->GetPointData()->GetNormals());
            FW_RAISE_IF("Only vtkFloatArray is supported to manage normals.", !normals);

            size_t nbComponents = normals->GetNumberOfComponents();
            SLM_ASSERT("Wrong nb of components ("<<nbComponents<<")", nbComponents == 3);

            mesh->allocatePointNormals();
            meshHelper.updateLock();

            const ::fwData::Mesh::Id nbPoints = mesh->getNumberOfPoints();
            for (size_t i = 0; i != nbPoints; ++i)
            {
                meshHelper.setPointNormal(i, normals->GetPointer(i*nbComponents));
            }
        }

        if(grid->GetCellData()->GetAttribute(vtkDataSetAttributes::NORMALS))
        {
            vtkSmartPointer<vtkFloatArray> normals;
            normals = vtkFloatArray::SafeDownCast(grid->GetCellData()->GetNormals());
            FW_RAISE_IF("Only vtkFloatArray is supported to manage normals.", !normals);

            size_t nbComponents = normals->GetNumberOfComponents();
            SLM_ASSERT("Wrong nb of components ("<<nbComponents<<")", nbComponents == 3);

            mesh->allocateCellNormals();
            meshHelper.updateLock();

            const ::fwData::Mesh::Id nbCells = mesh->getNumberOfCells();
            for (size_t i = 0; i != nbCells; ++i)
            {
                meshHelper.setCellNormal(i, normals->GetPointer(i*nbComponents));
            }
        }

        if(grid->GetPointData()->GetAttribute(vtkDataSetAttributes::TCOORDS))
        {
            vtkSmartPointer<vtkFloatArray> texCoords;
            texCoords = vtkFloatArray::SafeDownCast(grid->GetPointData()->GetTCoords());
            FW_RAISE_IF("Only vtkFloatArray is supported to manage texCoords.", !texCoords);

            size_t nbComponents = texCoords->GetNumberOfComponents();
            SLM_ASSERT("Wrong nb of components ("<<nbComponents<<")", nbComponents == 2);

            mesh->allocatePointTexCoords();
            meshHelper.updateLock();

            const ::fwData::Mesh::Id nbPoints = mesh->getNumberOfPoints();
            for (size_t i = 0; i != nbPoints; ++i)
            {
                meshHelper.setPointTexCoord(i, texCoords->GetPointer(i*nbComponents));
            }
        }

        if(grid->GetCellData()->GetAttribute(vtkDataSetAttributes::TCOORDS))
        {
            vtkSmartPointer<vtkFloatArray> texCoords;
            texCoords = vtkFloatArray::SafeDownCast(grid->GetCellData()->GetTCoords());
            FW_RAISE_IF("Only vtkFloatArray is supported to manage texCoords.", !texCoords);

            size_t nbComponents = texCoords->GetNumberOfComponents();
            SLM_ASSERT("Wrong nb of components ("<<nbComponents<<")", nbComponents == 2);

            mesh->allocateCellTexCoords();
            meshHelper.updateLock();

            const ::fwData::Mesh::Id nbCells = mesh->getNumberOfCells();
            for (size_t i = 0; i != nbCells; ++i)
            {
                meshHelper.setCellTexCoord(i, texCoords->GetPointer(i*nbComponents));
            }
        }

        mesh->adjustAllocatedMemory();

    }

}

//------------------------------------------------------------------------------

void Mesh::toVTKMesh( const ::fwData::Mesh::csptr& mesh, vtkSmartPointer<vtkPolyData> polyData)
{
    const vtkSmartPointer< vtkPoints > pts = vtkSmartPointer< vtkPoints >::New();
    polyData->SetPoints(pts);
    Mesh::updatePolyDataPoints(polyData, mesh);

    ::fwDataTools::helper::MeshGetter meshHelper(mesh);
    const unsigned int nbCells = mesh->getNumberOfCells();

    const ::fwData::Mesh::ConstCellTypesMultiArrayType cellTypes             = meshHelper.getCellTypes();
    const ::fwData::Mesh::ConstCellDataMultiArrayType cellData               = meshHelper.getCellData();
    const ::fwData::Mesh::ConstCellDataOffsetsMultiArrayType cellDataOffsets = meshHelper.getCellDataOffsets();

    polyData->Allocate(static_cast< int >(nbCells));

    vtkIdType typeVtkCell;
    vtkIdType cell[4];
    for(unsigned int i = 0; i < nbCells; ++i )
    {
        const ::fwData::Mesh::CellTypes cellType = cellTypes[i];
        const ::fwData::Mesh::Id offset          = cellDataOffsets[i];
        switch( cellType )
        {
            case ::fwData::Mesh::POINT:
                typeVtkCell = VTK_VERTEX;
                cell[0]     = cellData[offset];
                polyData->InsertNextCell( typeVtkCell, 1, cell );
                break;
            case ::fwData::Mesh::EDGE:
                typeVtkCell = VTK_LINE;
                cell[0]     = cellData[offset];
                cell[1]     = cellData[offset+1];
                polyData->InsertNextCell( typeVtkCell, 2, cell );
                break;
            case ::fwData::Mesh::TRIANGLE:
                typeVtkCell = VTK_TRIANGLE;
                cell[0]     = cellData[offset];
                cell[1]     = cellData[offset+1];
                cell[2]     = cellData[offset+2];
                polyData->InsertNextCell( typeVtkCell, 3, cell );
                break;
            case ::fwData::Mesh::QUAD:
                typeVtkCell = VTK_QUAD;
                cell[0]     = cellData[offset];
                cell[1]     = cellData[offset+1];
                cell[2]     = cellData[offset+2];
                cell[3]     = cellData[offset+3];
                polyData->InsertNextCell( typeVtkCell, 4, cell );
                break;
            case ::fwData::Mesh::TETRA:
                typeVtkCell = VTK_LINE;

                cell[0] = cellData[offset+1];
                cell[1] = cellData[offset+2];
                polyData->InsertNextCell( typeVtkCell, 2, cell );

                cell[0] = cellData[offset+2];
                cell[1] = cellData[offset+3];
                polyData->InsertNextCell( typeVtkCell, 2, cell );

                cell[0] = cellData[offset+3];
                cell[1] = cellData[offset];
                polyData->InsertNextCell( typeVtkCell, 2, cell );

                cell[0] = cellData[offset+2];
                cell[1] = cellData[offset];
                polyData->InsertNextCell( typeVtkCell, 2, cell );

                cell[0] = cellData[offset+1];
                cell[1] = cellData[offset+3];
                polyData->InsertNextCell( typeVtkCell, 2, cell );

        }
    }

    Mesh::updatePolyDataPointNormals(polyData, mesh);
    Mesh::updatePolyDataCellNormals(polyData, mesh);

    Mesh::updatePolyDataPointColor(polyData, mesh);
    Mesh::updatePolyDataCellColor(polyData, mesh);

    Mesh::updatePolyDataPointTexCoords(polyData, mesh);
    Mesh::updatePolyDataCellTexCoords(polyData, mesh);
}

//------------------------------------------------------------------------------

void Mesh::toVTKGrid( const ::fwData::Mesh::csptr& mesh, vtkSmartPointer<vtkUnstructuredGrid> grid)
{
    const vtkSmartPointer< vtkPoints > pts = vtkSmartPointer< vtkPoints >::New();
    grid->SetPoints(pts);
    Mesh::updateGridPoints(grid, mesh);

    ::fwDataTools::helper::MeshGetter meshHelper(mesh);
    const unsigned int nbCells = mesh->getNumberOfCells();

    const ::fwData::Mesh::ConstCellTypesMultiArrayType cellTypes             = meshHelper.getCellTypes();
    const ::fwData::Mesh::ConstCellDataMultiArrayType cellData               = meshHelper.getCellData();
    const ::fwData::Mesh::ConstCellDataOffsetsMultiArrayType cellDataOffsets = meshHelper.getCellDataOffsets();

    grid->Allocate(static_cast<int>(nbCells));

    vtkIdType typeVtkCell;
    vtkIdType cell[4];
    for(unsigned int i = 0; i < nbCells; ++i )
    {
        const ::fwData::Mesh::CellTypes cellType = cellTypes[i];
        const ::fwData::Mesh::Id offset          = cellDataOffsets[i];
        switch( cellType )
        {
            case ::fwData::Mesh::POINT:
                typeVtkCell = VTK_VERTEX;
                cell[0]     = cellData[offset];
                grid->InsertNextCell( typeVtkCell, 1, cell );
                break;
            case ::fwData::Mesh::EDGE:
                typeVtkCell = VTK_LINE;
                cell[0]     = cellData[offset];
                cell[1]     = cellData[offset+1];
                grid->InsertNextCell( typeVtkCell, 2, cell );
                break;
            case ::fwData::Mesh::TRIANGLE:
                typeVtkCell = VTK_TRIANGLE;
                cell[0]     = cellData[offset];
                cell[1]     = cellData[offset+1];
                cell[2]     = cellData[offset+2];
                grid->InsertNextCell( typeVtkCell, 3, cell );
                break;
            case ::fwData::Mesh::QUAD:
                typeVtkCell = VTK_QUAD;
                cell[0]     = cellData[offset];
                cell[1]     = cellData[offset+1];
                cell[2]     = cellData[offset+2];
                cell[3]     = cellData[offset+3];
                grid->InsertNextCell( typeVtkCell, 4, cell );
                break;
            case ::fwData::Mesh::TETRA:
                typeVtkCell = VTK_TETRA;
                cell[0]     = cellData[offset];
                cell[1]     = cellData[offset+1];
                cell[2]     = cellData[offset+2];
                cell[3]     = cellData[offset+3];
                grid->InsertNextCell( typeVtkCell, 4, cell );
        }
    }

    Mesh::updateGridPointNormals(grid, mesh);
    Mesh::updateGridCellNormals(grid, mesh);

    Mesh::updateGridPointColor(grid, mesh);
    Mesh::updateGridCellColor(grid, mesh);

    Mesh::updateGridPointTexCoords(grid, mesh);
    Mesh::updateGridCellTexCoords(grid, mesh);
}

//------------------------------------------------------------------------------

void Mesh::updatePolyDataPoints(vtkSmartPointer<vtkPolyData> polyDataDst,
                                const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);
    ::fwDataTools::helper::ArrayGetter arrayHelper(meshSrc->getPointsArray());

    vtkPoints* polyDataPoints                    = polyDataDst->GetPoints();
    const vtkIdType nbPoints                     = meshSrc->getNumberOfPoints();
    const ::fwData::Mesh::PointValueType* points = arrayHelper.begin< ::fwData::Mesh::PointValueType >();

    if (nbPoints != polyDataPoints->GetNumberOfPoints())
    {
        polyDataPoints->SetNumberOfPoints(nbPoints);
    }

    for (vtkIdType i = 0; i != nbPoints; ++i)
    {
        polyDataPoints->SetPoint(i, points[i*3], points[i*3+1], points[i*3+2]);
    }

    polyDataPoints->Modified();
}

//------------------------------------------------------------------------------

void Mesh::updatePolyDataPointColor(vtkSmartPointer<vtkPolyData> polyDataDst,
                                    const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    const ::fwData::Array::csptr pointColorArray = meshSrc->getPointColorsArray();
    if(pointColorArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(pointColorArray);

        const vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        const size_t nbComponents                          = pointColorArray->getNumberOfComponents();
        colors->SetNumberOfComponents(static_cast<int>(nbComponents));
        colors->SetName("Colors");

        const unsigned char* pointColor          = arrayHelper.begin< unsigned char >();
        const unsigned char* const pointColorEnd = arrayHelper.end< unsigned char >();

        const vtkIdType size = pointColorArray->getSize()[0] * nbComponents;

        unsigned char* newColors = new unsigned char[size];
        std::copy(pointColor, pointColorEnd, newColors);
        colors->SetArray(newColors, size, 0, vtkUnsignedCharArray::VTK_DATA_ARRAY_DELETE);

        polyDataDst->GetPointData()->SetScalars(colors);
        polyDataDst->Modified();
    }
    else
    {
        if(polyDataDst->GetPointData()->HasArray("Colors"))
        {
            polyDataDst->GetPointData()->RemoveArray("Colors");
        }
        polyDataDst->Modified();
    }
}

//------------------------------------------------------------------------------

void Mesh::updatePolyDataCellColor(vtkSmartPointer<vtkPolyData> polyDataDst,
                                   const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    const ::fwData::Array::csptr cellColorArray = meshSrc->getCellColorsArray();
    if(cellColorArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(cellColorArray);

        const vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        const size_t nbComponents                          = cellColorArray->getNumberOfComponents();
        colors->SetNumberOfComponents(static_cast<int>(nbComponents));
        colors->SetName("Colors");

        ::fwMemory::BufferObject::Lock lock = cellColorArray->getBufferObject()->lock();
        const unsigned char* cellColor          = arrayHelper.begin< unsigned char >();
        const unsigned char* const cellColorEnd = arrayHelper.end< unsigned char >();

        const vtkIdType size = cellColorArray->getSize()[0] * nbComponents;

        unsigned char* newColors = new unsigned char[size];
        std::copy(cellColor, cellColorEnd, newColors);
        colors->SetArray(newColors, size, 0, vtkUnsignedCharArray::VTK_DATA_ARRAY_DELETE);

        polyDataDst->GetCellData()->SetScalars(colors);
        polyDataDst->Modified();
    }
    else
    {
        if(polyDataDst->GetCellData()->HasArray("Colors"))
        {
            polyDataDst->GetCellData()->RemoveArray("Colors");
        }
        polyDataDst->Modified();
    }

}

//------------------------------------------------------------------------------

void Mesh::updatePolyDataPointNormals(vtkSmartPointer<vtkPolyData> polyDataDst,
                                      const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    const ::fwData::Array::csptr pointNormalsArray = meshSrc->getPointNormalsArray();
    if(pointNormalsArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(pointNormalsArray);

        vtkSmartPointer<vtkFloatArray> normals = vtkSmartPointer<vtkFloatArray>::New();
        const size_t nbComponents              = pointNormalsArray->getNumberOfComponents();
        normals->SetNumberOfComponents(static_cast<int>(nbComponents));

        const float* pointNormal          = arrayHelper.begin< float >();
        const float* const pointNormalEnd = arrayHelper.end< float >();

        const vtkIdType size = pointNormalsArray->getSize()[0] * nbComponents;

        float* newNormals = new float[size];
        std::copy(pointNormal, pointNormalEnd, newNormals);
        normals->SetArray(newNormals, size, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        polyDataDst->GetPointData()->SetNormals(normals);
        polyDataDst->Modified();
    }
    else
    {
        if(polyDataDst->GetPointData()->GetAttribute(vtkDataSetAttributes::NORMALS))
        {
            polyDataDst->GetPointData()->RemoveArray(vtkDataSetAttributes::NORMALS);
        }
        polyDataDst->Modified();
    }
}

//------------------------------------------------------------------------------

void Mesh::updatePolyDataCellNormals(vtkSmartPointer<vtkPolyData> polyDataDst,
                                     const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    const ::fwData::Array::csptr cellNormalsArray = meshSrc->getCellNormalsArray();

    if(cellNormalsArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(cellNormalsArray);

        vtkSmartPointer<vtkFloatArray> normals = vtkSmartPointer<vtkFloatArray>::New();
        const size_t nbComponents              = cellNormalsArray->getNumberOfComponents();
        normals->SetNumberOfComponents(static_cast<int>(nbComponents));

        const float* cellNormal          = arrayHelper.begin< float >();
        const float* const cellNormalEnd = arrayHelper.end< float >();

        const vtkIdType size = cellNormalsArray->getSize()[0] * nbComponents;

        float* newNormals = new float[size];
        std::copy(cellNormal, cellNormalEnd, newNormals);
        normals->SetArray(newNormals, size, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        polyDataDst->GetCellData()->SetNormals(normals);
        polyDataDst->Modified();
    }
    else
    {
        if(polyDataDst->GetCellData()->GetAttribute(vtkDataSetAttributes::NORMALS))
        {
            polyDataDst->GetCellData()->RemoveArray(vtkDataSetAttributes::NORMALS);
        }
        polyDataDst->Modified();
    }
}

//------------------------------------------------------------------------------

void Mesh::updatePolyDataPointTexCoords(vtkSmartPointer<vtkPolyData> polyDataDst,
                                        const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    const ::fwData::Array::csptr pointTexCoordsArray = meshSrc->getPointTexCoordsArray();
    if(pointTexCoordsArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(pointTexCoordsArray);

        vtkSmartPointer<vtkFloatArray> texCoords = vtkSmartPointer<vtkFloatArray>::New();
        const size_t nbComponents                = pointTexCoordsArray->getNumberOfComponents();
        texCoords->SetNumberOfComponents(static_cast<int>(nbComponents));

        const float* pointTexCoord          = arrayHelper.begin< float >();
        const float* const pointTexCoordEnd = arrayHelper.end< float >();

        const vtkIdType size = pointTexCoordsArray->getSize()[0] * nbComponents;

        float* newTexCoords = new float[size];
        std::copy(pointTexCoord, pointTexCoordEnd, newTexCoords);
        texCoords->SetArray(newTexCoords, size, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        polyDataDst->GetPointData()->SetTCoords(texCoords);
        polyDataDst->Modified();
    }
    else
    {
        if(polyDataDst->GetPointData()->GetAttribute(vtkDataSetAttributes::TCOORDS))
        {
            polyDataDst->GetPointData()->RemoveArray(vtkDataSetAttributes::TCOORDS);
        }
        polyDataDst->Modified();
    }
}

//------------------------------------------------------------------------------

void Mesh::updatePolyDataCellTexCoords(vtkSmartPointer<vtkPolyData> polyDataDst,
                                       const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    const ::fwData::Array::csptr cellTexCoordsArray = meshSrc->getCellTexCoordsArray();
    if(cellTexCoordsArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(cellTexCoordsArray);

        vtkSmartPointer<vtkFloatArray> texCoords = vtkSmartPointer<vtkFloatArray>::New();
        const size_t nbComponents                = cellTexCoordsArray->getNumberOfComponents();
        texCoords->SetNumberOfComponents(static_cast<int>(nbComponents));

        const float* cellTexCoord          = arrayHelper.begin< float >();
        const float* const cellTexCoordEnd = arrayHelper.end< float >();

        const vtkIdType size = cellTexCoordsArray->getSize()[0] * nbComponents;

        float* newTexCoords = new float[size];
        std::copy(cellTexCoord, cellTexCoordEnd, newTexCoords);
        texCoords->SetArray(newTexCoords, size, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        polyDataDst->GetCellData()->SetTCoords(texCoords);
        polyDataDst->Modified();
    }
    else
    {
        if(polyDataDst->GetCellData()->GetAttribute(vtkDataSetAttributes::TCOORDS))
        {
            polyDataDst->GetCellData()->RemoveArray(vtkDataSetAttributes::TCOORDS);
        }
        polyDataDst->Modified();
    }

}

//-----------------------------------------------------------------------------

double Mesh::computeVolume( const ::fwData::Mesh::csptr& mesh )
{
    vtkSmartPointer< vtkPolyData > vtkMeshRaw = vtkSmartPointer< vtkPolyData >::New();
    Mesh::toVTKMesh( mesh, vtkMeshRaw );

    // identify and fill holes in meshes
    vtkSmartPointer< vtkFillHolesFilter > holesFilter = vtkSmartPointer< vtkFillHolesFilter >::New();
    holesFilter->SetHoleSize(2000);
    holesFilter->SetInputData(vtkMeshRaw);
    holesFilter->Update();
    if (holesFilter->GetOutput()->GetNumberOfCells() > 0) // Filter return empty mesh when no topological holes are
                                                          // present
    {
        vtkMeshRaw = holesFilter->GetOutput();
    }

    // compute normals for polygonal mesh
    const vtkSmartPointer< vtkPolyDataNormals > filter = vtkSmartPointer< vtkPolyDataNormals >::New();
    filter->SetInputData(vtkMeshRaw);
    filter->AutoOrientNormalsOn();
    filter->FlipNormalsOff();

    // estimate volume, area, shape index of triangle mesh
    const vtkSmartPointer< vtkMassProperties > calculator = vtkSmartPointer< vtkMassProperties >::New();
    calculator->SetInputConnection( filter->GetOutputPort() );
    calculator->Update();

    const double volume = calculator->GetVolume();
    OSLM_DEBUG(
        "GetVolume : " << volume << " vtkMassProperties::GetVolumeProjected = " <<
            calculator->GetVolumeProjected() );
    OSLM_DEBUG("Error : " << (calculator->GetVolume()- fabs(calculator->GetVolumeProjected()))*10000);
    if ( (calculator->GetVolume()- fabs(calculator->GetVolumeProjected()))*10000 > calculator->GetVolume() )
    {
        std::stringstream ss;
        ss << "volume - |volume projected| > volume/10000.0" << std::endl;
        ss << "[volume = " << volume << "]" << std::endl;
        ss << "[volume projected = " << calculator->GetVolumeProjected()<<"]";
        throw (std::out_of_range( ss.str() ));
    }

    return volume;
}

//------------------------------------------------------------------------------

void Mesh::updateGridPointNormals(vtkSmartPointer<vtkUnstructuredGrid> gridDst,
                                  const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkUnstructuredGrid should not be NULL", gridDst);

    const ::fwData::Array::csptr pointNormalsArray = meshSrc->getPointNormalsArray();
    if(pointNormalsArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(pointNormalsArray);

        vtkSmartPointer<vtkFloatArray> normals = vtkSmartPointer<vtkFloatArray>::New();
        const size_t nbComponents              = pointNormalsArray->getNumberOfComponents();
        normals->SetNumberOfComponents(static_cast<int>(nbComponents));

        const float* pointNormal          = arrayHelper.begin< float >();
        const float* const pointNormalEnd = arrayHelper.end< float >();

        const vtkIdType size = pointNormalsArray->getSize()[0] * nbComponents;

        float* newNormals = new float[size];
        std::copy(pointNormal, pointNormalEnd, newNormals);
        normals->SetArray(newNormals, size, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        gridDst->GetPointData()->SetNormals(normals);
        gridDst->Modified();
    }
    else
    {
        if(gridDst->GetPointData()->GetAttribute(vtkDataSetAttributes::NORMALS))
        {
            gridDst->GetPointData()->RemoveArray(vtkDataSetAttributes::NORMALS);
        }
        gridDst->Modified();
    }

}

//------------------------------------------------------------------------------

void Mesh::updateGridPoints(vtkSmartPointer<vtkUnstructuredGrid> gridDst,
                            const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", gridDst);
    ::fwDataTools::helper::ArrayGetter arrayHelper(meshSrc->getPointsArray());

    vtkPoints* polyDataPoints                    = gridDst->GetPoints();
    const vtkIdType nbPoints                     = meshSrc->getNumberOfPoints();
    const ::fwData::Mesh::PointValueType* points = arrayHelper.begin< ::fwData::Mesh::PointValueType >();

    if (nbPoints != polyDataPoints->GetNumberOfPoints())
    {
        polyDataPoints->SetNumberOfPoints(nbPoints);
    }

    for (vtkIdType i = 0; i != nbPoints; ++i)
    {
        polyDataPoints->SetPoint(i, points[i*3], points[i*3+1], points[i*3+2]);
    }

    polyDataPoints->Modified();
}

//------------------------------------------------------------------------------
void Mesh::updateGridPointColor(vtkSmartPointer<vtkUnstructuredGrid> gridDst,
                                const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", gridDst);

    const ::fwData::Array::csptr pointColorArray = meshSrc->getPointColorsArray();
    if(pointColorArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(pointColorArray);

        vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        const size_t nbComponents                    = pointColorArray->getNumberOfComponents();
        colors->SetNumberOfComponents(static_cast<int>(nbComponents));
        colors->SetName("Colors");

        const unsigned char* pointColor          = arrayHelper.begin< unsigned char >();
        const unsigned char* const pointColorEnd = arrayHelper.end< unsigned char >();

        const vtkIdType size = pointColorArray->getSize()[0] * nbComponents;

        unsigned char* newColors = new unsigned char[size];
        std::copy(pointColor, pointColorEnd, newColors);
        colors->SetArray(newColors, size, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        gridDst->GetPointData()->SetScalars(colors);
        gridDst->Modified();
    }
    else
    {
        if(gridDst->GetPointData()->HasArray("Colors"))
        {
            gridDst->GetPointData()->RemoveArray("Colors");
        }
        gridDst->Modified();
    }

}

//------------------------------------------------------------------------------

void Mesh::updateGridCellColor(vtkSmartPointer<vtkUnstructuredGrid> gridDst,
                               const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", gridDst);

    const ::fwData::Array::csptr cellColorArray = meshSrc->getCellColorsArray();
    if(cellColorArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(cellColorArray);

        vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        const size_t nbComponents                    = cellColorArray->getNumberOfComponents();
        colors->SetNumberOfComponents(static_cast<int>(nbComponents));
        colors->SetName("Colors");

        const ::fwMemory::BufferObject::Lock lock = cellColorArray->getBufferObject()->lock();
        const unsigned char* cellColor            = arrayHelper.begin< unsigned char >();
        const unsigned char* const cellColorEnd   = arrayHelper.end< unsigned char >();

        const vtkIdType size = cellColorArray->getSize()[0] * nbComponents;

        unsigned char* newColors = new unsigned char[size];
        std::copy(cellColor, cellColorEnd, newColors);
        colors->SetArray(newColors, size, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        gridDst->GetCellData()->SetScalars(colors);
        gridDst->Modified();
    }
    else
    {
        if(gridDst->GetCellData()->HasArray("Colors"))
        {
            gridDst->GetCellData()->RemoveArray("Colors");
        }
        gridDst->Modified();
    }
}

//------------------------------------------------------------------------------

void Mesh::updateGridCellNormals(vtkSmartPointer<vtkUnstructuredGrid> gridDst,
                                 const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", gridDst);

    const ::fwData::Array::csptr cellNormalsArray = meshSrc->getCellNormalsArray();

    if(cellNormalsArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(cellNormalsArray);

        vtkSmartPointer<vtkFloatArray> normals = vtkSmartPointer<vtkFloatArray>::New();
        const size_t nbComponents              = cellNormalsArray->getNumberOfComponents();
        normals->SetNumberOfComponents(static_cast<int>(nbComponents));

        const float* cellNormal          = arrayHelper.begin< float >();
        const float* const cellNormalEnd = arrayHelper.end< float >();

        const vtkIdType size = cellNormalsArray->getSize()[0] * nbComponents;

        float* newNormals = new float[size];
        std::copy(cellNormal, cellNormalEnd, newNormals);
        normals->SetArray(newNormals, size, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        gridDst->GetCellData()->SetNormals(normals);
        gridDst->Modified();
    }
    else
    {
        if(gridDst->GetCellData()->GetAttribute(vtkDataSetAttributes::NORMALS))
        {
            gridDst->GetCellData()->RemoveArray(vtkDataSetAttributes::NORMALS);
        }
        gridDst->Modified();
    }

}

//------------------------------------------------------------------------------

void Mesh::updateGridPointTexCoords(vtkSmartPointer<vtkUnstructuredGrid> gridDst,
                                    const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", gridDst);

    const ::fwData::Array::csptr pointTexCoordsArray = meshSrc->getPointTexCoordsArray();
    if(pointTexCoordsArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(pointTexCoordsArray);

        vtkSmartPointer<vtkFloatArray> texCoords = vtkSmartPointer<vtkFloatArray>::New();
        const size_t nbComponents                = pointTexCoordsArray->getNumberOfComponents();
        texCoords->SetNumberOfComponents(static_cast<int>(nbComponents));

        const float* pointTexCoord          = arrayHelper.begin< float >();
        const float* const pointTexCoordEnd = arrayHelper.end< float >();

        const vtkIdType size = pointTexCoordsArray->getSize()[0] * nbComponents;

        float* newTexCoords = new float[size];
        std::copy(pointTexCoord, pointTexCoordEnd, newTexCoords);
        texCoords->SetArray(newTexCoords, size, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        gridDst->GetPointData()->SetTCoords(texCoords);
        gridDst->Modified();
    }
    else
    {
        if(gridDst->GetPointData()->GetAttribute(vtkDataSetAttributes::TCOORDS))
        {
            gridDst->GetPointData()->RemoveArray(vtkDataSetAttributes::TCOORDS);
        }
        gridDst->Modified();
    }

}

//------------------------------------------------------------------------------

void Mesh::updateGridCellTexCoords(vtkSmartPointer<vtkUnstructuredGrid> gridDst,
                                   const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", gridDst);

    const ::fwData::Array::csptr cellTexCoordsArray = meshSrc->getCellTexCoordsArray();

    if(cellTexCoordsArray)
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(cellTexCoordsArray);

        vtkSmartPointer<vtkFloatArray> texCoords = vtkSmartPointer<vtkFloatArray>::New();
        size_t nbComponents                      = cellTexCoordsArray->getNumberOfComponents();
        texCoords->SetNumberOfComponents(static_cast<int>(nbComponents));

        const float* cellTexCoord          = arrayHelper.begin< float >();
        const float* const cellTexCoordEnd = arrayHelper.end< float >();

        const vtkIdType size = cellTexCoordsArray->getSize()[0] * nbComponents;

        float* newTexCoords = new float[size];
        std::copy(cellTexCoord, cellTexCoordEnd, newTexCoords);
        texCoords->SetArray(newTexCoords, size, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        gridDst->GetCellData()->SetTCoords(texCoords);
        gridDst->Modified();
    }
    else
    {
        if(gridDst->GetCellData()->GetAttribute(vtkDataSetAttributes::TCOORDS))
        {
            gridDst->GetCellData()->RemoveArray(vtkDataSetAttributes::TCOORDS);
        }
        gridDst->Modified();
    }

}

//-----------------------------------------------------------------------------

} // namespace helper
} // namespace fwVtkIO
