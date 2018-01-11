/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "igtlProtocol/converter/MeshConverter.hpp"

#include "igtlProtocol/DataConverter.hpp"

#include <fwData/Array.hpp>
#include <fwData/Mesh.hpp>

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/Mesh.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <igtl/igtlPolyDataMessage.h>

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
    ::fwDataTools::helper::Mesh meshHelper(::fwData::Mesh::constCast(meshSrc));

    const ::fwData::Mesh::Id nbCells = meshSrc->getNumberOfCells();
    ::fwData::Mesh::CellTypesMultiArrayType cellTypes             = meshHelper.getCellTypes();
    ::fwData::Mesh::CellDataMultiArrayType cellData               = meshHelper.getCellData();
    ::fwData::Mesh::CellDataOffsetsMultiArrayType cellDataOffsets = meshHelper.getCellDataOffsets();

    dest->SetLines(::igtl::PolyDataCellArray::New());
    dest->SetTriangleStrips(::igtl::PolyDataCellArray::New());
    dest->SetVertices(::igtl::PolyDataCellArray::New());

    igtlUint32 cell[5];

    for(size_t i = 0; i < nbCells; ++i)
    {
        const ::fwData::Mesh::CellTypes cellType = cellTypes[i];
        const ::fwData::Mesh::Id offset          = cellDataOffsets[i];

        switch (cellType)
        {
            case ::fwData::Mesh::EDGE:
                cell[0] = cellData[offset];
                cell[1] = cellData[offset + 1];
                dest->GetLines()->AddCell(2, cell);
                break;

            case ::fwData::Mesh::TRIANGLE:
                cell[0] = cellData[offset];
                cell[1] = cellData[offset + 1];
                cell[2] = cellData[offset + 2];
                dest->GetTriangleStrips()->AddCell(3, cell);
                break;

            case ::fwData::Mesh::QUAD:
                cell[0] = cellData[offset];
                cell[1] = cellData[offset+1];
                cell[2] = cellData[offset+2];
                cell[3] = cellData[offset+3];
                dest->GetVertices()->AddCell(4, cell);
                break;

            case ::fwData::Mesh::TETRA:
                cell[0] = cellData[offset];
                cell[1] = cellData[offset + 1];
                cell[2] = cellData[offset + 2];
                cell[3] = cellData[offset + 3];
                dest->GetVertices()->AddCell(4, cell);
                break;
        }
    }
}

//-----------------------------------------------------------------------------

void MeshConverter::copyPointsFromFwMesh(::fwData::Mesh::csptr meshSrc, ::igtl::PolyDataMessage::Pointer dest) const
{
    ::fwDataTools::helper::Mesh meshHelper(::fwData::Mesh::constCast(meshSrc));
    const ::fwData::Mesh::Id nbPoints = meshSrc->getNumberOfPoints();
    ::fwData::Mesh::PointsMultiArrayType points = meshHelper.getPoints();
    ::fwData::Mesh::PointsMultiArrayType::index i;

    dest->SetPoints(::igtl::PolyDataPointArray::New().GetPointer());
    for (i = 0; i != nbPoints; ++i)
    {
        dest->GetPoints()->AddPoint(points[i][0], points[i][1], points[i][2]);
    }

}

//-----------------------------------------------------------------------------

