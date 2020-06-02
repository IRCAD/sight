/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "igtlProtocol/converter/MeshConverter.hpp"

#include "igtlProtocol/DataConverter.hpp"

#include <boost/numeric/conversion/cast.hpp>

#include <fwData/Array.hpp>
#include <fwData/Mesh.hpp>

#include <igtlPolyDataMessage.h>

#include <algorithm>

namespace igtlProtocol
{
namespace converter
{
const std::string MeshConverter::s_IGTL_TYPE          = "POLYDATA";
const std::string MeshConverter::s_FWDATA_OBJECT_TYPE = ::fwData::Mesh::classname();

converterRegisterMacro(::igtlProtocol::converter::MeshConverter);

MeshConverter::MeshConverter()
{
}

//-----------------------------------------------------------------------------

MeshConverter::~MeshConverter()
{
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer MeshConverter::fromFwDataObject (::fwData::Object::csptr src) const
{
    ::fwData::Mesh::csptr meshSrc = ::fwData::Mesh::dynamicConstCast(src);

    ::igtl::PolyDataMessage::Pointer dest = ::igtl::PolyDataMessage::New();
    this->copyCellsFromFwMesh(meshSrc, dest);
    this->copyPointsFromFwMesh(meshSrc, dest);
    this->copyAttributesFromFwMesh(meshSrc, dest);
    return igtl::MessageBase::Pointer(dest);
}

//-----------------------------------------------------------------------------

void MeshConverter::copyCellsFromFwMesh(::fwData::Mesh::csptr meshSrc, ::igtl::PolyDataMessage::Pointer dest) const
{
    const auto dumpLock = meshSrc->lock();
    auto cellItr        = meshSrc->begin< ::fwData::iterator::ConstCellIterator >();

    const ::fwData::Mesh::Id nbCells = meshSrc->getNumberOfCells();

    dest->SetLines(::igtl::PolyDataCellArray::New());
    dest->SetTriangleStrips(::igtl::PolyDataCellArray::New());
    dest->SetVertices(::igtl::PolyDataCellArray::New());

    igtlUint32 cell[5];

    for(size_t i = 0; i < nbCells; ++i, ++cellItr)
    {
        const ::fwData::Mesh::CellType cellType = *cellItr->type;

        switch (cellType)
        {
            case ::fwData::Mesh::CellType::EDGE:
                cell[0] = cellItr->pointIdx[0];
                cell[1] = cellItr->pointIdx[1];
                dest->GetLines()->AddCell(2, cell);
                break;

            case ::fwData::Mesh::CellType::TRIANGLE:
                cell[0] = cellItr->pointIdx[0];
                cell[1] = cellItr->pointIdx[1];
                cell[2] = cellItr->pointIdx[2];
                dest->GetTriangleStrips()->AddCell(3, cell);
                break;

            case ::fwData::Mesh::CellType::QUAD:
                cell[0] = cellItr->pointIdx[0];
                cell[1] = cellItr->pointIdx[1];
                cell[2] = cellItr->pointIdx[2];
                cell[3] = cellItr->pointIdx[3];
                dest->GetVertices()->AddCell(4, cell);
                break;

            case ::fwData::Mesh::CellType::TETRA:
                cell[0] = cellItr->pointIdx[0];
                cell[1] = cellItr->pointIdx[1];
                cell[2] = cellItr->pointIdx[2];
                cell[3] = cellItr->pointIdx[3];
                dest->GetVertices()->AddCell(4, cell);
                break;
            default:
                OSLM_ERROR("This type of cell is not managed: " << static_cast<std::uint8_t>(cellType));
        }
    }
}

//-----------------------------------------------------------------------------

void MeshConverter::copyPointsFromFwMesh(::fwData::Mesh::csptr meshSrc, ::igtl::PolyDataMessage::Pointer dest) const
{
    const auto dumpLock = meshSrc->lock();

    auto itr       = meshSrc->begin< ::fwData::iterator::ConstPointIterator >();
    const auto end = meshSrc->end< ::fwData::iterator::ConstPointIterator >();

    dest->SetPoints(::igtl::PolyDataPointArray::New().GetPointer());
    for (; itr != end; ++itr)
    {
        dest->GetPoints()->AddPoint(itr->point->x, itr->point->y, itr->point->z);
    }

}

//-----------------------------------------------------------------------------

void MeshConverter::copyAttributesFromFwMesh(::fwData::Mesh::csptr meshSrc,
                                             ::igtl::PolyDataMessage::Pointer dest) const
{
    const auto dumpLock = meshSrc->lock();

    const size_t numberOfPoints = meshSrc->getNumberOfPoints();
    const size_t numberOfCells  = meshSrc->getNumberOfCells();

    auto pointsItr       = meshSrc->begin< ::fwData::iterator::ConstPointIterator >();
    const auto pointsEnd = meshSrc->end< ::fwData::iterator::ConstPointIterator >();

    auto cellsItr       = meshSrc->begin< ::fwData::iterator::ConstCellIterator >();
    const auto cellsEnd = meshSrc->end< ::fwData::iterator::ConstCellIterator >();

    dest->ClearAttributes();

    // point attributes
    if (meshSrc->hasPointColors() || meshSrc->hasPointNormals() || meshSrc->hasPointTexCoords())
    {
        igtlFloat32* igtlDataPointColor  = nullptr;
        igtlFloat32* igtlDataPointNormal = nullptr;
        igtlFloat32* igtlDataPointTex    = nullptr;

        if (meshSrc->hasPointColors())
        {
            igtlDataPointColor = new igtlFloat32[4* numberOfPoints];
        }
        if (meshSrc->hasPointNormals())
        {
            igtlDataPointNormal = new igtlFloat32[3* numberOfPoints];
        }
        if (meshSrc->hasPointTexCoords())
        {
            igtlDataPointTex = new igtlFloat32[2* numberOfPoints];
        }

        for (size_t i = 0; pointsItr != pointsEnd; ++pointsItr, ++i)
        {
            if (pointsItr->rgb)
            {
                igtlDataPointColor[4*i]   = static_cast<float>(pointsItr->rgb->r) / 255.f;
                igtlDataPointColor[4*i+1] = static_cast<float>(pointsItr->rgb->g) / 255.f;
                igtlDataPointColor[4*i+2] = static_cast<float>(pointsItr->rgb->b) / 255.f;
                igtlDataPointColor[4*i+3] = 1.f;
            }
            else if (pointsItr->rgba)
            {
                igtlDataPointColor[4*i]   = static_cast<float>(pointsItr->rgba->r) / 255.f;
                igtlDataPointColor[4*i+1] = static_cast<float>(pointsItr->rgba->g) / 255.f;
                igtlDataPointColor[4*i+2] = static_cast<float>(pointsItr->rgba->b) / 255.f;
                igtlDataPointColor[4*i+3] = static_cast<float>(pointsItr->rgba->a) / 255.f;
            }
            if(pointsItr->normal)
            {
                igtlDataPointNormal[3*i]   = pointsItr->normal->nx;
                igtlDataPointNormal[3*i+1] = pointsItr->normal->ny;
                igtlDataPointNormal[3*i+2] = pointsItr->normal->nz;
            }
            if (pointsItr->tex)
            {
                igtlDataPointTex[2*i]   = pointsItr->tex->u;
                igtlDataPointTex[2*i+1] = pointsItr->tex->v;
            }
        }

        if (meshSrc->hasPointColors())
        {
            ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
            attr->SetType(::igtl::PolyDataAttribute::POINT_RGBA);
            attr->SetName("PointColors");
            attr->SetSize(static_cast<igtlUint32>(numberOfPoints));
            attr->SetData(igtlDataPointColor);
            dest->AddAttribute(attr);
            delete[] igtlDataPointColor;
        }
        if (meshSrc->hasPointNormals())
        {
            ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
            attr->SetType(::igtl::PolyDataAttribute::POINT_NORMAL);
            attr->SetName("PointNormals");
            attr->SetSize(static_cast<igtlUint32>(numberOfPoints));
            attr->SetData(igtlDataPointNormal);
            dest->AddAttribute(attr);
            delete [] igtlDataPointNormal;
        }
        if (meshSrc->hasPointTexCoords())
        {
            ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
            attr->SetName("PointTexCoord");
            attr->SetType(::igtl::PolyDataAttribute::POINT_VECTOR);
            attr->SetSize( numberOfPoints);
            attr->SetData(igtlDataPointTex);
            dest->AddAttribute(attr);
            delete [] igtlDataPointTex;
        }
    }

    // cell attributes
    if (meshSrc->hasCellColors() || meshSrc->hasCellNormals() || meshSrc->hasCellTexCoords())
    {
        igtlFloat32* igtlDataCellColor  = nullptr;
        igtlFloat32* igtlDataCellNormal = nullptr;
        igtlFloat32* igtlDataCellTex    = nullptr;

        if (meshSrc->hasCellColors())
        {
            igtlDataCellColor = new igtlFloat32[4* numberOfCells];
        }
        if (meshSrc->hasCellNormals())
        {
            igtlDataCellNormal = new igtlFloat32[3* numberOfCells];
        }
        if (meshSrc->hasCellTexCoords())
        {
            igtlDataCellTex = new igtlFloat32[2* numberOfCells];
        }

        for (size_t i = 0; cellsItr != cellsEnd; ++cellsItr, ++i)
        {
            if (cellsItr->rgb)
            {
                igtlDataCellColor[4*i]   = static_cast<float>(cellsItr->rgb->r) / 255.f;
                igtlDataCellColor[4*i+1] = static_cast<float>(cellsItr->rgb->g) / 255.f;
                igtlDataCellColor[4*i+2] = static_cast<float>(cellsItr->rgb->b) / 255.f;
                igtlDataCellColor[4*i+3] = 1.f;
            }
            else if (cellsItr->rgba)
            {
                igtlDataCellColor[4*i]   = static_cast<float>(cellsItr->rgba->r) / 255.f;
                igtlDataCellColor[4*i+1] = static_cast<float>(cellsItr->rgba->g) / 255.f;
                igtlDataCellColor[4*i+2] = static_cast<float>(cellsItr->rgba->b) / 255.f;
                igtlDataCellColor[4*i+3] = static_cast<float>(cellsItr->rgba->a) / 255.f;
            }
            if(cellsItr->normal)
            {
                igtlDataCellNormal[3*i]   = cellsItr->normal->nx;
                igtlDataCellNormal[3*i+1] = cellsItr->normal->ny;
                igtlDataCellNormal[3*i+2] = cellsItr->normal->nz;
            }
            if (cellsItr->tex)
            {
                igtlDataCellTex[2*i]   = cellsItr->tex->u;
                igtlDataCellTex[2*i+1] = cellsItr->tex->v;
            }
        }

        if (meshSrc->hasCellColors())
        {
            ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
            attr->SetType(::igtl::PolyDataAttribute::CELL_RGBA);
            attr->SetName("CellColors");
            attr->SetSize(static_cast<igtlUint32>(numberOfCells));
            attr->SetData(igtlDataCellColor);
            dest->AddAttribute(attr);
            delete[] igtlDataCellColor;
        }
        if (meshSrc->hasCellNormals())
        {
            ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
            attr->SetType(::igtl::PolyDataAttribute::CELL_NORMAL);
            attr->SetName("CellNormals");
            attr->SetSize(static_cast<igtlUint32>(numberOfCells));
            attr->SetData(igtlDataCellNormal);
            dest->AddAttribute(attr);
            delete [] igtlDataCellNormal;
        }
        if (meshSrc->hasCellTexCoords())
        {
            ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
            attr->SetName("CellTexCoord");
            attr->SetType(::igtl::PolyDataAttribute::CELL_VECTOR);
            attr->SetSize( numberOfCells);
            attr->SetData(igtlDataCellTex);
            dest->AddAttribute(attr);
            delete [] igtlDataCellTex;
        }
    }
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr MeshConverter::fromIgtlMessage (const ::igtl::MessageBase::Pointer src) const
{
    igtlFloat32 point[3];
    igtlUint32 cell[5];

    ::igtl::PolyDataMessage* msg             = dynamic_cast< ::igtl::PolyDataMessage*>(src.GetPointer());
    ::igtl::PolyDataMessage::Pointer meshMsg = ::igtl::PolyDataMessage::Pointer(msg);
    ::fwData::Mesh::sptr mesh                = ::fwData::Mesh::New();

    const int numberOfPoints = meshMsg->GetPoints()->GetNumberOfPoints();
    igtlUint32 numberOfCells = meshMsg->GetLines()->GetNumberOfCells() +
                               meshMsg->GetVertices()->GetNumberOfCells()
                               + meshMsg->GetTriangleStrips()->GetNumberOfCells();

    igtlUint32 numberOfCellData = meshMsg->GetLines()->GetNumberOfCells() * 2 +
                                  meshMsg->GetTriangleStrips()->GetNumberOfCells() * 3 +
                                  meshMsg->GetVertices()->GetNumberOfCells() * 4;

    if(numberOfCellData == 0)
    {
        numberOfCellData = numberOfPoints;
    }
    if (numberOfCells == 0)
    {
        numberOfCells = numberOfPoints;
    }

    ::fwData::Mesh::Attributes attributes = ::fwData::Mesh::Attributes::NONE;
    for (int i = 0; i < meshMsg->GetNumberOfAttributes(); ++i)
    {
        const ::igtl::PolyDataAttribute::Pointer attr = meshMsg->GetAttribute(i);
        switch (attr->GetType())
        {
            case ::igtl::PolyDataAttribute::POINT_RGBA:
                attributes = attributes | ::fwData::Mesh::Attributes::POINT_COLORS;
                break;
            case ::igtl::PolyDataAttribute::CELL_RGBA:
                attributes = attributes | ::fwData::Mesh::Attributes::CELL_COLORS;
                break;
            case ::igtl::PolyDataAttribute::POINT_NORMAL:
                attributes = attributes | ::fwData::Mesh::Attributes::POINT_NORMALS;
                break;
            case ::igtl::PolyDataAttribute::CELL_NORMAL:
                attributes = attributes | ::fwData::Mesh::Attributes::CELL_NORMALS;
                break;
            case ::igtl::PolyDataAttribute::POINT_VECTOR:
                attributes = attributes | ::fwData::Mesh::Attributes::POINT_TEX_COORDS;
                break;
            case ::igtl::PolyDataAttribute::CELL_VECTOR:
                attributes = attributes | ::fwData::Mesh::Attributes::CELL_TEX_COORDS;
                break;
        }
    }

    mesh->reserve(numberOfPoints, numberOfCells, numberOfCellData, attributes);
    const auto dumpLock = mesh->lock();

    ::igtl::PolyDataPointArray* points = meshMsg->GetPoints();
    unsigned int nbPoints = static_cast<unsigned int>(points->GetNumberOfPoints());
    for (unsigned int i = 0; i < nbPoints; ++i)
    {
        points->GetPoint(i, point);
        mesh->pushPoint(point[0], point[1], point[2]);
    }

    if(numberOfCells != 0)
    {
        for (unsigned int i = 0; i < meshMsg->GetLines()->GetNumberOfCells(); ++i)
        {
            meshMsg->GetLines()->GetCell(i, cell);
            mesh->pushCell(cell[0], cell[1]);
        }

        for (unsigned int i = 0; i < meshMsg->GetTriangleStrips()->GetNumberOfCells(); ++i)
        {
            meshMsg->GetTriangleStrips()->GetCell(i, cell);
            mesh->pushCell(cell[0], cell[1], cell[2]);
        }

        for (unsigned int i = 0; i < meshMsg->GetVertices()->GetNumberOfCells(); ++i)
        {
            meshMsg->GetVertices()->GetCell(i, cell);
            mesh->pushCell(cell[0], cell[1], cell[2], cell[3]);
        }
    }
    else
    {
        for(unsigned int i = 0; i < nbPoints; ++i )
        {
            mesh->pushCell( (::fwData::Mesh::CellValueType) i);
        }
    }

    mesh->adjustAllocatedMemory();

    this->copyAttributeFromPolyData(meshMsg, mesh);

    return mesh;
}

//-----------------------------------------------------------------------------

void MeshConverter::copyAttributeFromPolyData(::igtl::PolyDataMessage::Pointer src, ::fwData::Mesh::sptr dest) const
{
    ::igtl::PolyDataAttribute::Pointer attr;

    for (int i = 0; i < src->GetNumberOfAttributes(); ++i)
    {
        attr = src->GetAttribute(i);
        switch (attr->GetType())
        {
            case ::igtl::PolyDataAttribute::POINT_RGBA:
            {
                igtlFloat32* data = new igtlFloat32[attr->GetSize() * attr->GetNumberOfComponents()];
                attr->GetData(data);

                size_t k = 0;
                for (unsigned int j = 0; j < attr->GetSize(); ++j)
                {
                    dest->setPointColor(j, static_cast< ::fwData::Mesh::ColorValueType >(data[k] * 255.f),
                                        static_cast< ::fwData::Mesh::ColorValueType >(data[k+1] * 255.f),
                                        static_cast< ::fwData::Mesh::ColorValueType >(data[k+2] * 255.f),
                                        static_cast< ::fwData::Mesh::ColorValueType >(data[k+3] * 255.f));
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

                size_t k = 0;
                for (unsigned int j = 0; j < attr->GetSize(); ++j)
                {
                    dest->setCellColor(j, static_cast< ::fwData::Mesh::ColorValueType >(data[k]*255.f),
                                       static_cast< ::fwData::Mesh::ColorValueType >(data[k+1] * 255.f),
                                       static_cast< ::fwData::Mesh::ColorValueType >(data[k+2] * 255.f),
                                       static_cast< ::fwData::Mesh::ColorValueType >(data[k+3] * 255.f));
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
                for (unsigned int j = 0; j < dest->getNumberOfPoints(); ++j)
                {
                    dest->setPointNormal(j, data[j * 3], data[j * 3 +1], data[j * 3+2]);
                }
                attr->Clear();
                delete[] data;
                break;
            }
            case ::igtl::PolyDataAttribute::CELL_NORMAL:
            {
                igtlFloat32* data = new igtlFloat32[attr->GetSize() * attr->GetNumberOfComponents()];
                attr->GetData(data);
                for (unsigned int j = 0; j < dest->getNumberOfCells(); ++j)
                {
                    dest->setCellNormal(j, data[j * 3], data[j * 3 +1], data[j * 3+2]);
                }
                attr->Clear();
                delete[] data;
                break;
            }

            case ::igtl::PolyDataAttribute::POINT_VECTOR:
            {
                igtlFloat32* data = new igtlFloat32[attr->GetSize() * attr->GetNumberOfComponents()];
                attr->GetData(data);
                for (unsigned int j = 0; j < dest->getNumberOfPoints(); ++j)
                {
                    dest->setPointTexCoord(j, data[j * 2], data[j * 2 +1]);
                }
                attr->Clear();
                delete[] data;
                break;
            }

            case ::igtl::PolyDataAttribute::CELL_VECTOR:
            {
                igtlFloat32* data = new igtlFloat32[attr->GetSize() * attr->GetNumberOfComponents()];
                attr->GetData(data);
                for (unsigned int j = 0; j < dest->getNumberOfCells(); ++j)
                {
                    dest->setCellTexCoord(j, data[j * 2], data[j * 2 +1]);
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
    SLM_TRACE_FUNC();

    return std::make_shared< MeshConverter >();
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
} // namespace igtlProtocol
