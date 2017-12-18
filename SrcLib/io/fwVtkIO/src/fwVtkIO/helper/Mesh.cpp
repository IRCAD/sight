/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwVtkIO/helper/Mesh.hpp"

#include <fwData/Array.hpp>

#include <fwDataTools/helper/Array.hpp>
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
        vtkIdType numberOfPoints = points->GetNumberOfPoints();
        vtkIdType numberOfCells  = polyData->GetNumberOfCells();

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

            size_t nbComponents = colors->GetNumberOfComponents();
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

            ::fwData::Mesh::Id nbCells = mesh->getNumberOfCells();
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

            ::fwData::Mesh::Id nbPoints = mesh->getNumberOfPoints();
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

            ::fwData::Mesh::Id nbCells = mesh->getNumberOfCells();
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

            ::fwData::Mesh::Id nbPoints = mesh->getNumberOfPoints();
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

            ::fwData::Mesh::Id nbCells = mesh->getNumberOfCells();
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
        vtkIdType numberOfPoints = points->GetNumberOfPoints();
        vtkIdType numberOfCells  = grid->GetNumberOfCells();

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
                    meshHelper.insertNextCell( idList->GetId(0), idList->GetId(1), idList->GetId(2), idList->GetId(3));
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

            ::fwData::Mesh::Id nbPoints = mesh->getNumberOfPoints();
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

            ::fwData::Mesh::Id nbCells = mesh->getNumberOfCells();
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

            ::fwData::Mesh::Id nbPoints = mesh->getNumberOfPoints();
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

            ::fwData::Mesh::Id nbCells = mesh->getNumberOfCells();
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

            ::fwData::Mesh::Id nbPoints = mesh->getNumberOfPoints();
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

            ::fwData::Mesh::Id nbCells = mesh->getNumberOfCells();
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
    vtkSmartPointer< vtkPoints > pts = vtkSmartPointer< vtkPoints >::New();
    polyData->SetPoints(pts);
    Mesh::updatePolyDataPoints(polyData, mesh);

    ::fwDataTools::helper::MeshGetter meshHelper(mesh);
    unsigned int nbCells = mesh->getNumberOfCells();

    ::fwData::Mesh::ConstCellTypesMultiArrayType cellTypes             = meshHelper.getCellTypes();
    ::fwData::Mesh::ConstCellDataMultiArrayType cellData               = meshHelper.getCellData();
    ::fwData::Mesh::ConstCellDataOffsetsMultiArrayType cellDataOffsets = meshHelper.getCellDataOffsets();

    polyData->Allocate(4, nbCells);

    vtkIdType typeVtkCell;
    vtkIdType cell[4];
    for(unsigned int i = 0; i < nbCells; ++i )
    {
        ::fwData::Mesh::CellTypes cellType = cellTypes[i];
        ::fwData::Mesh::Id offset          = cellDataOffsets[i];
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
                cell[0]     = cellData[offset];
                cell[1]     = cellData[offset+1];
                polyData->InsertNextCell( typeVtkCell, 2, cell );

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

vtkSmartPointer<vtkPolyData> Mesh::updatePolyDataPoints(vtkSmartPointer<vtkPolyData> polyDataDst,
                                                        const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);
    ::fwDataTools::helper::MeshGetter meshHelper(meshSrc);

    vtkPoints* polyDataPoints = polyDataDst->GetPoints();
    ::fwData::Mesh::Id nbPoints                      = meshSrc->getNumberOfPoints();
    ::fwData::Mesh::ConstPointsMultiArrayType points = meshHelper.getPoints();

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

vtkSmartPointer<vtkPolyData> Mesh::updatePolyDataPointColor(vtkSmartPointer<vtkPolyData> polyDataDst,
                                                            const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    ::fwData::Array::sptr pointColorArray = meshSrc->getPointColorsArray();
    if(pointColorArray)
    {
        ::fwDataTools::helper::Array arrayHelper(pointColorArray);

        vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        size_t nbComponents                          = pointColorArray->getNumberOfComponents();
        colors->SetNumberOfComponents(nbComponents);
        colors->SetName("Colors");

        unsigned char* pointColor    = arrayHelper.begin< unsigned char >();
        unsigned char* pointColorEnd = arrayHelper.end< unsigned char >();

        for (; pointColor != pointColorEnd; pointColor += nbComponents)
        {
            // Since VTK 7.1, InsertNextTupleValue is deprecated in favor of InsertNextTypedTuple.
#if (VTK_MAJOR_VERSION == 7 && VTK_MINOR_VERSION >= 1) ||  VTK_MAJOR_VERSION > 7
            colors->InsertNextTypedTuple(pointColor);
#else
            colors->InsertNextTupleValue(pointColor);
#endif
        }
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

    return polyDataDst;
}

//------------------------------------------------------------------------------

vtkSmartPointer<vtkPolyData> Mesh::updatePolyDataCellColor(vtkSmartPointer<vtkPolyData> polyDataDst,
                                                           const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    ::fwData::Array::sptr cellColorArray = meshSrc->getCellColorsArray();
    if(cellColorArray)
    {
        ::fwDataTools::helper::Array arrayHelper(cellColorArray);

        vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        size_t nbComponents                          = cellColorArray->getNumberOfComponents();
        colors->SetNumberOfComponents(nbComponents);
        colors->SetName("Colors");

        ::fwMemory::BufferObject::Lock lock = cellColorArray->getBufferObject()->lock();
        unsigned char* cellColor    = arrayHelper.begin< unsigned char >();
        unsigned char* cellColorEnd = arrayHelper.end< unsigned char >();

        for (; cellColor != cellColorEnd; cellColor += nbComponents)
        {
            // Since VTK 7.1, InsertNextTupleValue is deprecated in favor of InsertNextTypedTuple.
#if (VTK_MAJOR_VERSION == 7 && VTK_MINOR_VERSION >= 1) ||  VTK_MAJOR_VERSION > 7
            colors->InsertNextTypedTuple(cellColor);
#else
            colors->InsertNextTupleValue(cellColor);
#endif
        }

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

    return polyDataDst;
}

//------------------------------------------------------------------------------

vtkSmartPointer<vtkPolyData> Mesh::updatePolyDataPointNormals(vtkSmartPointer<vtkPolyData> polyDataDst,
                                                              const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    ::fwData::Array::sptr pointNormalsArray = meshSrc->getPointNormalsArray();
    if(pointNormalsArray)
    {
        ::fwDataTools::helper::Array arrayHelper(pointNormalsArray);

        vtkSmartPointer<vtkFloatArray> normals = vtkSmartPointer<vtkFloatArray>::New();
        size_t nbComponents                    = pointNormalsArray->getNumberOfComponents();
        normals->SetNumberOfComponents(nbComponents);

        float* pointNormal    = arrayHelper.begin< float >();
        float* pointNormalEnd = arrayHelper.end< float >();

        for (; pointNormal != pointNormalEnd; pointNormal += nbComponents)
        {
            // Since VTK 7.1, InsertNextTupleValue is deprecated in favor of InsertNextTypedTuple.
#if (VTK_MAJOR_VERSION == 7 && VTK_MINOR_VERSION >= 1) ||  VTK_MAJOR_VERSION > 7
            normals->InsertNextTypedTuple(pointNormal);
#else
            normals->InsertNextTupleValue(pointNormal);
#endif
        }

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

    return polyDataDst;
}

//------------------------------------------------------------------------------

vtkSmartPointer<vtkPolyData> Mesh::updatePolyDataCellNormals(vtkSmartPointer<vtkPolyData> polyDataDst,
                                                             const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    ::fwData::Array::sptr cellNormalsArray = meshSrc->getCellNormalsArray();

    if(cellNormalsArray)
    {
        ::fwDataTools::helper::Array arrayHelper(cellNormalsArray);

        vtkSmartPointer<vtkFloatArray> normals = vtkSmartPointer<vtkFloatArray>::New();
        size_t nbComponents                    = cellNormalsArray->getNumberOfComponents();
        normals->SetNumberOfComponents(nbComponents);

        float* cellNormal    = arrayHelper.begin< float >();
        float* cellNormalEnd = arrayHelper.end< float >();

        for (; cellNormal != cellNormalEnd; cellNormal += nbComponents)
        {
            // Since VTK 7.1, InsertNextTupleValue is deprecated in favor of InsertNextTypedTuple.
#if (VTK_MAJOR_VERSION == 7 && VTK_MINOR_VERSION >= 1) ||  VTK_MAJOR_VERSION > 7
            normals->InsertNextTypedTuple(cellNormal);
#else
            normals->InsertNextTupleValue(cellNormal);
#endif
        }

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

    return polyDataDst;
}

//------------------------------------------------------------------------------

vtkSmartPointer<vtkPolyData> Mesh::updatePolyDataPointTexCoords(vtkSmartPointer<vtkPolyData> polyDataDst,
                                                                const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    ::fwData::Array::sptr pointTexCoordsArray = meshSrc->getPointTexCoordsArray();
    if(pointTexCoordsArray)
    {
        ::fwDataTools::helper::Array arrayHelper(pointTexCoordsArray);

        vtkSmartPointer<vtkFloatArray> normals = vtkSmartPointer<vtkFloatArray>::New();
        size_t nbComponents                    = pointTexCoordsArray->getNumberOfComponents();
        normals->SetNumberOfComponents(nbComponents);

        float* pointTexCoord    = arrayHelper.begin< float >();
        float* pointTexCoordEnd = arrayHelper.end< float >();

        for (; pointTexCoord != pointTexCoordEnd; pointTexCoord += nbComponents)
        {
            // Since VTK 7.1, InsertNextTupleValue is deprecated in favor of InsertNextTypedTuple.
#if (VTK_MAJOR_VERSION == 7 && VTK_MINOR_VERSION >= 1) ||  VTK_MAJOR_VERSION > 7
            normals->InsertNextTypedTuple(pointTexCoord);
#else
            normals->InsertNextTupleValue(pointTexCoord);
#endif
        }

        polyDataDst->GetPointData()->SetTCoords(normals);
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

    return polyDataDst;
}

//------------------------------------------------------------------------------

vtkSmartPointer<vtkPolyData> Mesh::updatePolyDataCellTexCoords(vtkSmartPointer<vtkPolyData> polyDataDst,
                                                               const ::fwData::Mesh::csptr& meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    ::fwData::Array::sptr cellTexCoordsArray = meshSrc->getCellTexCoordsArray();

    if(cellTexCoordsArray)
    {
        ::fwDataTools::helper::Array arrayHelper(cellTexCoordsArray);

        vtkSmartPointer<vtkFloatArray> normals = vtkSmartPointer<vtkFloatArray>::New();
        size_t nbComponents                    = cellTexCoordsArray->getNumberOfComponents();
        normals->SetNumberOfComponents(nbComponents);

        float* cellTexCoord    = arrayHelper.begin< float >();
        float* cellTexCoordEnd = arrayHelper.end< float >();

        for (; cellTexCoord != cellTexCoordEnd; cellTexCoord += nbComponents)
        {
            // Since VTK 7.1, InsertNextTupleValue is deprecated in favor of InsertNextTypedTuple.
#if (VTK_MAJOR_VERSION == 7 && VTK_MINOR_VERSION >= 1) ||  VTK_MAJOR_VERSION > 7
            normals->InsertNextTypedTuple(cellTexCoord);
#else
            normals->InsertNextTupleValue(cellTexCoord);
#endif
        }

        polyDataDst->GetCellData()->SetTCoords(normals);
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

    return polyDataDst;
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
    vtkSmartPointer< vtkPolyDataNormals > filter = vtkSmartPointer< vtkPolyDataNormals >::New();
    filter->SetInputData(vtkMeshRaw);
    filter->AutoOrientNormalsOn();
    filter->FlipNormalsOff();

    // estimate volume, area, shape index of triangle mesh
    vtkSmartPointer< vtkMassProperties > calculator = vtkSmartPointer< vtkMassProperties >::New();
    calculator->SetInputConnection( filter->GetOutputPort() );
    calculator->Update();

    double volume = calculator->GetVolume();
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

} // namespace helper
} // namespace fwVtkIO
