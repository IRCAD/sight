/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "io/vtk/helper/mesh.hpp"

#include <data/array.hpp>

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

void from_vtk_point_set(vtkPointSet& _dataset, data::mesh& _mesh)
{
    vtkPoints* points = _dataset.GetPoints();
    if(points != nullptr)
    {
        _mesh.clear();
        const vtkIdType number_of_points = points->GetNumberOfPoints();
        const vtkIdType number_of_cells  = _dataset.GetNumberOfCells();

        data::mesh::attribute attributes                   = data::mesh::attribute::none;
        vtkSmartPointer<vtkUnsignedCharArray> point_colors = nullptr;
        vtkSmartPointer<vtkUnsignedCharArray> cell_colors  = nullptr;
        vtkSmartPointer<vtkFloatArray> point_normals       = nullptr;
        vtkSmartPointer<vtkFloatArray> cell_normals        = nullptr;
        vtkSmartPointer<vtkFloatArray> point_tex_coords    = nullptr;
        vtkSmartPointer<vtkFloatArray> cell_tex_coords     = nullptr;

        const std::array color_array_name {"Colors", "RGB", "RGBA"};

        auto* const point_data = _dataset.GetPointData();
        for(const auto& color_array : color_array_name)
        {
            if(point_data->HasArray(color_array) != 0)
            {
                int idx = 0;
                point_data->GetAbstractArray(color_array, idx);
                if(point_data->IsArrayAnAttribute(idx) == vtkDataSetAttributes::SCALARS)
                {
                    attributes   = attributes | data::mesh::attribute::point_colors;
                    point_colors = vtkUnsignedCharArray::SafeDownCast(point_data->GetScalars(color_array));
                    break;
                }
            }
        }

        auto* const cell_data = _dataset.GetCellData();
        for(const auto& color_array : color_array_name)
        {
            if(cell_data->HasArray(color_array) != 0)
            {
                int idx = 0;
                cell_data->GetAbstractArray(color_array, idx);
                if(cell_data->IsArrayAnAttribute(idx) == vtkDataSetAttributes::SCALARS)
                {
                    attributes  = attributes | data::mesh::attribute::cell_colors;
                    cell_colors = vtkUnsignedCharArray::SafeDownCast(cell_data->GetScalars(color_array));
                    break;
                }
            }
        }

        if(point_data->GetAttribute(vtkDataSetAttributes::NORMALS) != nullptr)
        {
            attributes    = attributes | data::mesh::attribute::point_normals;
            point_normals = vtkFloatArray::SafeDownCast(point_data->GetNormals());
        }

        if(cell_data->GetAttribute(vtkDataSetAttributes::NORMALS) != nullptr)
        {
            attributes   = attributes | data::mesh::attribute::cell_normals;
            cell_normals = vtkFloatArray::SafeDownCast(cell_data->GetNormals());
        }

        if(point_data->GetAttribute(vtkDataSetAttributes::TCOORDS) != nullptr)
        {
            attributes       = attributes | data::mesh::attribute::point_tex_coords;
            point_tex_coords = vtkFloatArray::SafeDownCast(point_data->GetTCoords());
        }

        if(cell_data->GetAttribute(vtkDataSetAttributes::TCOORDS) != nullptr)
        {
            attributes      = attributes | data::mesh::attribute::cell_tex_coords;
            cell_tex_coords = vtkFloatArray::SafeDownCast(cell_data->GetTCoords());
        }

        const auto dump_lock = _mesh.dump_lock();

        int first_cell_type = (number_of_cells > 0) ? _dataset.GetCell(0)->GetCellType() : VTK_EMPTY_CELL;

        static std::map<int, data::mesh::cell_type_t> s_get_cell_type = {
            {VTK_VERTEX, data::mesh::cell_type_t::point},
            {VTK_LINE, data::mesh::cell_type_t::line},
            {VTK_TRIANGLE, data::mesh::cell_type_t::triangle},
            {VTK_QUAD, data::mesh::cell_type_t::quad},
            {VTK_TETRA, data::mesh::cell_type_t::tetra},
        };

        if(number_of_points > 0 && number_of_cells > 0)
        {
            _mesh.resize(
                static_cast<data::mesh::size_t>(number_of_points),
                static_cast<data::mesh::size_t>(number_of_cells),
                s_get_cell_type[first_cell_type],
                attributes
            );
        }

        {
            vtkIdType i = 0;
            for(auto& pos : _mesh.range<data::iterator::point::xyz>())
            {
                const double* point = points->GetPoint(i++);
                pos = {static_cast<float>(point[0]), static_cast<float>(point[1]), static_cast<float>(point[2])};
            }
        }

        if(point_colors != nullptr)
        {
            vtkIdType i = 0;
            for(auto& c : _mesh.range<data::iterator::point::rgba>())
            {
                if(point_colors->GetNumberOfComponents() == 3)
                {
                    auto* const color = point_colors->GetPointer(i++ * 3);
                    c.r = color[0];
                    c.g = color[1];
                    c.b = color[2];
                    c.a = 255;
                }
                else
                {
                    auto* const color = point_colors->GetPointer(i++ * 4);
                    c.r = color[0];
                    c.g = color[1];
                    c.b = color[2];
                    c.a = color[3];
                }
            }
        }

        if(point_normals != nullptr)
        {
            vtkIdType i = 0;
            for(auto& n : _mesh.range<data::iterator::point::nxyz>())
            {
                auto* const normal = point_normals->GetPointer(i++ * 3);
                n.nx = normal[0];
                n.ny = normal[1];
                n.nz = normal[2];
            }
        }

        if(point_tex_coords != nullptr)
        {
            vtkIdType i = 0;
            for(auto& uv : _mesh.range<data::iterator::point::uv>())
            {
                auto* const tex_coords = point_tex_coords->GetPointer(i++ * 2);
                uv.u = tex_coords[0];
                uv.v = tex_coords[1];
            }
        }

        auto get_vtk_cell = [&_dataset](auto _i, [[maybe_unused]] int _num_id)
                            {
                                const auto cell    = _dataset.GetCell(_i);
                                const auto id_list = cell->GetPointIds();
                                SIGHT_ASSERT(
                                    "Wrong number of ids: " << id_list->GetNumberOfIds(),
                                    id_list->GetNumberOfIds() == _num_id
                                );
                                return std::make_pair(id_list, cell->GetCellType());
                            };

        {
            std::size_t skip = 0;
            vtkIdType i      = 0;
            switch(first_cell_type)
            {
                case VTK_VERTEX:

                    for(auto& cell : _mesh.range<data::iterator::cell::point>())
                    {
                        const auto& [idList, cellType] = get_vtk_cell(i++, 1);
                        if(first_cell_type != cellType)
                        {
                            ++skip;
                            continue;
                        }

                        cell.pt = static_cast<data::mesh::point_t>(idList->GetId(0));
                    }

                    break;

                case VTK_LINE:

                    for(auto& cell : _mesh.range<data::iterator::cell::line>())
                    {
                        const auto& [idList, cellType] = get_vtk_cell(i++, 2);
                        if(first_cell_type != cellType)
                        {
                            ++skip;
                            continue;
                        }

                        cell.pt[0] = static_cast<data::mesh::point_t>(idList->GetId(0));
                        cell.pt[1] = static_cast<data::mesh::point_t>(idList->GetId(1));
                    }

                    break;

                case VTK_TRIANGLE:
                    for(auto& cell : _mesh.range<data::iterator::cell::triangle>())
                    {
                        const auto& [idList, cellType] = get_vtk_cell(i++, 3);
                        if(first_cell_type != cellType)
                        {
                            ++skip;
                            continue;
                        }

                        cell.pt[0] = static_cast<data::mesh::point_t>(idList->GetId(0));
                        cell.pt[1] = static_cast<data::mesh::point_t>(idList->GetId(1));
                        cell.pt[2] = static_cast<data::mesh::point_t>(idList->GetId(2));
                    }

                    break;

                case VTK_QUAD:
                case VTK_TETRA:

                    for(auto& cell : _mesh.range<data::iterator::cell::quad>())
                    {
                        const auto& [idList, cellType] = get_vtk_cell(i++, 4);
                        if(first_cell_type != cellType)
                        {
                            ++skip;
                            continue;
                        }

                        cell.pt[0] = static_cast<data::mesh::point_t>(idList->GetId(0));
                        cell.pt[1] = static_cast<data::mesh::point_t>(idList->GetId(1));
                        cell.pt[2] = static_cast<data::mesh::point_t>(idList->GetId(2));
                        cell.pt[3] = static_cast<data::mesh::point_t>(idList->GetId(3));
                    }

                    break;

                case VTK_EMPTY_CELL:
                    // Nothing to do.
                    break;

                default:
                    SIGHT_THROW("VTK Mesh type " << first_cell_type << " not supported.");
            }

            SIGHT_ERROR_IF(skip << " mixed cells were skipped, we only support a single cell type in a mesh", skip);
        }

        if(cell_colors != nullptr)
        {
            vtkIdType i = 0;
            for(auto& c : _mesh.range<data::iterator::cell::rgba>())
            {
                if(cell_colors->GetNumberOfComponents() == 3)
                {
                    auto* const color = cell_colors->GetPointer(i++ * 3);
                    c.r = color[0];
                    c.g = color[1];
                    c.b = color[2];
                    c.a = 255;
                }
                else
                {
                    auto* const color = cell_colors->GetPointer(i++ * 4);
                    c.r = color[0];
                    c.g = color[1];
                    c.b = color[2];
                    c.a = color[3];
                }
            }
        }

        if(cell_normals != nullptr)
        {
            vtkIdType i = 0;
            for(auto& n : _mesh.range<data::iterator::cell::nxyz>())
            {
                auto* const normal = cell_normals->GetPointer(i++ * 3);
                n.nx = normal[0];
                n.ny = normal[1];
                n.nz = normal[2];
            }
        }

        if(cell_tex_coords != nullptr)
        {
            vtkIdType i = 0;
            for(auto& uv : _mesh.range<data::iterator::cell::uv>())
            {
                auto* const tex_coords = cell_tex_coords->GetPointer(i++ * 2);
                uv.u = tex_coords[0];
                uv.v = tex_coords[1];
            }
        }
    }
}