void MeshConverter::copyAttributesFromFwMesh(::fwData::Mesh::csptr meshSrc,
                                             ::igtl::PolyDataMessage::Pointer dest) const
{
    ::fwData::Array::sptr pointColorArray   = meshSrc->getPointColorsArray();
    ::fwData::Array::sptr cellColorArray    = meshSrc->getCellColorsArray();
    ::fwData::Array::sptr pointNormalsArray = meshSrc->getPointNormalsArray();
    ::fwData::Array::sptr cellNormalsArray  = meshSrc->getCellNormalsArray();

    ::fwData::Array::sptr pointTexCoordArray = meshSrc->getPointTexCoordsArray();
    ::fwData::Array::sptr cellTexCoordArray  = meshSrc->getCellTexCoordsArray();

    dest->ClearAttributes();
    const ::fwData::Mesh::Id numberOfPoints = meshSrc->getNumberOfPoints();
    const ::fwData::Mesh::Id numberOfCells  = meshSrc->getNumberOfCells();

    if (pointColorArray)
    {
        ::fwDataTools::helper::Array arrayHelper(pointColorArray);
        std::size_t pointColorSize = pointColorArray->getNumberOfComponents() * meshSrc->getNumberOfPoints();

        ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
        attr->SetType(::igtl::PolyDataAttribute::POINT_RGBA);
        attr->SetName("PointColors");
        //NOTE: number of components is automaticly set to 4 when POINT_RGBA is choose
        attr->SetSize(static_cast<igtlUint32>(numberOfPoints));

        igtlFloat32* igtlDataPoint = new igtlFloat32[pointColorSize];

        std::transform(arrayHelper.begin<unsigned char>(), arrayHelper.end<unsigned char>(),
                       igtlDataPoint, &MeshConverter::toIgtlFloat32);

        attr->SetData(igtlDataPoint);

        dest->AddAttribute(attr);

        delete[] igtlDataPoint;

    }

    if (cellColorArray)
    {
        ::fwDataTools::helper::Array arrayHelper(cellColorArray);
        std::size_t cellColorSize = cellColorArray->getNumberOfComponents() * meshSrc->getNumberOfCells();

        ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
        attr->SetType(::igtl::PolyDataAttribute::CELL_RGBA);
        attr->SetName("CellColors");
        attr->SetSize(static_cast<igtlUint32>( numberOfCells));

        igtlFloat32* igtlData = new igtlFloat32[cellColorSize];
        std::transform(arrayHelper.begin<unsigned char>(), arrayHelper.end<unsigned char>(),
                       igtlData, &MeshConverter::toIgtlFloat32);

        attr->SetData(igtlData);
        dest->AddAttribute(attr);

        delete[] igtlData;

    }

    if (pointNormalsArray)
    {
        ::fwDataTools::helper::Array arrayHelper(pointNormalsArray);

        ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
        attr->SetType(::igtl::PolyDataAttribute::POINT_NORMAL);
        attr->SetName("PointNormals");
        attr->SetSize(static_cast<igtlUint32>(numberOfPoints));
        attr->SetData(arrayHelper.begin<igtlFloat32>());
        dest->AddAttribute(attr);

    }

    if (cellNormalsArray)
    {
        ::fwDataTools::helper::Array arrayHelper(cellNormalsArray);

        ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
        attr->SetName("CellNormals");
        attr->SetType(::igtl::PolyDataAttribute::CELL_NORMAL);
        attr->SetSize(static_cast<igtlUint32>(numberOfCells));
        attr->SetData(arrayHelper.begin<igtlFloat32>());
        dest->AddAttribute(attr);

    }

    if(pointTexCoordArray)
    {
        ::fwDataTools::helper::Array arrayHelper(pointTexCoordArray);

        ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
        attr->SetName("PointTexCoord");
        attr->SetType(::igtl::PolyDataAttribute::POINT_VECTOR);
        attr->SetSize( numberOfPoints);
        attr->SetData(arrayHelper.begin<igtlFloat32>());
        dest->AddAttribute(attr);
    }

    if(cellTexCoordArray)
    {
        ::fwDataTools::helper::Array arrayHelper(cellTexCoordArray);

        ::igtl::PolyDataAttribute::Pointer attr = ::igtl::PolyDataAttribute::New();
        attr->SetName("CellTexCoord");
        attr->SetType(::igtl::PolyDataAttribute::CELL_VECTOR);
        attr->SetSize(numberOfCells);
        attr->SetData(arrayHelper.begin<igtlFloat32>());
        dest->AddAttribute(attr);
    }
}

//-----------------------------------------------------------------------------

igtlFloat32 MeshConverter::toIgtlFloat32 (unsigned char colorComponent)
{
    return colorComponent / 255.0f;
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

    ::fwDataTools::helper::Mesh meshHelper(mesh);
    mesh->allocate(numberOfPoints, numberOfCells, numberOfCellData);

    ::igtl::PolyDataPointArray* points = meshMsg->GetPoints();
    unsigned int nbPoints = static_cast<unsigned int>(points->GetNumberOfPoints());
    for (unsigned int i = 0; i < nbPoints; ++i)
    {
        points->GetPoint(i, point);
        meshHelper.insertNextPoint(point[0], point[1], point[2]);
    }

    if(numberOfCells != 0)
    {
        for (unsigned int i = 0; i < meshMsg->GetLines()->GetNumberOfCells(); ++i)
        {
            meshMsg->GetLines()->GetCell(i, cell);
            meshHelper.insertNextCell(cell[0], cell[1]);
        }

        for (unsigned int i = 0; i < meshMsg->GetTriangleStrips()->GetNumberOfCells(); ++i)
        {
            meshMsg->GetTriangleStrips()->GetCell(i, cell);
            meshHelper.insertNextCell(cell[0], cell[1], cell[2]);
        }

        for (unsigned int i = 0; i < meshMsg->GetVertices()->GetNumberOfCells(); ++i)
        {
            meshMsg->GetVertices()->GetCell(i, cell);
            meshHelper.insertNextCell(cell[0], cell[1], cell[2], cell[3]);
        }
    }
    else
    {
        for(unsigned int i = 0; i < nbPoints; ++i )
        {
            meshHelper.insertNextCell( (::fwData::Mesh::CellValueType) i);
        }
    }

    mesh->adjustAllocatedMemory();

    this->copyAttributeFromPolyData(meshMsg, mesh);

    return mesh;
}

