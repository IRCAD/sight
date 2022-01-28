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

#include "io/igtl/detail/converter/MeshConverter.hpp"

#include "io/igtl/detail/DataConverter.hpp"

#include <data/Array.hpp>
#include <data/Mesh.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <igtlPolyDataMessage.h>

#include <algorithm>

namespace sight::io::igtl::detail
{

namespace converter
{

const std::string MeshConverter::s_IGTL_TYPE          = "POLYDATA";
const std::string MeshConverter::s_FWDATA_OBJECT_TYPE = data::Mesh::classname();

converterRegisterMacro(io::igtl::detail::converter::MeshConverter);

MeshConverter::MeshConverter()
{
}

//-----------------------------------------------------------------------------

MeshConverter::~MeshConverter()
{
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer MeshConverter::fromFwDataObject(data::Object::csptr src) const
{
    data::Mesh::csptr meshSrc = data::Mesh::dynamicConstCast(src);

    ::igtl::PolyDataMessage::Pointer dest = ::igtl::PolyDataMessage::New();
    this->copyCellsFromFwMesh(meshSrc, dest);
    this->copyPointsFromFwMesh(meshSrc, dest);
    this->copyAttributesFromFwMesh(meshSrc, dest);
    return ::igtl::MessageBase::Pointer(dest);
}

//-----------------------------------------------------------------------------

void MeshConverter::copyCellsFromFwMesh(data::Mesh::csptr meshSrc, ::igtl::PolyDataMessage::Pointer dest) const
{
    const auto dumpLock = meshSrc->dump_lock();

    dest->SetLines(::igtl::PolyDataCellArray::New());
    dest->SetTriangleStrips(::igtl::PolyDataCellArray::New());
    dest->SetVertices(::igtl::PolyDataCellArray::New());

    igtlUint32 cell[5];

    const data::Mesh::CellType cellType = meshSrc->getCellType();

    switch(cellType)
    {
        case data::Mesh::CellType::LINE:

            for(const auto& cellItr : meshSrc->crange<data::iterator::cell::line>())
            {
                cell[0] = cellItr.pt[0];
                cell[1] = cellItr.pt[1];
                dest->GetLines()->AddCell(2, cell);
            }

            break;

        case data::Mesh::CellType::TRIANGLE:
            for(const auto& cellItr : meshSrc->crange<data::iterator::cell::triangle>())
            {
                cell[0] = cellItr.pt[0];
                cell[1] = cellItr.pt[1];
                cell[2] = cellItr.pt[2];
                dest->GetTriangleStrips()->AddCell(3, cell);
            }

            break;

        case data::Mesh::CellType::QUAD:

            for(const auto& cellItr : meshSrc->crange<data::iterator::cell::quad>())
            {
                cell[0] = cellItr.pt[0];
                cell[1] = cellItr.pt[1];
                cell[2] = cellItr.pt[2];
                cell[3] = cellItr.pt[3];
                dest->GetVertices()->AddCell(4, cell);
            }

            break;

        case data::Mesh::CellType::TETRA:
            for(const auto& cellItr : meshSrc->crange<data::iterator::cell::tetra>())
            {
                cell[0] = cellItr.pt[0];
                cell[1] = cellItr.pt[1];
                cell[2] = cellItr.pt[2];
                cell[3] = cellItr.pt[3];
                dest->GetVertices()->AddCell(4, cell);
            }

            break;

        default:
            SIGHT_ERROR("This type of cell is not managed: " << static_cast<std::uint8_t>(cellType));
    }
}

//-----------------------------------------------------------------------------

void MeshConverter::copyPointsFromFwMesh(data::Mesh::csptr meshSrc, ::igtl::PolyDataMessage::Pointer dest) const
{
    const auto dumpLock = meshSrc->dump_lock();

    dest->SetPoints(::igtl::PolyDataPointArray::New().GetPointer());
    for(const auto& p : meshSrc->crange<data::iterator::point::xyz>())
    {
        dest->GetPoints()->AddPoint(p.x, p.y, p.z);
    }
}

//-----------------------------------------------------------------------------

void MeshConverter::copyAttributesFromFwMesh(
    data::Mesh::csptr meshSrc,
    ::igtl::PolyDataMessage::Pointer dest
) const
{
    const auto dumpLock = meshSrc->dump_lock();

    const std::size_t numberOfPoints = meshSrc->numPoints();
    const std::size_t numberOfCells  = meshSrc->numCells();

    dest->ClearAttributes();

    // point attributes
    if(meshSrc->has<data::Mesh::Attributes::POINT_COLORS>())
    {
        igtlFloat32* igtlDataPointColor = new igtlFloat32[4 * numberOfPoints];

        std::size_t i = 0;
        for(const auto& color : meshSrc->crange<data::iterator::point::rgba>())
        {
            igtlDataPointColor[4 * i]     = static_cast<float>(color.r) / 255.f;
            igtlDataPointColor[4 * i + 1] = static_cast<float>(color.g) / 255.f;
            igtlDataPointColor[4 * i + 2] = static_cast<float>(color.b) / 255.f;
            igtlDataPointColor[4 * i + 3] = static_cast<float>(color.a) / 255.f;
            ++i;
        }

        ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
        attr->SetType(::igtl::PolyDataAttribute::POINT_RGBA);
        attr->SetName("PointColors");
        attr->SetSize(static_cast<igtlUint32>(numberOfPoints));
        attr->SetData(igtlDataPointColor);
        dest->AddAttribute(attr);
        delete[] igtlDataPointColor;
    }

    if(meshSrc->has<data::Mesh::Attributes::POINT_NORMALS>())
    {
        igtlFloat32* igtlDataPointNormal = new igtlFloat32[3 * numberOfPoints];

        std::size_t i = 0;
        for(const auto& normal : meshSrc->crange<data::iterator::point::nxyz>())
        {
            igtlDataPointNormal[3 * i]     = normal.nx;
            igtlDataPointNormal[3 * i + 1] = normal.ny;
            igtlDataPointNormal[3 * i + 2] = normal.nz;
            ++i;
        }

        ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
        attr->SetType(::igtl::PolyDataAttribute::POINT_NORMAL);
        attr->SetName("PointNormals");
        attr->SetSize(static_cast<igtlUint32>(numberOfPoints));
        attr->SetData(igtlDataPointNormal);
        dest->AddAttribute(attr);
        delete[] igtlDataPointNormal;
    }

    if(meshSrc->has<data::Mesh::Attributes::POINT_TEX_COORDS>())
    {
        igtlFloat32* igtlDataPointTex = new igtlFloat32[2 * numberOfPoints];

        std::size_t i = 0;
        for(const auto& tex : meshSrc->crange<data::iterator::point::uv>())
        {
            igtlDataPointTex[2 * i]     = tex.u;
            igtlDataPointTex[2 * i + 1] = tex.v;
            ++i;
        }

        ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
        attr->SetName("PointTexCoord");
        attr->SetType(::igtl::PolyDataAttribute::POINT_VECTOR);
        attr->SetSize(numberOfPoints);
        attr->SetData(igtlDataPointTex);
        dest->AddAttribute(attr);
        delete[] igtlDataPointTex;
    }

    // cell attributes
    if(meshSrc->has<data::Mesh::Attributes::CELL_COLORS>())
    {
        igtlFloat32* igtlDataCellColor = new igtlFloat32[4 * numberOfCells];

        std::size_t i = 0;
        for(const auto& color : meshSrc->crange<data::iterator::cell::rgba>())
        {
            igtlDataCellColor[4 * i]     = static_cast<float>(color.r) / 255.f;
            igtlDataCellColor[4 * i + 1] = static_cast<float>(color.g) / 255.f;
            igtlDataCellColor[4 * i + 2] = static_cast<float>(color.b) / 255.f;
            igtlDataCellColor[4 * i + 3] = static_cast<float>(color.a) / 255.f;
            ++i;
        }

        ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
        attr->SetType(::igtl::PolyDataAttribute::CELL_RGBA);
        attr->SetName("CellColors");
        attr->SetSize(static_cast<igtlUint32>(numberOfCells));
        attr->SetData(igtlDataCellColor);
        dest->AddAttribute(attr);
        delete[] igtlDataCellColor;
    }

    if(meshSrc->has<data::Mesh::Attributes::CELL_NORMALS>())
    {
        igtlFloat32* igtlDataCellNormal = new igtlFloat32[3 * numberOfCells];

        std::size_t i = 0;
        for(const auto& normal : meshSrc->crange<data::iterator::cell::nxyz>())
        {
            igtlDataCellNormal[3 * i]     = normal.nx;
            igtlDataCellNormal[3 * i + 1] = normal.ny;
            igtlDataCellNormal[3 * i + 2] = normal.nz;
            ++i;
        }

        ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
        attr->SetType(::igtl::PolyDataAttribute::CELL_NORMAL);
        attr->SetName("CellNormals");
        attr->SetSize(static_cast<igtlUint32>(numberOfCells));
        attr->SetData(igtlDataCellNormal);
        dest->AddAttribute(attr);
        delete[] igtlDataCellNormal;
    }

    if(meshSrc->has<data::Mesh::Attributes::CELL_TEX_COORDS>())
    {
        igtlFloat32* igtlDataCellTex = new igtlFloat32[2 * numberOfCells];

        std::size_t i = 0;
        for(const auto& tex : meshSrc->crange<data::iterator::cell::uv>())
        {
            igtlDataCellTex[2 * i]     = tex.u;
            igtlDataCellTex[2 * i + 1] = tex.v;
            ++i;
        }

        ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
        attr->SetName("CellTexCoord");
        attr->SetType(::igtl::PolyDataAttribute::CELL_VECTOR);
        attr->SetSize(numberOfCells);
        attr->SetData(igtlDataCellTex);
        dest->AddAttribute(attr);
        delete[] igtlDataCellTex;
    }
}

//-----------------------------------------------------------------------------

data::Object::sptr MeshConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const
{
    igtlFloat32 point[3];
    igtlUint32 cell[5];

    ::igtl::PolyDataMessage* msg             = dynamic_cast< ::igtl::PolyDataMessage*>(src.GetPointer());
    ::igtl::PolyDataMessage::Pointer meshMsg = ::igtl::PolyDataMessage::Pointer(msg);
    data::Mesh::sptr mesh                    = data::Mesh::New();

    const int numberOfPoints = meshMsg->GetPoints()->GetNumberOfPoints();

    data::Mesh::Attributes attributes = data::Mesh::Attributes::NONE;
    for(int i = 0 ; i < meshMsg->GetNumberOfAttributes() ; ++i)
    {
        const ::igtl::PolyDataAttribute::Pointer attr = meshMsg->GetAttribute(i);
        switch(attr->GetType())
        {
            case ::igtl::PolyDataAttribute::POINT_RGBA:
                attributes = attributes | data::Mesh::Attributes::POINT_COLORS;
                break;

            case ::igtl::PolyDataAttribute::CELL_RGBA:
                attributes = attributes | data::Mesh::Attributes::CELL_COLORS;
                break;

            case ::igtl::PolyDataAttribute::POINT_NORMAL:
                attributes = attributes | data::Mesh::Attributes::POINT_NORMALS;
                break;

            case ::igtl::PolyDataAttribute::CELL_NORMAL:
                attributes = attributes | data::Mesh::Attributes::CELL_NORMALS;
                break;

            case ::igtl::PolyDataAttribute::POINT_VECTOR:
                attributes = attributes | data::Mesh::Attributes::POINT_TEX_COORDS;
                break;

            case ::igtl::PolyDataAttribute::CELL_VECTOR:
                attributes = attributes | data::Mesh::Attributes::CELL_TEX_COORDS;
                break;
        }
    }

    data::Mesh::CellType cellType = data::Mesh::CellType::_SIZE;
    igtlUint32 numberOfCells      = 0;
    if(meshMsg->GetLines()->GetNumberOfCells() > 0)
    {
        cellType      = data::Mesh::CellType::LINE;
        numberOfCells = meshMsg->GetLines()->GetNumberOfCells();
    }
    else if(meshMsg->GetTriangleStrips()->GetNumberOfCells() > 0)
    {
        cellType      = data::Mesh::CellType::TRIANGLE;
        numberOfCells = meshMsg->GetTriangleStrips()->GetNumberOfCells();
    }
    else if(meshMsg->GetVertices()->GetNumberOfCells() > 0)
    {
        cellType      = data::Mesh::CellType::QUAD;
        numberOfCells = meshMsg->GetVertices()->GetNumberOfCells();
    }

    if(numberOfCells == 0)
    {
        numberOfCells = numberOfPoints;
    }

    mesh->reserve(numberOfPoints, numberOfCells, cellType, attributes);
    const auto dumpLock = mesh->dump_lock();

    ::igtl::PolyDataPointArray* points = meshMsg->GetPoints();
    unsigned int nbPoints              = static_cast<unsigned int>(points->GetNumberOfPoints());
    for(unsigned int i = 0 ; i < nbPoints ; ++i)
    {
        points->GetPoint(i, point);
        mesh->pushPoint(point[0], point[1], point[2]);
    }

    if(cellType == data::Mesh::CellType::LINE)
    {
        for(unsigned int i = 0 ; i < meshMsg->GetLines()->GetNumberOfCells() ; ++i)
        {
            meshMsg->GetLines()->GetCell(i, cell);
            mesh->pushCell(cell[0], cell[1]);
        }
    }
    else if(cellType == data::Mesh::CellType::TRIANGLE)
    {
        for(unsigned int i = 0 ; i < meshMsg->GetTriangleStrips()->GetNumberOfCells() ; ++i)
        {
            meshMsg->GetTriangleStrips()->GetCell(i, cell);
            mesh->pushCell(cell[0], cell[1], cell[2]);
        }
    }
    else if(cellType == data::Mesh::CellType::QUAD)
    {
        for(unsigned int i = 0 ; i < meshMsg->GetVertices()->GetNumberOfCells() ; ++i)
        {
            meshMsg->GetVertices()->GetCell(i, cell);
            mesh->pushCell(cell[0], cell[1], cell[2], cell[3]);
        }
    }

    mesh->shrinkToFit();

    this->copyAttributeFromPolyData(meshMsg, mesh);

    return mesh;
}

//-----------------------------------------------------------------------------

void MeshConverter::copyAttributeFromPolyData(::igtl::PolyDataMessage::Pointer src, data::Mesh::sptr dest) const
{
    ::igtl::PolyDataAttribute::Pointer attr;

    for(int i = 0 ; i < src->GetNumberOfAttributes() ; ++i)
    {
        attr = src->GetAttribute(i);
        switch(attr->GetType())
        {
            case ::igtl::PolyDataAttribute::POINT_RGBA:
            {
                igtlFloat32* data = new igtlFloat32[attr->GetSize() * attr->GetNumberOfComponents()];
                attr->GetData(data);

                std::size_t k = 0;
                for(unsigned int j = 0 ; j < attr->GetSize() ; ++j)
                {
                    dest->setPointColor(
                        j,
                        static_cast<data::Mesh::color_t>(data[k] * 255.f),
                        static_cast<data::Mesh::color_t>(data[k + 1] * 255.f),
                        static_cast<data::Mesh::color_t>(data[k + 2] * 255.f),
                        static_cast<data::Mesh::color_t>(data[k + 3] * 255.f)
                    );
                    k += attr->GetNumberOfComponents();
                }

                attr->Clear();
                delete[] data;
                break;
            }

            case ::igtl::PolyDataAttribute::CELL_RGBA:
            {
                igtlFloat32* data = new igtlFloat32[attr->GetSize() * attr->GetNumberOfComponents()];
                attr->GetData(data);

                std::size_t k = 0;
                for(unsigned int j = 0 ; j < attr->GetSize() ; ++j)
                {
                    dest->setCellColor(
                        j,
                        static_cast<data::Mesh::color_t>(data[k] * 255.f),
                        static_cast<data::Mesh::color_t>(data[k + 1] * 255.f),
                        static_cast<data::Mesh::color_t>(data[k + 2] * 255.f),
                        static_cast<data::Mesh::color_t>(data[k + 3] * 255.f)
                    );
                    k += attr->GetNumberOfComponents();
                }

                attr->Clear();
                delete[] data;
                break;
            }

            case ::igtl::PolyDataAttribute::POINT_NORMAL:
            {
                igtlFloat32* data = new igtlFloat32[attr->GetSize() * attr->GetNumberOfComponents()];
                attr->GetData(data);
                for(unsigned int j = 0 ; j < dest->numPoints() ; ++j)
                {
                    dest->setPointNormal(j, data[j * 3], data[j * 3 + 1], data[j * 3 + 2]);
                }

                attr->Clear();
                delete[] data;
                break;
            }

            case ::igtl::PolyDataAttribute::CELL_NORMAL:
            {
                igtlFloat32* data = new igtlFloat32[attr->GetSize() * attr->GetNumberOfComponents()];
                attr->GetData(data);
                for(unsigned int j = 0 ; j < dest->numCells() ; ++j)
                {
                    dest->setCellNormal(j, data[j * 3], data[j * 3 + 1], data[j * 3 + 2]);
                }

                attr->Clear();
                delete[] data;
                break;
            }

            case ::igtl::PolyDataAttribute::POINT_VECTOR:
            {
                igtlFloat32* data = new igtlFloat32[attr->GetSize() * attr->GetNumberOfComponents()];
                attr->GetData(data);
                for(unsigned int j = 0 ; j < dest->numPoints() ; ++j)
                {
                    dest->setPointTexCoord(j, data[j * 2], data[j * 2 + 1]);
                }

                attr->Clear();
                delete[] data;
                break;
            }

            case ::igtl::PolyDataAttribute::CELL_VECTOR:
            {
                igtlFloat32* data = new igtlFloat32[attr->GetSize() * attr->GetNumberOfComponents()];
                attr->GetData(data);
                for(unsigned int j = 0 ; j < dest->numCells() ; ++j)
                {
                    dest->setCellTexCoord(j, data[j * 2], data[j * 2 + 1]);
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

IConverter::sptr MeshConverter::New()
{
    return std::make_shared<MeshConverter>();
}

//-----------------------------------------------------------------------------

std::string const& MeshConverter::getIgtlType() const
{
    return MeshConverter::s_IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& MeshConverter::getFwDataObjectType() const
{
    return MeshConverter::s_FWDATA_OBJECT_TYPE;
}

} // namespace converter

} // namespace sight::io::igtl::detail