//------------------------------------------------------------------------------

void update_points_and_attributes(vtkPointSet& _point_set_dst, const data::mesh& _mesh_src)
{
    vtkPoints* data_points = _point_set_dst.GetPoints();

    const auto nb_points = static_cast<vtkIdType>(_mesh_src.num_points());

    if(nb_points != data_points->GetNumberOfPoints())
    {
        data_points->SetNumberOfPoints(nb_points);
    }

    vtkIdType i = 0;
    for(const auto& pos : _mesh_src.crange<data::iterator::point::xyz>())
    {
        data_points->SetPoint(
            i++,
            static_cast<double>(pos.x),
            static_cast<double>(pos.y),
            static_cast<double>(pos.z)
        );
    }

    unsigned char* new_colors             = nullptr;
    float* new_normals                    = nullptr;
    float* new_tex_coords                 = nullptr;
    const std::size_t nb_color_components = 4;
    if(_mesh_src.has<data::mesh::attribute::point_colors>())
    {
        new_colors = new unsigned char [static_cast<std::size_t>(nb_points) * nb_color_components];
        i          = 0;
        for(const auto& c : _mesh_src.crange<data::iterator::point::rgba>())
        {
            new_colors[i * 4]     = c.r;
            new_colors[i * 4 + 1] = c.g;
            new_colors[i * 4 + 2] = c.b;
            new_colors[i * 4 + 3] = c.a;
            ++i;
        }
    }

    if(_mesh_src.has<data::mesh::attribute::point_normals>())
    {
        new_normals = new float [static_cast<std::size_t>(nb_points * 3)];
        i           = 0;
        for(const auto& n : _mesh_src.crange<data::iterator::point::nxyz>())
        {
            new_normals[i * 3]     = n.nx;
            new_normals[i * 3 + 1] = n.ny;
            new_normals[i * 3 + 2] = n.nz;
            ++i;
        }
    }

    if(_mesh_src.has<data::mesh::attribute::point_tex_coords>())
    {
        new_tex_coords = new float [static_cast<std::size_t>(nb_points * 2)];
        i              = 0;
        for(const auto& uv : _mesh_src.crange<data::iterator::point::uv>())
        {
            new_tex_coords[i * 2]     = uv.u;
            new_tex_coords[i * 2 + 1] = uv.v;
            ++i;
        }
    }

    if(_mesh_src.has<data::mesh::attribute::point_colors>())
    {
        vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
        colors->SetNumberOfComponents(static_cast<int>(nb_color_components));
        colors->SetName("Colors");
        colors->SetArray(
            new_colors,
            nb_points * static_cast<vtkIdType>(nb_color_components),
            0,
            vtkUnsignedCharArray::VTK_DATA_ARRAY_DELETE
        );
        _point_set_dst.GetPointData()->SetScalars(colors);
    }
    else if(_point_set_dst.GetPointData()->HasArray("Colors") != 0)
    {
        _point_set_dst.GetPointData()->RemoveArray("Colors");
    }

    if(_mesh_src.has<data::mesh::attribute::point_normals>())
    {
        vtkSmartPointer<vtkFloatArray> normals = vtkSmartPointer<vtkFloatArray>::New();
        normals->SetNumberOfComponents(3);
        normals->SetArray(new_normals, nb_points * 3, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);

        _point_set_dst.GetPointData()->SetNormals(normals);
    }
    else if(_point_set_dst.GetPointData()->GetAttribute(vtkDataSetAttributes::NORMALS) != nullptr)
    {
        _point_set_dst.GetPointData()->RemoveArray(vtkDataSetAttributes::NORMALS);
    }

    if(_mesh_src.has<data::mesh::attribute::point_tex_coords>())
    {
        vtkSmartPointer<vtkFloatArray> tex_coords = vtkSmartPointer<vtkFloatArray>::New();
        tex_coords->SetNumberOfComponents(2);
        tex_coords->SetArray(new_tex_coords, nb_points * 2, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);
        _point_set_dst.GetPointData()->SetTCoords(tex_coords);
    }
    else if(_point_set_dst.GetPointData()->GetAttribute(vtkDataSetAttributes::TCOORDS) != nullptr)
    {
        _point_set_dst.GetPointData()->RemoveArray(vtkDataSetAttributes::TCOORDS);
    }

    data_points->Modified();
    _point_set_dst.Modified();
}