//-----------------------------------------------------------------------------

igtlUint8 MeshConverter::toIgtlUint8(igtlFloat32 colorComponent)
{
    return static_cast<igtlUint8>(colorComponent * 255);
}

//-----------------------------------------------------------------------------

void MeshConverter::copyAttributeFromPolyData(::igtl::PolyDataMessage::Pointer src, ::fwData::Mesh::sptr dest) const
{
    SLM_TRACE_FUNC();

    ::igtl::PolyDataAttribute::Pointer attr;

    ::fwDataTools::helper::Mesh meshHelper(dest);
    unsigned char rgbaColor[4];

    for (int i = 0; i < src->GetNumberOfAttributes(); ++i)
    {
        attr = src->GetAttribute(i);
        switch (attr->GetType())
        {
            case ::igtl::PolyDataAttribute::POINT_RGBA:
            {
                dest->allocatePointColors(
                    static_cast< ::fwData::Mesh::ColorArrayTypes>(attr->GetNumberOfComponents()));
                meshHelper.updateLock();
                igtlFloat32* data = new igtlFloat32[attr->GetSize() * attr->GetNumberOfComponents()];
                attr->GetData(data);
                for (unsigned int j = 0; j < attr->GetSize(); j = j+attr->GetNumberOfComponents())
                {
                    std::transform(&data[j], &data[j] + attr->GetNumberOfComponents(), rgbaColor,
                                   &MeshConverter::toIgtlUint8);
                    meshHelper.setPointColor(j, rgbaColor);
                }
                attr->Clear();
                delete[] data;
                break;
            }
            case ::igtl::PolyDataAttribute::CELL_RGBA:
            {
                dest->allocateCellColors(
                    static_cast< ::fwData::Mesh::ColorArrayTypes>(attr->GetNumberOfComponents()));
                meshHelper.updateLock();
                igtlFloat32* data = new igtlFloat32[attr->GetSize() * attr->GetNumberOfComponents()];
                attr->GetData(data);
                for (unsigned int j = 0; j < attr->GetSize(); j += attr->GetNumberOfComponents())
                {
                    std::transform(&data[j], &data[j] + attr->GetNumberOfComponents(), rgbaColor,
                                   &MeshConverter::toIgtlUint8);
                    meshHelper.setCellColor(j, rgbaColor);
                }
                attr->Clear();
                delete[] data;
                break;
            }
            case ::igtl::PolyDataAttribute::POINT_NORMAL:
            {
                dest->allocatePointNormals();

                meshHelper.updateLock();
                igtlFloat32* data = new igtlFloat32[attr->GetSize() * attr->GetNumberOfComponents()];
                attr->GetData(data);
                for (unsigned int j = 0; j < dest->getNumberOfPoints(); ++j)
                {
                    meshHelper.setPointNormal(j, &data[i * attr->GetNumberOfComponents()]);
                }
                attr->Clear();
                delete[] data;
                break;
            }
            case ::igtl::PolyDataAttribute::CELL_NORMAL:
            {
                dest->allocateCellNormals();
                meshHelper.updateLock();
                igtlFloat32* data = new igtlFloat32[attr->GetSize() * attr->GetNumberOfComponents()];
                attr->GetData(data);
                for (unsigned int j = 0; j < dest->getNumberOfCells(); ++j)
                {
                    meshHelper.setCellNormal(j, &data[i * attr->GetNumberOfComponents()]);
                }
                attr->Clear();
                delete[] data;
                break;
            }

            case ::igtl::PolyDataAttribute::POINT_VECTOR:
            {
                dest->allocatePointTexCoords();
                meshHelper.updateLock();
                igtlFloat32* data = new igtlFloat32[attr->GetSize() * attr->GetNumberOfComponents()];
                attr->GetData(data);
                for (unsigned int j = 0; j < dest->getNumberOfPoints(); ++j)
                {
                    meshHelper.setPointTexCoord(j, &data[i * attr->GetNumberOfComponents()]);
                }
                attr->Clear();
                delete[] data;
                break;
            }

            case ::igtl::PolyDataAttribute::CELL_VECTOR:
            {
                dest->allocateCellTexCoords();
                meshHelper.updateLock();
                igtlFloat32* data = new igtlFloat32[attr->GetSize() * attr->GetNumberOfComponents()];
                attr->GetData(data);
                for (unsigned int j = 0; j < dest->getNumberOfCells(); ++j)
                {
                    meshHelper.setCellTexCoord(j, &data[i * attr->GetNumberOfComponents()]);
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
