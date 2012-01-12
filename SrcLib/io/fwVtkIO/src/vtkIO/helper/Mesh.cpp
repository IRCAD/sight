/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkPoints.h>
#include <vtkDataArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkCell.h>
#include <vtkFloatArray.h>
#include <vtkSmartPointer.h>

#include <vtkMassProperties.h>
#include <vtkPolyDataNormals.h>
#include <vtkFillHolesFilter.h>

#include <fwData/Array.hpp>

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


        if(polyData->GetPointData()->HasArray("Colors"))
        {
            vtkSmartPointer<vtkUnsignedCharArray> colors;
            colors = vtkUnsignedCharArray::SafeDownCast(polyData->GetPointData()->GetArray("Colors"));
            FW_RAISE_IF("Only vtkUnsignedCharArray is supported to manage color.", !colors);

            size_t nbComponents = colors->GetNumberOfComponents();
            SLM_ASSERT("Wrong nb of components ("<<nbComponents<<")",
                    nbComponents == 3 || nbComponents == 4);
            mesh->allocatePointColors((::fwData::Mesh::ColorArrayTypes)nbComponents);
            ::fwData::Mesh::Id nbPoints = mesh->getNumberOfPoints() ;
            for (size_t i = 0; i != nbPoints; ++i)
            {
                mesh->setPointColor(i, colors->GetPointer(i*nbComponents));
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
            ::fwData::Mesh::Id nbCells = mesh->getNumberOfCells() ;
            for (size_t i = 0; i != nbCells; ++i)
            {
                mesh->setCellColor(i, colors->GetPointer(i*nbComponents));
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
            ::fwData::Mesh::Id nbPoints = mesh->getNumberOfPoints() ;
            for (size_t i = 0; i != nbPoints; ++i)
            {
                mesh->setPointNormal(i, normals->GetPointer(i*nbComponents));
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
            ::fwData::Mesh::Id nbCells = mesh->getNumberOfCells() ;
            for (size_t i = 0; i != nbCells; ++i)
            {
                mesh->setCellNormal(i, normals->GetPointer(i*nbComponents));
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

    Mesh::updatePolyDataPointNormals(polyData, mesh);
    Mesh::updatePolyDataCellNormals(polyData, mesh);

    Mesh::updatePolyDataPointColor(polyData, mesh);
    Mesh::updatePolyDataCellColor(polyData, mesh);
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

vtkSmartPointer<vtkPolyData> Mesh::updatePolyDataPointColor(vtkSmartPointer<vtkPolyData> polyDataDst, ::fwData::Mesh::sptr meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    ::fwData::Array::sptr pointColorArray = meshSrc->getPointColorsArray();
    if(pointColorArray)
    {
        ::fwData::Mesh::Id nbPoints = meshSrc->getNumberOfPoints() ;
        vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        size_t nbComponents = pointColorArray->getNumberOfComponents();
        colors->SetNumberOfComponents(nbComponents);
        colors->SetName("Colors");

        ::fwData::Array::IndexType colorIndex(1);
        for (size_t i = 0; i != nbPoints; ++i)
        {
            colorIndex[0] = i;
            colors->InsertNextTupleValue(pointColorArray->getItem<unsigned char>(colorIndex));
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

vtkSmartPointer<vtkPolyData> Mesh::updatePolyDataCellColor(vtkSmartPointer<vtkPolyData> polyDataDst, ::fwData::Mesh::sptr meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    ::fwData::Array::sptr cellColorArray = meshSrc->getCellColorsArray();
    if(cellColorArray)
    {
        ::fwData::Mesh::Id nbCells = meshSrc->getNumberOfCells() ;
        vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        size_t nbComponents = cellColorArray->getNumberOfComponents();
        colors->SetNumberOfComponents(nbComponents);
        colors->SetName("Colors");

        ::fwData::Array::IndexType colorIndex(1);
        for (size_t i = 0; i != nbCells; ++i)
        {
            colorIndex[0] = i;
            colors->InsertNextTupleValue(cellColorArray->getItem<unsigned char>(colorIndex));
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

vtkSmartPointer<vtkPolyData> Mesh::updatePolyDataPointNormals(vtkSmartPointer<vtkPolyData> polyDataDst, ::fwData::Mesh::sptr meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    ::fwData::Array::sptr pointNormalsArray = meshSrc->getPointNormalsArray();

    if(pointNormalsArray)
    {
        ::fwData::Mesh::Id nbPoints = meshSrc->getNumberOfPoints();
        vtkSmartPointer<vtkFloatArray> normals = vtkSmartPointer<vtkFloatArray>::New();
        size_t nbComponents = pointNormalsArray->getNumberOfComponents();
        normals->SetNumberOfComponents(nbComponents);

        ::fwData::Array::IndexType normalIndex(1);
        for (size_t i = 0; i != nbPoints; ++i)
        {
            normalIndex[0] = i;
            normals->InsertNextTupleValue(pointNormalsArray->getItem< float >(normalIndex));
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

vtkSmartPointer<vtkPolyData> Mesh::updatePolyDataCellNormals(vtkSmartPointer<vtkPolyData> polyDataDst, ::fwData::Mesh::sptr meshSrc )
{
    SLM_ASSERT( "vtkPolyData should not be NULL", polyDataDst);

    ::fwData::Array::sptr cellNormalsArray = meshSrc->getCellNormalsArray();

    if(cellNormalsArray)
    {
        ::fwData::Mesh::Id nbCells = meshSrc->getNumberOfCells();
        vtkSmartPointer<vtkFloatArray> normals = vtkSmartPointer<vtkFloatArray>::New();
        size_t nbComponents = cellNormalsArray->getNumberOfComponents();
        normals->SetNumberOfComponents(nbComponents);

        ::fwData::Array::IndexType normalIndex(1);
        for (size_t i = 0; i != nbCells; ++i)
        {
            normalIndex[0] = i;
            normals->InsertNextTupleValue(cellNormalsArray->getItem< float >(normalIndex));
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

//-----------------------------------------------------------------------------

double Mesh::computeVolume( ::fwData::Mesh::sptr mesh )
{
    //::fwData::Mesh::NewSptr closedMesh;
    //closedMesh->deepCopy(mesh);
    //::fwMath::closeSurface(closedMesh->points(), closedMesh->cells());

    vtkSmartPointer< vtkPolyData > vtkMeshRaw = vtkSmartPointer< vtkPolyData >::New();
    Mesh::toVTKMesh( mesh, vtkMeshRaw );

    // identify and fill holes in meshes
    vtkSmartPointer< vtkFillHolesFilter > holesFilter = vtkSmartPointer< vtkFillHolesFilter >::New();
    holesFilter->SetInput(vtkMeshRaw);

    // compute normals for polygonal mesh
    vtkSmartPointer< vtkPolyDataNormals > filter = vtkSmartPointer< vtkPolyDataNormals >::New();
    filter->SetInput(holesFilter->GetOutput());
    filter->AutoOrientNormalsOn();
    filter->FlipNormalsOff();

    // estimate volume, area, shape index of triangle mesh
    vtkSmartPointer< vtkMassProperties > calculator = vtkSmartPointer< vtkMassProperties >::New();
    calculator->SetInput( filter->GetOutput() );
    calculator->Update();
    double volume =  calculator->GetVolume();
    OSLM_DEBUG("GetVolume : " << volume << " vtkMassProperties::GetVolumeProjected = " << calculator->GetVolumeProjected() );
    OSLM_DEBUG("Error : " << (calculator->GetVolume()- fabs(calculator->GetVolumeProjected()))*10000);
    if ( (calculator->GetVolume()- fabs(calculator->GetVolumeProjected()))*10000 > calculator->GetVolume() )
    {
        std::stringstream ss;
        ss << "vtkMassProperties::GetVolume() - | vtkMassProperties::GetVolumeProjected() |";
        ss << ">  vtkMassProperties::GetVolume()/10000.0" << std::endl;
        ss << "vtkMassProperties::GetVolume() = " << volume << " vtkMassProperties::GetVolumeProjected = " << calculator->GetVolumeProjected();
        throw (std::out_of_range( ss.str() ));
    }

    return volume;
}

//------------------------------------------------------------------------------

} // namespace helper
} // namespace vtkIO