//------------------------------------------------------------------------------

void to_vtk_point_set(const data::mesh& _mesh, vtkPointSet& _dataset)
{
    const vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
    _dataset.SetPoints(pts);

    const auto nb_cells  = _mesh.num_cells();
    const auto dump_lock = _mesh.dump_lock();

    if(nb_cells > 0)
    {
        vtkPolyData* poly_data    = vtkPolyData::SafeDownCast(&_dataset);
        vtkUnstructuredGrid* grid = vtkUnstructuredGrid::SafeDownCast(&_dataset);
        SIGHT_ASSERT(
            "Pointset must be either a vtkPolyData or a vtkUnstructuredGrid",
            (poly_data && !grid) || (!poly_data && grid)
        );

        if(poly_data != nullptr)
        {
            poly_data->Allocate(static_cast<int>(nb_cells));
        }
        else
        {
            grid->Allocate(static_cast<int>(nb_cells));
        }

        unsigned char* new_colors             = nullptr;
        float* new_normals                    = nullptr;
        float* new_tex_coords                 = nullptr;
        const std::size_t nb_color_components = 4;

        if(_mesh.has<data::mesh::attribute::cell_colors>())
        {
            new_colors = new unsigned char [nb_cells * nb_color_components];

            vtkIdType i = 0;
            for(const auto& c : _mesh.crange<data::iterator::cell::rgba>())
            {
                new_colors[i * 4]     = c.r;
                new_colors[i * 4 + 1] = c.g;
                new_colors[i * 4 + 2] = c.b;
                new_colors[i * 4 + 3] = c.a;
                ++i;
            }
        }

        if(_mesh.has<data::mesh::attribute::cell_normals>())
        {
            new_normals = new float [nb_cells * 3LL];
            vtkIdType i = 0;
            for(const auto& n : _mesh.crange<data::iterator::cell::nxyz>())
            {
                new_normals[i * 3]     = n.nx;
                new_normals[i * 3 + 1] = n.ny;
                new_normals[i * 3 + 2] = n.nz;
                ++i;
            }
        }

        if(_mesh.has<data::mesh::attribute::cell_tex_coords>())
        {
            new_tex_coords = new float [nb_cells * 2LL];
            vtkIdType i = 0;
            for(const auto& uv : _mesh.crange<data::iterator::cell::uv>())
            {
                new_tex_coords[i * 2]     = uv.u;
                new_tex_coords[i * 2 + 1] = uv.v;
                ++i;
            }
        }

        auto insert_next_cell = [&poly_data, &grid](auto _type_vtk_cell, auto _i, auto _cell)
                                {
                                    if(poly_data)
                                    {
                                        poly_data->InsertNextCell(_type_vtk_cell, _i, _cell);
                                    }
                                    else
                                    {
                                        grid->InsertNextCell(_type_vtk_cell, _i, _cell);
                                    }
                                };

        int type_vtk_cell = 0;
        std::array<vtkIdType, 4> cell {};
        switch(_mesh.cell_type())
        {
            case data::mesh::cell_type_t::point:
                type_vtk_cell = VTK_VERTEX;
                for(const auto& p : _mesh.crange<data::iterator::cell::point>())
                {
                    cell[0] = static_cast<vtkIdType>(p.pt);
                    insert_next_cell(type_vtk_cell, 1, cell.data());
                }

                break;

            case data::mesh::cell_type_t::line:
                type_vtk_cell = VTK_LINE;

                for(const auto& p : _mesh.crange<data::iterator::cell::line>())
                {
                    cell[0] = static_cast<vtkIdType>(p.pt[0]);
                    cell[1] = static_cast<vtkIdType>(p.pt[1]);
                    insert_next_cell(type_vtk_cell, 2, cell.data());
                }

                break;

            case data::mesh::cell_type_t::triangle:
                type_vtk_cell = VTK_TRIANGLE;

                for(const auto& p : _mesh.crange<data::iterator::cell::triangle>())
                {
                    cell[0] = static_cast<vtkIdType>(p.pt[0]);
                    cell[1] = static_cast<vtkIdType>(p.pt[1]);
                    cell[2] = static_cast<vtkIdType>(p.pt[2]);
                    insert_next_cell(type_vtk_cell, 3, cell.data());
                }

                break;

            case data::mesh::cell_type_t::quad:
                type_vtk_cell = VTK_QUAD;

                for(const auto& p : _mesh.crange<data::iterator::cell::quad>())
                {
                    cell[0] = static_cast<vtkIdType>(p.pt[0]);
                    cell[1] = static_cast<vtkIdType>(p.pt[1]);
                    cell[2] = static_cast<vtkIdType>(p.pt[2]);
                    cell[3] = static_cast<vtkIdType>(p.pt[3]);
                    insert_next_cell(type_vtk_cell, 4, cell.data());
                }

                break;

            case data::mesh::cell_type_t::tetra:
                type_vtk_cell = VTK_TETRA;
                for(const auto& p : _mesh.crange<data::iterator::cell::tetra>())
                {
                    cell[0] = static_cast<vtkIdType>(p.pt[0]);
                    cell[1] = static_cast<vtkIdType>(p.pt[1]);
                    cell[2] = static_cast<vtkIdType>(p.pt[2]);
                    cell[3] = static_cast<vtkIdType>(p.pt[3]);
                    insert_next_cell(type_vtk_cell, 4, cell.data());
                }

                break;

            default:
                SIGHT_THROW("Mesh type not supported.");
        }

        if(_mesh.has<data::mesh::attribute::cell_colors>())
        {
            vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
            colors->SetNumberOfComponents(static_cast<int>(nb_color_components));
            colors->SetName("Colors");
            colors->SetArray(
                new_colors,
                static_cast<vtkIdType>(nb_cells) * static_cast<vtkIdType>(nb_color_components),
                0,
                vtkUnsignedCharArray::VTK_DATA_ARRAY_DELETE
            );
            _dataset.GetCellData()->SetScalars(colors);
        }
        else if(_dataset.GetCellData()->HasArray("Colors") != 0)
        {
            _dataset.GetCellData()->RemoveArray("Colors");
        }

        if(_mesh.has<data::mesh::attribute::cell_normals>())
        {
            vtkSmartPointer<vtkFloatArray> normals = vtkSmartPointer<vtkFloatArray>::New();
            normals->SetNumberOfComponents(3);
            normals->SetArray(
                new_normals,
                static_cast<vtkIdType>(nb_cells) * 3,
                0,
                vtkFloatArray::VTK_DATA_ARRAY_DELETE
            );

            _dataset.GetCellData()->SetNormals(normals);
        }
        else if(_dataset.GetCellData()->GetAttribute(vtkDataSetAttributes::NORMALS) != nullptr)
        {
            _dataset.GetCellData()->RemoveArray(vtkDataSetAttributes::NORMALS);
        }

        if(_mesh.has<data::mesh::attribute::cell_tex_coords>())
        {
            vtkSmartPointer<vtkFloatArray> tex_coords = vtkSmartPointer<vtkFloatArray>::New();
            tex_coords->SetNumberOfComponents(2);
            tex_coords->SetArray(
                new_tex_coords,
                static_cast<vtkIdType>(nb_cells) * 2,
                0,
                vtkFloatArray::VTK_DATA_ARRAY_DELETE
            );
            _dataset.GetCellData()->SetTCoords(tex_coords);
        }
        else if(_dataset.GetCellData()->GetAttribute(vtkDataSetAttributes::TCOORDS) != nullptr)
        {
            _dataset.GetCellData()->RemoveArray(vtkDataSetAttributes::TCOORDS);
        }

        _dataset.Modified();
    }

    update_points_and_attributes(_dataset, _mesh);
}

