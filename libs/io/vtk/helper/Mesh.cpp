/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "io/vtk/helper/Mesh.hpp"

#include <data/Array.hpp>

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
#include <vtkUnsignedCharArray.h>

namespace sight::io::vtk::helper
{

//------------------------------------------------------------------------------

void fromVTKPointSet(vtkPointSet& dataset, data::Mesh& mesh)
{
    vtkPoints* points = dataset.GetPoints();
    if(points != nullptr)
    {
        mesh.clear();
        const vtkIdType numberOfPoints = points->GetNumberOfPoints();
        const vtkIdType numberOfCells  = dataset.GetNumberOfCells();

        data::Mesh::Attributes attributes                 = data::Mesh::Attributes::NONE;
        vtkSmartPointer<vtkUnsignedCharArray> pointColors = nullptr;
        vtkSmartPointer<vtkUnsignedCharArray> cellColors  = nullptr;
        vtkSmartPointer<vtkFloatArray> pointNormals       = nullptr;
        vtkSmartPointer<vtkFloatArray> cellNormals        = nullptr;
        vtkSmartPointer<vtkFloatArray> pointTexCoords     = nullptr;
        vtkSmartPointer<vtkFloatArray> cellTexCoords      = nullptr;

        const std::array color_array_name {"Colors", "RGB", "RGBA"};

        auto* const point_data = dataset.GetPointData();
        for(const auto& color_array : color_array_name)
        {
            if(point_data->HasArray(color_array) != 0)
            {
                int idx = 0;
                point_data->GetAbstractArray(color_array, idx);
                if(point_data->IsArrayAnAttribute(idx) == vtkDataSetAttributes::SCALARS)
                {
                    attributes  = attributes | data::Mesh::Attributes::POINT_COLORS;
                    pointColors = vtkUnsignedCharArray::SafeDownCast(point_data->GetScalars(color_array));
                    break;
                }
            }
        }

        auto* const cell_data = dataset.GetCellData();
        for(const auto& color_array : color_array_name)
        {
            if(cell_data->HasArray(color_array) != 0)
            {
                int idx = 0;
                cell_data->GetAbstractArray(color_array, idx);
                if(cell_data->IsArrayAnAttribute(idx) == vtkDataSetAttributes::SCALARS)
                {
                    attributes = attributes | data::Mesh::Attributes::CELL_COLORS;
                    cellColors = vtkUnsignedCharArray::SafeDownCast(cell_data->GetScalars(color_array));
                    break;
                }
            }
        }

        if(point_data->GetAttribute(vtkDataSetAttributes::NORMALS) != nullptr)
        {
            attributes   = attributes | data::Mesh::Attributes::POINT_NORMALS;
            pointNormals = vtkFloatArray::SafeDownCast(point_data->GetNormals());
        }

        if(cell_data->GetAttribute(vtkDataSetAttributes::NORMALS) != nullptr)
        {
            attributes  = attributes | data::Mesh::Attributes::CELL_NORMALS;
            cellNormals = vtkFloatArray::SafeDownCast(cell_data->GetNormals());
        }

        if(point_data->GetAttribute(vtkDataSetAttributes::TCOORDS) != nullptr)
        {
            attributes     = attributes | data::Mesh::Attributes::POINT_TEX_COORDS;
            pointTexCoords = vtkFloatArray::SafeDownCast(point_data->GetTCoords());
        }

        if(cell_data->GetAttribute(vtkDataSetAttributes::TCOORDS) != nullptr)
        {
            attributes    = attributes | data::Mesh::Attributes::CELL_TEX_COORDS;
            cellTexCoords = vtkFloatArray::SafeDownCast(cell_data->GetTCoords());
        }

        const auto dumpLock = mesh.dump_lock();

        int firstCellType = (numberOfCells > 0) ? dataset.GetCell(0)->GetCellType() : VTK_EMPTY_CELL;

        static std::map<int, data::Mesh::CellType> s_getCellType = {
            {VTK_VERTEX, data::Mesh::CellType::POINT},
            {VTK_LINE, data::Mesh::CellType::LINE},
            {VTK_TRIANGLE, data::Mesh::CellType::TRIANGLE},
            {VTK_QUAD, data::Mesh::CellType::QUAD},
            {VTK_TETRA, data::Mesh::CellType::TETRA},
        };

        mesh.resize(
            static_cast<data::Mesh::size_t>(numberOfPoints),
            static_cast<data::Mesh::size_t>(numberOfCells),
            s_getCellType[firstCellType],
            attributes
        );

        const double* point = nullptr;

        {
            vtkIdType i = 0;
            for(auto& pos : mesh.range<data::iterator::point::xyz>())
            {
                point = points->GetPoint(i++);
                pos   = {static_cast<float>(point[0]), static_cast<float>(point[1]), static_cast<float>(point[2])};
            }
        }

        if(pointColors != nullptr)
        {
            vtkIdType i = 0;
            for(auto& c : mesh.range<data::iterator::point::rgba>())
            {
                if(pointColors->GetNumberOfComponents() == 3)
                {
                    auto* const color = pointColors->GetPointer(i++ *3);
                    c.r = color[0];
                    c.g = color[1];
                    c.b = color[2];
                    c.a = 255;
                }
                else
                {
                    auto* const color = pointColors->GetPointer(i++ *4);
                    c.r = color[0];
                    c.g = color[1];
                    c.b = color[2];
                    c.a = color[3];
                }
            }
        }

        if(pointNormals != nullptr)
        {
            vtkIdType i = 0;
            for(auto& n : mesh.range<data::iterator::point::nxyz>())
            {
                auto* const normal = pointNormals->GetPointer(i++ *3);
                n.nx = normal[0];
                n.ny = normal[1];
                n.nz = normal[2];
            }
        }

        if(pointTexCoords != nullptr)
        {
            vtkIdType i = 0;
            for(auto& uv : mesh.range<data::iterator::point::uv>())
            {
                auto* const texCoords = pointTexCoords->GetPointer(i++ *2);
                uv.u = texCoords[0];
                uv.v = texCoords[1];
            }
        }

        auto getVtkCell = [&dataset](auto i, [[maybe_unused]] int numId)
                          {
                              const auto cell   = dataset.GetCell(i);
                              const auto idList = cell->GetPointIds();
                              SIGHT_ASSERT(
                                  "Wrong number of ids: " << idList->GetNumberOfIds(),
                                  idList->GetNumberOfIds() == numId
                              );
                              return std::make_pair(idList, cell->GetCellType());
                          };

        {
            std::size_t skip = 0;
            vtkIdType i      = 0;
            switch(firstCellType)
            {
                case VTK_VERTEX:

                    for(auto& cell : mesh.range<data::iterator::cell::point>())
                    {
                        const auto& [idList, cellType] = getVtkCell(i++, 1);
                        if(firstCellType != cellType)
                        {
                            ++skip;
                            continue;
                        }

                        cell.pt = static_cast<data::Mesh::point_t>(idList->GetId(0));
                    }

                    break;

                case VTK_LINE:

                    for(auto& cell : mesh.range<data::iterator::cell::line>())
                    {
                        const auto& [idList, cellType] = getVtkCell(i++, 2);
                        if(firstCellType != cellType)
                        {
                            ++skip;
                            continue;
                        }

                        cell.pt[0] = static_cast<data::Mesh::point_t>(idList->GetId(0));
                        cell.pt[1] = static_cast<data::Mesh::point_t>(idList->GetId(1));
                    }

                    break;

                case VTK_TRIANGLE:
                    for(auto& cell : mesh.range<data::iterator::cell::triangle>())
                    {
                        const auto& [idList, cellType] = getVtkCell(i++, 3);
                        if(firstCellType != cellType)
                        {
                            ++skip;
                            continue;
                        }

                        cell.pt[0] = static_cast<data::Mesh::point_t>(idList->GetId(0));
                        cell.pt[1] = static_cast<data::Mesh::point_t>(idList->GetId(1));
                        cell.pt[2] = static_cast<data::Mesh::point_t>(idList->GetId(2));
                    }

                    break;

                case VTK_QUAD:
                case VTK_TETRA:

                    for(auto& cell : mesh.range<data::iterator::cell::quad>())
                    {
                        const auto& [idList, cellType] = getVtkCell(i++, 4);
                        if(firstCellType != cellType)
                        {
                            ++skip;
                            continue;
                        }

                        cell.pt[0] = static_cast<data::Mesh::point_t>(idList->GetId(0));
                        cell.pt[1] = static_cast<data::Mesh::point_t>(idList->GetId(1));
                        cell.pt[2] = static_cast<data::Mesh::point_t>(idList->GetId(2));
                        cell.pt[3] = static_cast<data::Mesh::point_t>(idList->GetId(3));
                    }

                    break;

                default:
                    SIGHT_THROW("VTK Mesh type " << firstCellType << " not supported.");
            }

            SIGHT_ERROR_IF(skip << " mixed cells were skipped, we only support a single cell type in a mesh", skip);
        }

        if(cellColors != nullptr)
        {
            vtkIdType i = 0;
            for(auto& c : mesh.range<data::iterator::cell::rgba>())
            {
                if(cellColors->GetNumberOfComponents() == 3)
                {
                    auto* const color = cellColors->GetPointer(i++ *3);
                    c.r = color[0];
                    c.g = color[1];
                    c.b = color[2];
                    c.a = 255;
                }
                else
                {
                    auto* const color = cellColors->GetPointer(i++ *4);
                    c.r = color[0];
                    c.g = color[1];
                    c.b = color[2];
                    c.a = color[3];
                }
            }
        }

        if(cellNormals != nullptr)
        {
            vtkIdType i = 0;
            for(auto& n : mesh.range<data::iterator::cell::nxyz>())
            {
                auto* const normal = cellNormals->GetPointer(i++ *3);
                n.nx = normal[0];
                n.ny = normal[1];
                n.nz = normal[2];
            }
        }

        if(cellTexCoords != nullptr)
        {
            vtkIdType i = 0;
            for(auto& uv : mesh.range<data::iterator::cell::uv>())
            {
                auto* const texCoords = cellTexCoords->GetPointer(i++ *2);
                uv.u = texCoords[0];
                uv.v = texCoords[1];
            }
        }
    }
}

//------------------------------------------------------------------------------

void updatePointsAndAttributes(vtkPointSet& pointSetDst, const data::Mesh& meshSrc)
{
    vtkPoints* dataPoints = pointSetDst.GetPoints();

    const auto nbPoints = static_cast<vtkIdType>(meshSrc.numPoints());

    if(nbPoints != dataPoints->GetNumberOfPoints())
    {
        dataPoints->SetNumberOfPoints(nbPoints);
    }

    vtkIdType i = 0;
    for(const auto& pos : meshSrc.crange<data::iterator::point::xyz>())
    {
        dataPoints->SetPoint(
            i++,
            static_cast<double>(pos.x),
            static_cast<double>(pos.y),
            static_cast<double>(pos.z)
        );
    }

    unsigned char* newColors            = nullptr;
    float* newNormals                   = nullptr;
    float* newTexCoords                 = nullptr;
    const std::size_t nbColorComponents = 4;
    if(meshSrc.has<data::Mesh::Attributes::POINT_COLORS>())
    {
        newColors = new unsigned char [static_cast<std::size_t>(nbPoints) * nbColorComponents];
        i         = 0;
        for(const auto& c : meshSrc.crange<data::iterator::point::rgba>())
        {
            newColors[i * 4]     = c.r;
            newColors[i * 4 + 1] = c.g;
            newColors[i * 4 + 2] = c.b;
            newColors[i * 4 + 3] = c.a;
            ++i;
        }
    }

    if(meshSrc.has<data::Mesh::Attributes::POINT_NORMALS>())
    {
        newNormals = new float [static_cast<std::size_t>(nbPoints * 3)];
        i          = 0;
        for(const auto& n : meshSrc.crange<data::iterator::point::nxyz>())
        {
            newNormals[i * 3]     = n.nx;
            newNormals[i * 3 + 1] = n.ny;
            newNormals[i * 3 + 2] = n.nz;
            ++i;
        }
    }

    if(meshSrc.has<data::Mesh::Attributes::POINT_TEX_COORDS>())
    {
        newTexCoords = new float [static_cast<std::size_t>(nbPoints * 2)];
        i            = 0;
        for(const auto& uv : meshSrc.crange<data::iterator::point::uv>())
        {
            newTexCoords[i * 2]     = uv.u;
            newTexCoords[i * 2 + 1] = uv.v;
            ++i;
        }
    }

    if(meshSrc.has<data::Mesh::Attributes::POINT_COLORS>())
    {
        vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        colors->SetNumberOfComponents(static_cast<int>(nbColorComponents));
        colors->SetName("Colors");
        colors->SetArray(
            newColors,
            nbPoints * static_cast<vtkIdType>(nbColorComponents),
            0,
            vtkUnsignedCharArray::VTK_DATA_ARRAY_DELETE
        );
        pointSetDst.GetPointData()->SetScalars(colors);
    }
    else if(pointSetDst.GetPointData()->HasArray("Colors") != 0)
    {
        pointSetDst.GetPointData()->RemoveArray("Colors");
    }

    if(meshSrc.has<data::Mesh::Attributes::POINT_NORMALS>())
    {
        vtkSmartPointer<vtkFloatArray> normals = vtkSmartPointer<vtkFloatArray>::New();
        normals->SetNumberOfComponents(3);
        normals->SetArray(newNormals, nbPoints * 3, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        pointSetDst.GetPointData()->SetNormals(normals);
    }
    else if(pointSetDst.GetPointData()->GetAttribute(vtkDataSetAttributes::NORMALS) != nullptr)
    {
        pointSetDst.GetPointData()->RemoveArray(vtkDataSetAttributes::NORMALS);
    }

    if(meshSrc.has<data::Mesh::Attributes::POINT_TEX_COORDS>())
    {
        vtkSmartPointer<vtkFloatArray> texCoords = vtkSmartPointer<vtkFloatArray>::New();
        texCoords->SetNumberOfComponents(2);
        texCoords->SetArray(newTexCoords, nbPoints * 2, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);
        pointSetDst.GetPointData()->SetTCoords(texCoords);
    }
    else if(pointSetDst.GetPointData()->GetAttribute(vtkDataSetAttributes::TCOORDS) != nullptr)
    {
        pointSetDst.GetPointData()->RemoveArray(vtkDataSetAttributes::TCOORDS);
    }

    dataPoints->Modified();
    pointSetDst.Modified();
}

//------------------------------------------------------------------------------

void toVTKPointSet(const data::Mesh& mesh, vtkPointSet& dataset)
{
    const vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
    dataset.SetPoints(pts);

    const auto nbCells  = mesh.numCells();
    const auto dumpLock = mesh.dump_lock();

    if(nbCells > 0)
    {
        vtkPolyData* polyData     = vtkPolyData::SafeDownCast(&dataset);
        vtkUnstructuredGrid* grid = vtkUnstructuredGrid::SafeDownCast(&dataset);
        SIGHT_ASSERT(
            "Pointset must be either a vtkPolyData or a vtkUnstructuredGrid",
            (polyData && !grid) || (!polyData && grid)
        );

        if(polyData != nullptr)
        {
            polyData->Allocate(static_cast<int>(nbCells));
        }
        else
        {
            grid->Allocate(static_cast<int>(nbCells));
        }

        unsigned char* newColors            = nullptr;
        float* newNormals                   = nullptr;
        float* newTexCoords                 = nullptr;
        const std::size_t nbColorComponents = 4;

        if(mesh.has<data::Mesh::Attributes::CELL_COLORS>())
        {
            newColors = new unsigned char [nbCells * nbColorComponents];

            vtkIdType i = 0;
            for(const auto& c : mesh.crange<data::iterator::cell::rgba>())
            {
                newColors[i * 4]     = c.r;
                newColors[i * 4 + 1] = c.g;
                newColors[i * 4 + 2] = c.b;
                newColors[i * 4 + 3] = c.a;
                ++i;
            }
        }

        if(mesh.has<data::Mesh::Attributes::CELL_NORMALS>())
        {
            newNormals = new float [nbCells * 3LL];
            vtkIdType i = 0;
            for(const auto& n : mesh.crange<data::iterator::cell::nxyz>())
            {
                newNormals[i * 3]     = n.nx;
                newNormals[i * 3 + 1] = n.ny;
                newNormals[i * 3 + 2] = n.nz;
                ++i;
            }
        }

        if(mesh.has<data::Mesh::Attributes::CELL_TEX_COORDS>())
        {
            newTexCoords = new float [nbCells * 2LL];
            vtkIdType i = 0;
            for(const auto& uv : mesh.crange<data::iterator::cell::uv>())
            {
                newTexCoords[i * 2]     = uv.u;
                newTexCoords[i * 2 + 1] = uv.v;
                ++i;
            }
        }

        auto insertNextCell = [&polyData, &grid](auto typeVtkCell, auto i, auto cell)
                              {
                                  if(polyData)
                                  {
                                      polyData->InsertNextCell(typeVtkCell, i, cell);
                                  }
                                  else
                                  {
                                      grid->InsertNextCell(typeVtkCell, i, cell);
                                  }
                              };

        int typeVtkCell = 0;
        std::array<vtkIdType, 4> cell {};
        switch(mesh.getCellType())
        {
            case data::Mesh::CellType::POINT:
                typeVtkCell = VTK_VERTEX;
                for(const auto& p : mesh.crange<data::iterator::cell::point>())
                {
                    cell[0] = static_cast<vtkIdType>(p.pt);
                    insertNextCell(typeVtkCell, 1, cell.data());
                }

                break;

            case data::Mesh::CellType::LINE:
                typeVtkCell = VTK_LINE;

                for(const auto& p : mesh.crange<data::iterator::cell::line>())
                {
                    cell[0] = static_cast<vtkIdType>(p.pt[0]);
                    cell[1] = static_cast<vtkIdType>(p.pt[1]);
                    insertNextCell(typeVtkCell, 2, cell.data());
                }

                break;

            case data::Mesh::CellType::TRIANGLE:
                typeVtkCell = VTK_TRIANGLE;

                for(const auto& p : mesh.crange<data::iterator::cell::triangle>())
                {
                    cell[0] = static_cast<vtkIdType>(p.pt[0]);
                    cell[1] = static_cast<vtkIdType>(p.pt[1]);
                    cell[2] = static_cast<vtkIdType>(p.pt[2]);
                    insertNextCell(typeVtkCell, 3, cell.data());
                }

                break;

            case data::Mesh::CellType::QUAD:
                typeVtkCell = VTK_QUAD;

                for(const auto& p : mesh.crange<data::iterator::cell::quad>())
                {
                    cell[0] = static_cast<vtkIdType>(p.pt[0]);
                    cell[1] = static_cast<vtkIdType>(p.pt[1]);
                    cell[2] = static_cast<vtkIdType>(p.pt[2]);
                    cell[3] = static_cast<vtkIdType>(p.pt[3]);
                    insertNextCell(typeVtkCell, 4, cell.data());
                }

                break;

            case data::Mesh::CellType::TETRA:
                typeVtkCell = VTK_TETRA;
                for(const auto& p : mesh.crange<data::iterator::cell::tetra>())
                {
                    cell[0] = static_cast<vtkIdType>(p.pt[0]);
                    cell[1] = static_cast<vtkIdType>(p.pt[1]);
                    cell[2] = static_cast<vtkIdType>(p.pt[2]);
                    cell[3] = static_cast<vtkIdType>(p.pt[3]);
                    insertNextCell(typeVtkCell, 4, cell.data());
                }

                break;

            default:
                SIGHT_THROW("Mesh type not supported.");
        }

        if(mesh.has<data::Mesh::Attributes::CELL_COLORS>())
        {
            vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
            colors->SetNumberOfComponents(static_cast<int>(nbColorComponents));
            colors->SetName("Colors");
            colors->SetArray(
                newColors,
                static_cast<vtkIdType>(nbCells) * static_cast<vtkIdType>(nbColorComponents),
                0,
                vtkUnsignedCharArray::VTK_DATA_ARRAY_DELETE
            );
            dataset.GetCellData()->SetScalars(colors);
        }
        else if(dataset.GetCellData()->HasArray("Colors") != 0)
        {
            dataset.GetCellData()->RemoveArray("Colors");
        }

        if(mesh.has<data::Mesh::Attributes::CELL_NORMALS>())
        {
            vtkSmartPointer<vtkFloatArray> normals = vtkSmartPointer<vtkFloatArray>::New();
            normals->SetNumberOfComponents(3);
            normals->SetArray(newNormals, static_cast<vtkIdType>(nbCells) * 3, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

            dataset.GetCellData()->SetNormals(normals);
        }
        else if(dataset.GetCellData()->GetAttribute(vtkDataSetAttributes::NORMALS) != nullptr)
        {
            dataset.GetCellData()->RemoveArray(vtkDataSetAttributes::NORMALS);
        }

        if(mesh.has<data::Mesh::Attributes::CELL_TEX_COORDS>())
        {
            vtkSmartPointer<vtkFloatArray> texCoords = vtkSmartPointer<vtkFloatArray>::New();
            texCoords->SetNumberOfComponents(2);
            texCoords->SetArray(
                newTexCoords,
                static_cast<vtkIdType>(nbCells) * 2,
                0,
                vtkFloatArray::VTK_DATA_ARRAY_DELETE
            );
            dataset.GetCellData()->SetTCoords(texCoords);
        }
        else if(dataset.GetCellData()->GetAttribute(vtkDataSetAttributes::TCOORDS) != nullptr)
        {
            dataset.GetCellData()->RemoveArray(vtkDataSetAttributes::TCOORDS);
        }

        dataset.Modified();
    }

    updatePointsAndAttributes(dataset, mesh);
}

//------------------------------------------------------------------------------

void Mesh::fromVTKMesh(vtkSmartPointer<vtkPolyData> polyData, data::Mesh::sptr mesh)
{
    fromVTKPointSet(*polyData, *mesh);
}

//------------------------------------------------------------------------------

void Mesh::fromVTKGrid(vtkSmartPointer<vtkUnstructuredGrid> grid, data::Mesh::sptr mesh)
{
    fromVTKPointSet(*grid, *mesh);
}

//------------------------------------------------------------------------------

void Mesh::toVTKMesh(const data::Mesh::csptr& mesh, vtkSmartPointer<vtkPolyData> polyData)
{
    toVTKPointSet(*mesh, *polyData);
}

//------------------------------------------------------------------------------

void Mesh::toVTKGrid(const data::Mesh::csptr& mesh, vtkSmartPointer<vtkUnstructuredGrid> grid)
{
    toVTKPointSet(*mesh, *grid);
}

//-----------------------------------------------------------------------------

double Mesh::computeVolume(const data::Mesh::csptr& mesh)
{
    vtkSmartPointer<vtkPolyData> vtkMeshRaw = vtkSmartPointer<vtkPolyData>::New();
    Mesh::toVTKMesh(mesh, vtkMeshRaw);

    // identify and fill holes in meshes
    vtkSmartPointer<vtkFillHolesFilter> holesFilter = vtkSmartPointer<vtkFillHolesFilter>::New();
    holesFilter->SetHoleSize(2000);
    holesFilter->SetInputData(vtkMeshRaw);
    holesFilter->Update();
    if(holesFilter->GetOutput()->GetNumberOfCells() > 0) // Filter return empty mesh when no topological holes are
                                                         // present
    {
        vtkMeshRaw = holesFilter->GetOutput();
    }

    // compute normals for polygonal mesh
    const vtkSmartPointer<vtkPolyDataNormals> filter = vtkSmartPointer<vtkPolyDataNormals>::New();
    filter->SetInputData(vtkMeshRaw);
    filter->AutoOrientNormalsOn();
    filter->FlipNormalsOff();

    // estimate volume, area, shape index of triangle mesh
    const vtkSmartPointer<vtkMassProperties> calculator = vtkSmartPointer<vtkMassProperties>::New();
    calculator->SetInputConnection(filter->GetOutputPort());
    calculator->Update();

    const double volume = calculator->GetVolume();
    SIGHT_DEBUG(
        "GetVolume : " << volume << " vtkMassProperties::GetVolumeProjected = "
        << calculator->GetVolumeProjected()
    );
    SIGHT_DEBUG("Error : " << (calculator->GetVolume() - fabs(calculator->GetVolumeProjected())) * 10000);
    if((calculator->GetVolume() - fabs(calculator->GetVolumeProjected())) * 10000 > calculator->GetVolume())
    {
        std::stringstream ss;
        ss << "volume - |volume projected| > volume/10000.0" << std::endl;
        ss << "[volume = " << volume << "]" << std::endl;
        ss << "[volume projected = " << calculator->GetVolumeProjected() << "]";
        throw(std::out_of_range(ss.str()));
    }

    return volume;
}

//-----------------------------------------------------------------------------

} // namespace sight::io::vtk::helper
