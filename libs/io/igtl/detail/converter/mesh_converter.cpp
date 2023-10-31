/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

// cspell:ignore NOLINT

#include "io/igtl/detail/converter/mesh_converter.hpp"

#include "io/igtl/detail/data_converter.hpp"

#include <data/array.hpp>
#include <data/mesh.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <igtlPolyDataMessage.h>

#include <algorithm>

namespace sight::io::igtl::detail::converter
{

const std::string mesh_converter::IGTL_TYPE          = "POLYDATA";
const std::string mesh_converter::FWDATA_OBJECT_TYPE = data::mesh::classname();

CONVERTER_REGISTER_MACRO(io::igtl::detail::converter::mesh_converter);

mesh_converter::mesh_converter()
= default;

//-----------------------------------------------------------------------------

mesh_converter::~mesh_converter()
= default;

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer mesh_converter::from_fw_data_object(data::object::csptr _src) const
{
    data::mesh::csptr mesh_src = std::dynamic_pointer_cast<const data::mesh>(_src);

    ::igtl::PolyDataMessage::Pointer dest = ::igtl::PolyDataMessage::New();
    sight::io::igtl::detail::converter::mesh_converter::copy_cells_from_fw_mesh(mesh_src, dest);
    sight::io::igtl::detail::converter::mesh_converter::copy_points_from_fw_mesh(mesh_src, dest);
    sight::io::igtl::detail::converter::mesh_converter::copy_attributes_from_fw_mesh(mesh_src, dest);
    return ::igtl::MessageBase::Pointer(dest); // NOLINT(modernize-return-braced-init-list)
}

//-----------------------------------------------------------------------------

void mesh_converter::copy_cells_from_fw_mesh(
    data::mesh::csptr _mesh_src,
    ::igtl::PolyDataMessage::Pointer _dest
)
{
    const auto dump_lock = _mesh_src->dump_lock();

    _dest->SetLines(::igtl::PolyDataCellArray::New());
    _dest->SetTriangleStrips(::igtl::PolyDataCellArray::New());
    _dest->SetVertices(::igtl::PolyDataCellArray::New());

    std::array<igtlUint32, 5> cell {};

    const data::mesh::cell_type_t cell_type = _mesh_src->cell_type();

    switch(cell_type)
    {
        case data::mesh::cell_type_t::line:

            for(const auto& cell_itr : _mesh_src->crange<data::iterator::cell::line>())
            {
                cell[0] = cell_itr.pt[0];
                cell[1] = cell_itr.pt[1];
                _dest->GetLines()->AddCell(2, cell.data());
            }

            break;

        case data::mesh::cell_type_t::triangle:
            for(const auto& cell_itr : _mesh_src->crange<data::iterator::cell::triangle>())
            {
                cell[0] = cell_itr.pt[0];
                cell[1] = cell_itr.pt[1];
                cell[2] = cell_itr.pt[2];
                _dest->GetTriangleStrips()->AddCell(3, cell.data());
            }

            break;

        case data::mesh::cell_type_t::quad:

            for(const auto& cell_itr : _mesh_src->crange<data::iterator::cell::quad>())
            {
                cell[0] = cell_itr.pt[0];
                cell[1] = cell_itr.pt[1];
                cell[2] = cell_itr.pt[2];
                cell[3] = cell_itr.pt[3];
                _dest->GetVertices()->AddCell(4, cell.data());
            }

            break;

        case data::mesh::cell_type_t::tetra:
            for(const auto& cell_itr : _mesh_src->crange<data::iterator::cell::tetra>())
            {
                cell[0] = cell_itr.pt[0];
                cell[1] = cell_itr.pt[1];
                cell[2] = cell_itr.pt[2];
                cell[3] = cell_itr.pt[3];
                _dest->GetVertices()->AddCell(4, cell.data());
            }

            break;

        default:
            SIGHT_ERROR("This type of cell is not managed: " << static_cast<std::uint8_t>(cell_type));
    }
}

//-----------------------------------------------------------------------------

void mesh_converter::copy_points_from_fw_mesh(
    data::mesh::csptr _mesh_src,
    ::igtl::PolyDataMessage::Pointer _dest
)
{
    const auto dump_lock = _mesh_src->dump_lock();

    _dest->SetPoints(::igtl::PolyDataPointArray::New().GetPointer());
    for(const auto& p : _mesh_src->crange<data::iterator::point::xyz>())
    {
        _dest->GetPoints()->AddPoint(p.x, p.y, p.z);
    }
}

//-----------------------------------------------------------------------------

void mesh_converter::copy_attributes_from_fw_mesh(
    data::mesh::csptr _mesh_src,
    ::igtl::PolyDataMessage::Pointer _dest
)
{
    const auto dump_lock = _mesh_src->dump_lock();

    const std::size_t number_of_points = _mesh_src->num_points();
    const std::size_t number_of_cells  = _mesh_src->num_cells();

    _dest->ClearAttributes();

    // point attributes
    if(_mesh_src->has<data::mesh::attribute::point_colors>())
    {
        auto* igtl_data_point_color = new igtlFloat32[4 * number_of_points];

        std::size_t i = 0;
        for(const auto& color : _mesh_src->crange<data::iterator::point::rgba>())
        {
            igtl_data_point_color[4 * i]     = static_cast<float>(color.r) / 255.F;
            igtl_data_point_color[4 * i + 1] = static_cast<float>(color.g) / 255.F;
            igtl_data_point_color[4 * i + 2] = static_cast<float>(color.b) / 255.F;
            igtl_data_point_color[4 * i + 3] = static_cast<float>(color.a) / 255.F;
            ++i;
        }

        ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
        attr->SetType(::igtl::PolyDataAttribute::POINT_RGBA);
        attr->SetName("PointColors");
        attr->SetSize(static_cast<igtlUint32>(number_of_points));
        attr->SetData(igtl_data_point_color);
        _dest->AddAttribute(attr);
        delete[] igtl_data_point_color;
    }

    if(_mesh_src->has<data::mesh::attribute::point_normals>())
    {
        auto* igtl_data_point_normal = new igtlFloat32[3 * number_of_points];

        std::size_t i = 0;
        for(const auto& normal : _mesh_src->crange<data::iterator::point::nxyz>())
        {
            igtl_data_point_normal[3 * i]     = normal.nx;
            igtl_data_point_normal[3 * i + 1] = normal.ny;
            igtl_data_point_normal[3 * i + 2] = normal.nz;
            ++i;
        }

        ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
        attr->SetType(::igtl::PolyDataAttribute::POINT_NORMAL);
        attr->SetName("PointNormals");
        attr->SetSize(static_cast<igtlUint32>(number_of_points));
        attr->SetData(igtl_data_point_normal);
        _dest->AddAttribute(attr);
        delete[] igtl_data_point_normal;
    }

    if(_mesh_src->has<data::mesh::attribute::point_tex_coords>())
    {
        auto* igtl_data_point_tex = new igtlFloat32[2 * number_of_points];

        std::size_t i = 0;
        for(const auto& tex : _mesh_src->crange<data::iterator::point::uv>())
        {
            igtl_data_point_tex[2 * i]     = tex.u;
            igtl_data_point_tex[2 * i + 1] = tex.v;
            ++i;
        }

        ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
        attr->SetName("PointTexCoord");
        attr->SetType(::igtl::PolyDataAttribute::POINT_VECTOR);
        attr->SetSize(igtlUint32(number_of_points));
        attr->SetData(igtl_data_point_tex);
        _dest->AddAttribute(attr);
        delete[] igtl_data_point_tex;
    }

    // cell attributes
    if(_mesh_src->has<data::mesh::attribute::cell_colors>())
    {
        auto* igtl_data_cell_color = new igtlFloat32[4 * number_of_cells];

        std::size_t i = 0;
        for(const auto& color : _mesh_src->crange<data::iterator::cell::rgba>())
        {
            igtl_data_cell_color[4 * i]     = static_cast<float>(color.r) / 255.F;
            igtl_data_cell_color[4 * i + 1] = static_cast<float>(color.g) / 255.F;
            igtl_data_cell_color[4 * i + 2] = static_cast<float>(color.b) / 255.F;
            igtl_data_cell_color[4 * i + 3] = static_cast<float>(color.a) / 255.F;
            ++i;
        }

        ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
        attr->SetType(::igtl::PolyDataAttribute::CELL_RGBA);
        attr->SetName("CellColors");
        attr->SetSize(static_cast<igtlUint32>(number_of_cells));
        attr->SetData(igtl_data_cell_color);
        _dest->AddAttribute(attr);
        delete[] igtl_data_cell_color;
    }

    if(_mesh_src->has<data::mesh::attribute::cell_normals>())
    {
        auto* igtl_data_cell_normal = new igtlFloat32[3 * number_of_cells];

        std::size_t i = 0;
        for(const auto& normal : _mesh_src->crange<data::iterator::cell::nxyz>())
        {
            igtl_data_cell_normal[3 * i]     = normal.nx;
            igtl_data_cell_normal[3 * i + 1] = normal.ny;
            igtl_data_cell_normal[3 * i + 2] = normal.nz;
            ++i;
        }

        ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
        attr->SetType(::igtl::PolyDataAttribute::CELL_NORMAL);
        attr->SetName("CellNormals");
        attr->SetSize(static_cast<igtlUint32>(number_of_cells));
        attr->SetData(igtl_data_cell_normal);
        _dest->AddAttribute(attr);
        delete[] igtl_data_cell_normal;
    }

    if(_mesh_src->has<data::mesh::attribute::cell_tex_coords>())
    {
        auto* igtl_data_cell_tex = new igtlFloat32[2 * number_of_cells];

        std::size_t i = 0;
        for(const auto& tex : _mesh_src->crange<data::iterator::cell::uv>())
        {
            igtl_data_cell_tex[2 * i]     = tex.u;
            igtl_data_cell_tex[2 * i + 1] = tex.v;
            ++i;
        }

        ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
        attr->SetName("CellTexCoord");
        attr->SetType(::igtl::PolyDataAttribute::CELL_VECTOR);
        attr->SetSize(igtlUint32(number_of_cells));
        attr->SetData(igtl_data_cell_tex);
        _dest->AddAttribute(attr);
        delete[] igtl_data_cell_tex;
    }
}

//-----------------------------------------------------------------------------

data::object::sptr mesh_converter::from_igtl_message(const ::igtl::MessageBase::Pointer _src) const
{
    std::array<igtlFloat32, 3> point {};
    std::array<igtlUint32, 5> cell {};

    auto* msg                                 = dynamic_cast< ::igtl::PolyDataMessage*>(_src.GetPointer());
    ::igtl::PolyDataMessage::Pointer mesh_msg = ::igtl::PolyDataMessage::Pointer(msg);
    data::mesh::sptr mesh                     = std::make_shared<data::mesh>();

    const int number_of_points = mesh_msg->GetPoints()->GetNumberOfPoints();

    data::mesh::attribute attributes = data::mesh::attribute::none;
    for(unsigned int i = 0 ; int(i) < mesh_msg->GetNumberOfAttributes() ; ++i)
    {
        const ::igtl::PolyDataAttribute::Pointer attr = mesh_msg->GetAttribute(i);
        switch(attr->GetType())
        {
            case ::igtl::PolyDataAttribute::POINT_RGBA:
                attributes = attributes | data::mesh::attribute::point_colors;
                break;

            case ::igtl::PolyDataAttribute::CELL_RGBA:
                attributes = attributes | data::mesh::attribute::cell_colors;
                break;

            case ::igtl::PolyDataAttribute::POINT_NORMAL:
                attributes = attributes | data::mesh::attribute::point_normals;
                break;

            case ::igtl::PolyDataAttribute::CELL_NORMAL:
                attributes = attributes | data::mesh::attribute::cell_normals;
                break;

            case ::igtl::PolyDataAttribute::POINT_VECTOR:
                attributes = attributes | data::mesh::attribute::point_tex_coords;
                break;

            case ::igtl::PolyDataAttribute::CELL_VECTOR:
                attributes = attributes | data::mesh::attribute::cell_tex_coords;
                break;

            default:
                SIGHT_ERROR("This type of cell is not managed: " << static_cast<std::uint8_t>(attr->GetType()));
        }
    }

    data::mesh::cell_type_t cell_type = data::mesh::cell_type_t::size;
    igtlUint32 number_of_cells        = 0;
    if(mesh_msg->GetLines()->GetNumberOfCells() > 0)
    {
        cell_type       = data::mesh::cell_type_t::line;
        number_of_cells = mesh_msg->GetLines()->GetNumberOfCells();
    }
    else if(mesh_msg->GetTriangleStrips()->GetNumberOfCells() > 0)
    {
        cell_type       = data::mesh::cell_type_t::triangle;
        number_of_cells = mesh_msg->GetTriangleStrips()->GetNumberOfCells();
    }
    else if(mesh_msg->GetVertices()->GetNumberOfCells() > 0)
    {
        cell_type       = data::mesh::cell_type_t::quad;
        number_of_cells = mesh_msg->GetVertices()->GetNumberOfCells();
    }

    if(number_of_cells == 0)
    {
        number_of_cells = igtlUint32(number_of_points);
    }

    mesh->reserve(data::mesh::size_t(number_of_points), number_of_cells, cell_type, attributes);
    const auto dump_lock = mesh->dump_lock();

    ::igtl::PolyDataPointArray* points = mesh_msg->GetPoints();
    auto nb_points                     = static_cast<unsigned int>(points->GetNumberOfPoints());
    for(unsigned int i = 0 ; i < nb_points ; ++i)
    {
        points->GetPoint(i, point.data());
        mesh->push_point(point[0], point[1], point[2]);
    }

    if(cell_type == data::mesh::cell_type_t::line)
    {
        for(unsigned int i = 0 ; i < mesh_msg->GetLines()->GetNumberOfCells() ; ++i)
        {
            mesh_msg->GetLines()->GetCell(i, cell.data());
            mesh->push_cell(cell[0], cell[1]);
        }
    }
    else if(cell_type == data::mesh::cell_type_t::triangle)
    {
        for(unsigned int i = 0 ; i < mesh_msg->GetTriangleStrips()->GetNumberOfCells() ; ++i)
        {
            mesh_msg->GetTriangleStrips()->GetCell(i, cell.data());
            mesh->push_cell(cell[0], cell[1], cell[2]);
        }
    }
    else if(cell_type == data::mesh::cell_type_t::quad)
    {
        for(unsigned int i = 0 ; i < mesh_msg->GetVertices()->GetNumberOfCells() ; ++i)
        {
            mesh_msg->GetVertices()->GetCell(i, cell.data());
            mesh->push_cell(cell[0], cell[1], cell[2], cell[3]);
        }
    }

    mesh->shrink_to_fit();

    sight::io::igtl::detail::converter::mesh_converter::copy_attribute_from_poly_data(mesh_msg, mesh);

    return mesh;
}

//-----------------------------------------------------------------------------

void mesh_converter::copy_attribute_from_poly_data(
    ::igtl::PolyDataMessage::Pointer _src,
    data::mesh::sptr _dest
)
{
    ::igtl::PolyDataAttribute::Pointer attr;

    for(unsigned int i = 0 ; int(i) < _src->GetNumberOfAttributes() ; ++i)
    {
        attr = _src->GetAttribute(i);
        switch(attr->GetType())
        {
            case ::igtl::PolyDataAttribute::POINT_RGBA:
            {
                auto* data = new igtlFloat32[static_cast<std::size_t>(attr->GetSize()) * attr->GetNumberOfComponents()];
                attr->GetData(data);

                std::size_t k = 0;
                for(unsigned int j = 0 ; j < attr->GetSize() ; ++j)
                {
                    _dest->set_point_color(
                        j,
                        static_cast<data::mesh::color_t>(data[k] * 255.F),
                        static_cast<data::mesh::color_t>(data[k + 1] * 255.F),
                        static_cast<data::mesh::color_t>(data[k + 2] * 255.F),
                        static_cast<data::mesh::color_t>(data[k + 3] * 255.F)
                    );
                    k += attr->GetNumberOfComponents();
                }

                attr->Clear();
                delete[] data;
                break;
            }

            case ::igtl::PolyDataAttribute::CELL_RGBA:
            {
                auto* data = new igtlFloat32[static_cast<std::size_t>(attr->GetSize()) * attr->GetNumberOfComponents()];
                attr->GetData(data);

                std::size_t k = 0;
                for(unsigned int j = 0 ; j < attr->GetSize() ; ++j)
                {
                    _dest->set_cell_color(
                        j,
                        static_cast<data::mesh::color_t>(data[k] * 255.F),
                        static_cast<data::mesh::color_t>(data[k + 1] * 255.F),
                        static_cast<data::mesh::color_t>(data[k + 2] * 255.F),
                        static_cast<data::mesh::color_t>(data[k + 3] * 255.F)
                    );
                    k += attr->GetNumberOfComponents();
                }

                attr->Clear();
                delete[] data;
                break;
            }

            case ::igtl::PolyDataAttribute::POINT_NORMAL:
            {
                auto* data = new igtlFloat32[static_cast<std::size_t>(attr->GetSize()) * attr->GetNumberOfComponents()];
                attr->GetData(data);
                for(unsigned int j = 0 ; j < _dest->num_points() ; ++j)
                {
                    _dest->set_point_normal(j, data[j * 3LL], data[j * 3 + 1], data[j * 3 + 2]);
                }

                attr->Clear();
                delete[] data;
                break;
            }

            case ::igtl::PolyDataAttribute::CELL_NORMAL:
            {
                auto* data = new igtlFloat32[static_cast<std::size_t>(attr->GetSize()) * attr->GetNumberOfComponents()];
                attr->GetData(data);
                for(unsigned int j = 0 ; j < _dest->num_cells() ; ++j)
                {
                    _dest->set_cell_normal(j, data[j * 3LL], data[j * 3 + 1], data[j * 3 + 2]);
                }

                attr->Clear();
                delete[] data;
                break;
            }

            case ::igtl::PolyDataAttribute::POINT_VECTOR:
            {
                auto* data = new igtlFloat32[static_cast<std::size_t>(attr->GetSize()) * attr->GetNumberOfComponents()];
                attr->GetData(data);
                for(unsigned int j = 0 ; j < _dest->num_points() ; ++j)
                {
                    _dest->set_point_tex_coord(j, data[j * 2LL], data[j * 2 + 1]);
                }

                attr->Clear();
                delete[] data;
                break;
            }

            case ::igtl::PolyDataAttribute::CELL_VECTOR:
            {
                auto* data = new igtlFloat32[static_cast<std::size_t>(attr->GetSize()) * attr->GetNumberOfComponents()];
                attr->GetData(data);
                for(unsigned int j = 0 ; j < _dest->num_cells() ; ++j)
                {
                    _dest->set_cell_tex_coord(j, data[j * 2LL], data[j * 2 + 1]);
                }

                attr->Clear();
                delete[] data;
                break;
            }

            default:
                break;
        }
    }
}

//-----------------------------------------------------------------------------

base::sptr mesh_converter::New()
{
    return std::make_shared<mesh_converter>();
}

//-----------------------------------------------------------------------------

std::string const& mesh_converter::get_igtl_type() const
{
    return mesh_converter::IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& mesh_converter::get_fw_data_object_type() const
{
    return mesh_converter::FWDATA_OBJECT_TYPE;
}

} // namespace sight::io::igtl::detail::converter