//------------------------------------------------------------------------------

void mesh::from_vtk_mesh(vtkSmartPointer<vtkPolyData> _poly_data, data::mesh::sptr _mesh)
{
    from_vtk_point_set(*_poly_data, *_mesh);
}

//------------------------------------------------------------------------------

void mesh::from_vtk_grid(vtkSmartPointer<vtkUnstructuredGrid> _grid, data::mesh::sptr _mesh)
{
    from_vtk_point_set(*_grid, *_mesh);
}

//------------------------------------------------------------------------------

void mesh::to_vtk_mesh(const data::mesh::csptr& _mesh, vtkSmartPointer<vtkPolyData> _poly_data)
{
    to_vtk_point_set(*_mesh, *_poly_data);
}

//------------------------------------------------------------------------------

void mesh::to_vtk_grid(const data::mesh::csptr& _mesh, vtkSmartPointer<vtkUnstructuredGrid> _grid)
{
    to_vtk_point_set(*_mesh, *_grid);
}

//-----------------------------------------------------------------------------

double mesh::compute_volume(const data::mesh::csptr& _mesh)
{
    vtkSmartPointer<vtkPolyData> vtk_mesh_raw = vtkSmartPointer<vtkPolyData>::New();
    mesh::to_vtk_mesh(_mesh, vtk_mesh_raw);

    // identify and fill holes in meshes
    vtkSmartPointer<vtkFillHolesFilter> holes_filter = vtkSmartPointer<vtkFillHolesFilter>::New();
    holes_filter->SetHoleSize(2000);
    holes_filter->SetInputData(vtk_mesh_raw);
    holes_filter->Update();
    if(holes_filter->GetOutput()->GetNumberOfCells() > 0) // filter return empty mesh when no topological holes are
                                                          // present
    {
        vtk_mesh_raw = holes_filter->GetOutput();
    }

    // compute normals for polygonal mesh
    const vtkSmartPointer<vtkPolyDataNormals> filter = vtkSmartPointer<vtkPolyDataNormals>::New();
    filter->SetInputData(vtk_mesh_raw);
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
