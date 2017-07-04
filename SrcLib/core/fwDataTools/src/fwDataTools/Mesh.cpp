/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataTools/Mesh.hpp"

#include "fwDataTools/helper/ArrayGetter.hpp"
#include "fwDataTools/thread/RegionThreader.hpp"
#include "fwDataTools/TransformationMatrix3D.hpp"

#include <fwTools/NumericRoundCast.hxx>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cstdlib>
#include <ctime>
#include <functional>
#include <map>

namespace fwDataTools
{

struct RandFloat
{
    //------------------------------------------------------------------------------

    float operator()()
    {
        return ((rand() % 101 - 50.f)) / 500.f;
    }
};

//------------------------------------------------------------------------------

void Mesh::initRand()
{
    std::srand(::fwTools::numericRoundCast<unsigned int>(std::time(NULL)));
}

//------------------------------------------------------------------------------

bool Mesh::hasUniqueCellType(::fwData::Mesh::sptr mesh, ::fwData::Mesh::CellTypes cell)
{
    bool res = true;
    ::fwDataTools::helper::Mesh meshHelper(mesh);
    ::fwData::Mesh::CellTypesMultiArrayType cellTypes = meshHelper.getCellTypes();

    for(::fwData::Mesh::CellTypes type :  cellTypes)
    {
        if(type != cell)
        {
            return false;
        }
    }
    return res;
}

//------------------------------------------------------------------------------

typedef boost::multi_array_ref<Point, 1> PointsMultiArrayType;

//------------------------------------------------------------------------------

Vector<float>& computeTriangleNormal(const Point& p1, const Point& p2, const Point& p3, Vector<float>& n)
{
    n = Vector<float>(p1, p2);
    Vector<float> v(p1, p3);
    n.crossWith(v);
    n.normalize();
    return n;
}

//------------------------------------------------------------------------------

Vector<float>& computeTriangleNormal( const PointsMultiArrayType& points, const ::fwData::Mesh::CellValueType* cell,
                                      Vector<float>& n)
{
    const Point& p1 = points[cell[0]];
    const Point& p2 = points[cell[1]];
    const Point& p3 = points[cell[2]];

    computeTriangleNormal(p1, p2, p3, n);
    return n;
}

//------------------------------------------------------------------------------

Vector<float>& computeCellNormal( const PointsMultiArrayType& points, const ::fwData::Mesh::CellValueType* cell,
                                  size_t cellSize, Vector<float>& n)
{
    n = Vector<float>();
    Vector<float> v;

    for (size_t i = 0; i < cellSize; ++i)
    {
        const Point& p1 = points[cell[i  ]];
        const Point& p2 = points[cell[(i+1)% cellSize]];
        const Point& p3 = points[cell[(i+2)% cellSize]];

        computeTriangleNormal(p1, p2, p3, n);

        n += v;
    }

    n /= ::fwTools::numericRoundCast<float>(cellSize);
    n.normalize();
    return n;
}

//------------------------------------------------------------------------------

void generateRegionCellNormals(::fwDataTools::helper::Mesh::sptr meshHelper, const ::fwData::Mesh::Id regionMin,
                               const ::fwData::Mesh::Id regionMax)
{
    ::fwData::Mesh::csptr mesh = meshHelper->getMesh();
    ::fwDataTools::helper::Array pointArrayHelper(mesh->getPointsArray());
    ::fwDataTools::helper::Array cellNormalsArrayHelper(mesh->getCellNormalsArray());

    PointsMultiArrayType point = PointsMultiArrayType(
        static_cast<PointsMultiArrayType::element*>(pointArrayHelper.getBuffer()),
        ::boost::extents[mesh->getNumberOfPoints()]
        );

    ::fwData::Mesh::CellTypesMultiArrayType cellTypes             = meshHelper->getCellTypes();
    ::fwData::Mesh::CellDataMultiArrayType cellData               = meshHelper->getCellData();
    ::fwData::Mesh::CellDataOffsetsMultiArrayType cellDataOffsets = meshHelper->getCellDataOffsets();

    const Vector<float> vZero;
    ::fwData::Mesh::CellTypes type;
    ::fwData::Mesh::CellDataOffsetType offset;
    ::fwData::Mesh::CellValueType* cell;
    ::fwData::Mesh::Id cellLen = 0;

    const ::fwData::Mesh::Id numberOfCells = mesh->getNumberOfCells();
    const ::fwData::Mesh::Id cellDataSize  = mesh->getCellDataSize();

    Vector< ::fwData::Mesh::NormalValueType >* normals =
        cellNormalsArrayHelper.begin< Vector< ::fwData::Mesh::NormalValueType > >();

    for(::fwData::Mesh::Id i = regionMin; i < regionMax; ++i)
    {
        Vector<float>& n = normals[i];

        type   = cellTypes[i];
        offset = cellDataOffsets[i];
        cell   = &cellData[offset];
        switch (type)
        {
            case 0:
            case 1:
            case 2:
                n = vZero;
                break;
            case 3:
            {
                computeTriangleNormal(point, cell, n);
            }
            break;
            case 4:
            case 5:
            {
                const ::fwData::Mesh::Id i1 = i+1;
                cellLen = (( i1 < numberOfCells ) ? cellDataOffsets[i1] : cellDataSize) - cellDataOffsets[i];

                computeCellNormal(point, cell, cellLen, n);
            }
        }
    }
}

//------------------------------------------------------------------------------

template <typename T>
void vectorSum( std::vector< std::vector<T> >& vectors, size_t regionMin, size_t regionMax )
{
    if (vectors.empty())
    {
        return;
    }

    typename std::vector< std::vector<T> >::iterator vIter = vectors.begin();

    std::vector<T>& res = vectors[0];

    for (++vIter; vIter != vectors.end(); ++vIter)
    {
        for (size_t i = regionMin; i < regionMax; ++i)
        {
            res[i] += (*vIter)[i];
        }
    }

}

//------------------------------------------------------------------------------

void Mesh::generateCellNormals(::fwData::Mesh::sptr mesh)
{
    const ::fwData::Mesh::Id numberOfCells = mesh->getNumberOfCells();
    if(numberOfCells > 0)
    {
        mesh->allocateCellNormals();
        ::fwDataTools::helper::Mesh::sptr meshHelper;
        meshHelper = ::fwDataTools::helper::Mesh::New(mesh);

        ::fwDataTools::thread::RegionThreader rt((numberOfCells >= 200000) ? 4 : 1);
        rt( std::bind(&generateRegionCellNormals, meshHelper, std::placeholders::_1, std::placeholders::_2),
            numberOfCells );
    }
}

//------------------------------------------------------------------------------

typedef std::vector< std::vector< unsigned char > > CharVectors;
typedef std::vector< std::vector< float > > FloatVectors;

//------------------------------------------------------------------------------

void generateRegionCellNormalsByPoints(FloatVectors& normalsData, CharVectors& normalCounts, size_t dataId,
                                       ::fwDataTools::helper::Mesh::sptr meshHelper, const ::fwData::Mesh::Id regionMin,
                                       const ::fwData::Mesh::Id regionMax)
{
    ::fwData::Mesh::csptr mesh = meshHelper->getMesh();
    FloatVectors::value_type& normalsResults = normalsData[dataId];
    CharVectors::value_type& normalCount     = normalCounts[dataId];

    const ::fwData::Mesh::Id nbOfPoints = mesh->getNumberOfPoints();
    normalsResults.resize(3*nbOfPoints, 0.f);
    normalCount.resize(nbOfPoints, 0);

    ::fwData::Mesh::CellTypesMultiArrayType cellTypes             = meshHelper->getCellTypes();
    ::fwData::Mesh::CellDataMultiArrayType cellData               = meshHelper->getCellData();
    ::fwData::Mesh::CellDataOffsetsMultiArrayType cellDataOffsets = meshHelper->getCellDataOffsets();

    ::fwData::Mesh::CellTypes type;
    ::fwData::Mesh::CellDataOffsetType offset;
    ::fwData::Mesh::CellValueType* cell;
    ::fwData::Mesh::Id cellLen = 0;

    const ::fwData::Mesh::Id numberOfCells = mesh->getNumberOfCells();
    const ::fwData::Mesh::Id cellDataSize  = mesh->getCellDataSize();

    ::fwDataTools::helper::Array arrayHelper(mesh->getCellNormalsArray());
    Vector< ::fwData::Mesh::NormalValueType >* normals =
        arrayHelper.begin< Vector< ::fwData::Mesh::NormalValueType > >();
    Vector< ::fwData::Mesh::NormalValueType >* normalResults =
        reinterpret_cast< Vector< ::fwData::Mesh::NormalValueType >* >( &(*normalsResults.begin()));

    ::fwData::Mesh::CellValueType* pointId;
    ::fwData::Mesh::CellValueType* cellEnd;

    for(::fwData::Mesh::Id i = regionMin; i < regionMax; ++i)
    {
        type    = cellTypes[i];
        offset  = cellDataOffsets[i];
        cell    = &cellData[offset];
        cellLen = type;

        switch (type)
        {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
                cellLen = type;
                break;
            case 5:
            {
                const ::fwData::Mesh::Id i1 = i+1;
                cellLen = (( i1 < numberOfCells ) ? cellDataOffsets[i1] : cellDataSize) - cellDataOffsets[i];
            }
        }

        cellEnd = cell + cellLen;

        for(pointId = cell; pointId != cellEnd; ++pointId)
        {
            Vector< ::fwData::Mesh::NormalValueType >& res = normalResults[*pointId];
            res                   += normals[i];
            normalCount[*pointId] += 1;
        }

    }
}

//------------------------------------------------------------------------------

void normalizeRegionCellNormalsByPoints(FloatVectors::value_type& normalsData, CharVectors::value_type& normalCount,
                                        ::fwData::Mesh::sptr mesh, const ::fwData::Mesh::Id regionMin,
                                        const ::fwData::Mesh::Id regionMax)
{
    Vector< ::fwData::Mesh::NormalValueType >* normalSum =
        reinterpret_cast< Vector< ::fwData::Mesh::NormalValueType >* >( &(*normalsData.begin()) );

    ::fwDataTools::helper::Array arrayHelper(mesh->getPointNormalsArray());
    Vector< ::fwData::Mesh::NormalValueType >* normals =
        arrayHelper.begin< Vector< ::fwData::Mesh::NormalValueType > >();

    for ( ::fwData::Mesh::Id i = regionMin; i < regionMax; ++i)
    {
        CharVectors::value_type::value_type count = normalCount[i];
        normals[i] = normalSum[i];

        if(count > 1)
        {
            normals[i] /= count;
        }
    }
}

//------------------------------------------------------------------------------

void Mesh::generatePointNormals(::fwData::Mesh::sptr mesh)
{
    const ::fwData::Mesh::Id nbOfPoints = mesh->getNumberOfPoints();
    if(nbOfPoints > 0)
    {
        const ::fwData::Mesh::Id numberOfCells = mesh->getNumberOfCells();
        ::fwData::Array::sptr oldCellNormals = mesh->getCellNormalsArray();
        mesh->clearCellNormals();

        generateCellNormals(mesh);

        mesh->allocatePointNormals();

        ::fwDataTools::helper::Mesh::sptr meshHelper;
        meshHelper = ::fwDataTools::helper::Mesh::New(mesh);

        ::fwDataTools::thread::RegionThreader rt((nbOfPoints >= 100000) ? 4 : 1);

        FloatVectors normalsData(rt.numberOfThread());
        CharVectors normalCounts(rt.numberOfThread());

        rt( std::bind(&generateRegionCellNormalsByPoints,
                      ::boost::ref(normalsData),
                      ::boost::ref(normalCounts),
                      std::placeholders::_3,
                      meshHelper,
                      std::placeholders::_1,
                      std::placeholders::_2),
            numberOfCells);

        rt( std::bind(&vectorSum<FloatVectors::value_type::value_type>,
                      ::boost::ref(normalsData),
                      std::placeholders::_1, std::placeholders::_2),
            nbOfPoints*3);

        rt( std::bind(&vectorSum<CharVectors::value_type::value_type>,
                      ::boost::ref(normalCounts),
                      std::placeholders::_1, std::placeholders::_2),
            nbOfPoints);

        rt( std::bind( &normalizeRegionCellNormalsByPoints,
                       ::boost::ref(normalsData[0]),
                       ::boost::ref(normalCounts[0]),
                       mesh, std::placeholders::_1, std::placeholders::_2),
            nbOfPoints);

        meshHelper.reset();
        mesh->setCellNormalsArray(oldCellNormals);
    }
}

//------------------------------------------------------------------------------

template <typename T>
void regionShakeNormals(T normals, const ::fwData::Mesh::Id regionMin, const ::fwData::Mesh::Id regionMax)
{
    RandFloat randFloat;
    for (::fwData::Mesh::Id i = regionMin; i < regionMax; ++i)
    {
        Vector<float> v(randFloat(), randFloat(), randFloat());
        normals[i] += v;
        normals[i].normalize();
    }
}

//------------------------------------------------------------------------------

void Mesh::shakeNormals(::fwData::Array::sptr array)
{

    if(array
       && array->getType() == ::fwTools::Type::create<float>()
       && !array->empty()
       && array->getNumberOfComponents() == 3
       && array->getNumberOfDimensions() == 1
       )
    {
        ::fwDataTools::helper::Array arrayHelper(array);
        void* buf;
        buf = arrayHelper.getBuffer();
        const ::fwData::Mesh::Id nbOfNormals = array->getSize().at(0);
        typedef boost::multi_array_ref<Vector<float>, 1> NormalsMultiArrayType;
        NormalsMultiArrayType normals = NormalsMultiArrayType(
            static_cast<NormalsMultiArrayType::element*>(buf),
            ::boost::extents[nbOfNormals]
            );

        ::fwDataTools::thread::RegionThreader rt((nbOfNormals >= 150000) ? 4 : 1);
        rt( std::bind(&regionShakeNormals<NormalsMultiArrayType>,
                      std::ref(normals),
                      std::placeholders::_1, std::placeholders::_2),
            nbOfNormals);
    }
}

//------------------------------------------------------------------------------

void Mesh::shakePointNormals(::fwData::Mesh::sptr mesh)
{
    shakeNormals(mesh->getPointNormalsArray());
}

//------------------------------------------------------------------------------

void Mesh::shakeCellNormals(::fwData::Mesh::sptr mesh)
{
    shakeNormals(mesh->getCellNormalsArray());
}

//------------------------------------------------------------------------------

void Mesh::colorizeMeshPoints(::fwData::Mesh::sptr mesh)
{
    mesh->allocatePointColors(::fwData::Mesh::RGB);

    ::fwDataTools::helper::Mesh meshHelper(mesh);

    ::fwData::Mesh::ColorValueType color[4];
    size_t numberOfPoints = mesh->getNumberOfPoints();
    for(size_t i = 0; i < numberOfPoints; ++i)
    {
        color[0] = rand()%256;
        color[1] = rand()%256;
        color[2] = rand()%256;
        meshHelper.setPointColor(i, color);
    }
}
//------------------------------------------------------------------------------

void Mesh::colorizeMeshCells(::fwData::Mesh::sptr mesh)
{
    mesh->allocateCellColors(::fwData::Mesh::RGBA);

    ::fwDataTools::helper::Mesh meshHelper(mesh);

    ::fwData::Mesh::ColorValueType color[4];
    size_t numberOfCells = mesh->getNumberOfCells();
    for(size_t i = 0; i < numberOfCells; ++i)
    {
        color[0] = rand()%256;
        color[1] = rand()%256;
        color[2] = rand()%256;
        color[3] = rand()%256;
        meshHelper.setCellColor(i, color);
    }
}

//------------------------------------------------------------------------------

void Mesh::shakePoint(::fwData::Mesh::sptr mesh)
{
    ::fwDataTools::helper::Mesh meshHelper(mesh);

    size_t nbPts = mesh->getNumberOfPoints();
    ::fwData::Mesh::PointsMultiArrayType points = meshHelper.getPoints();
    RandFloat randFloat;
    for(size_t i = 0; i < nbPts; ++i )
    {
        points[i][0] += randFloat()*5;
        points[i][1] += randFloat()*5;
        points[i][2] += randFloat()*5;
    }
}

//------------------------------------------------------------------------------

void Mesh::transform( ::fwData::Mesh::csptr inMesh, ::fwData::Mesh::sptr outMesh,
                      ::fwData::TransformationMatrix3D::csptr t )
{
    ::fwDataTools::helper::ArrayGetter arrayHelper(inMesh->getPointsArray());
    const ::fwData::Mesh::PointValueType* inPoints = arrayHelper.begin< ::fwData::Mesh::PointValueType >();

    ::fwDataTools::helper::Array outArrayHelper(outMesh->getPointsArray());
    ::fwData::Mesh::PointValueType* outPoints = outArrayHelper.begin< ::fwData::Mesh::PointValueType >();

    const ::glm::dmat4x4 matrix = ::fwDataTools::TransformationMatrix3D::getMatrixFromTF3D(t);

    const size_t numPts = inMesh->getNumberOfPoints();
    SLM_ASSERT("In and out meshes should have the same number of points", numPts == outMesh->getNumberOfPoints());

    for(size_t i = 0; i < numPts; ++i )
    {
        const ::glm::vec4 pt(inPoints[i*3], inPoints[i*3 + 1], inPoints[i*3 + 2], 1.);
        const ::glm::vec4 transformedPt = matrix * pt;

        outPoints[i*3]     = transformedPt.x;
        outPoints[i*3 + 1] = transformedPt.y;
        outPoints[i*3 + 2] = transformedPt.z;
    }

    auto normalsArray = inMesh->getPointNormalsArray();
    if(normalsArray != nullptr)
    {
        SLM_ASSERT("in and out meshes should have the same number of normals", outMesh->getPointNormalsArray());

        ::fwDataTools::helper::ArrayGetter normalsArrayHelper(inMesh->getPointNormalsArray());
        const ::fwData::Mesh::NormalValueType* normals = normalsArrayHelper.begin< ::fwData::Mesh::NormalValueType >();

        ::fwDataTools::helper::Array outNormalsArrayHelper(outMesh->getPointNormalsArray());
        ::fwData::Mesh::NormalValueType* outNormals = outNormalsArrayHelper.begin< ::fwData::Mesh::NormalValueType >();

        for(size_t i = 0; i < numPts; ++i )
        {
            const ::glm::vec4 normal(normals[i*3], normals[i*3 + 1], normals[i*3 + 2], 0.);
            const ::glm::vec4 transformedNormal = ::glm::normalize(matrix * normal);

            outNormals[i*3]     = transformedNormal.x;
            outNormals[i*3 + 1] = transformedNormal.y;
            outNormals[i*3 + 2] = transformedNormal.z;
        }
    }
}

//------------------------------------------------------------------------------

void Mesh::transform( ::fwData::Mesh::sptr mesh, ::fwData::TransformationMatrix3D::csptr t )
{
    Mesh::transform(mesh, mesh, t);
}

//------------------------------------------------------------------------------

void Mesh::colorizeMeshPoints( const ::fwData::Mesh::sptr& mesh, const std::uint8_t colorR, const std::uint8_t colorG,
                               const std::uint8_t colorB, const std::uint8_t colorA)
{
    ::fwData::Array::sptr itemColors = mesh->getPointColorsArray();
    SLM_ASSERT("color array must be allocated", itemColors);

    if ( itemColors && itemColors->getBufferObject() )
    {
        ::fwDataTools::helper::Array hArray( itemColors );

        ::fwData::Mesh::ColorValueType* itemColorsBuffer = hArray.begin< ::fwData::Mesh::ColorValueType >();

        fwData::Array::SizeType arraySize = itemColors->getSize();

        const size_t nbrVertex = arraySize[0];

        const size_t nbComponents = itemColors->getNumberOfComponents();

        for (size_t numVertex = 0; numVertex < nbrVertex; ++numVertex)
        {
            itemColorsBuffer[numVertex * nbComponents + 0] = colorR;
            itemColorsBuffer[numVertex * nbComponents + 1] = colorG;
            itemColorsBuffer[numVertex * nbComponents + 2] = colorB;
            if (nbComponents == 4)
            {
                itemColorsBuffer[numVertex * nbComponents + 3] = colorA;
            }
        }
    }
    ::fwData::Mesh::PointColorsModifiedSignalType::sptr sig;
    sig = mesh->signal< ::fwData::Mesh::PointColorsModifiedSignalType >(
        ::fwData::Mesh::s_POINT_COLORS_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void Mesh::colorizeMeshPoints( const ::fwData::Mesh::sptr& _mesh, const std::vector< size_t >& _vectorNumTriangle,
                               const std::uint8_t _colorR, const std::uint8_t _colorG, const std::uint8_t _colorB,
                               const std::uint8_t _colorA)
{
    ::fwDataTools::helper::ArrayGetter helperCells(_mesh->getCellDataArray());
    const ::fwData::Mesh::CellValueType* cellsMesh = helperCells.begin< ::fwData::Mesh::CellValueType >();

    ::fwData::Array::sptr itemColors = _mesh->getPointColorsArray();
    SLM_ASSERT("color array must be allocated", itemColors);

    ::fwDataTools::helper::Array hArray( itemColors );

    ::fwData::Mesh::ColorValueType* itemColorsBuffer =
        static_cast< ::fwData::Mesh::ColorValueType* >( hArray.getBuffer() );

    const size_t nbrTriangle  = _vectorNumTriangle.size();
    const size_t nbComponents = itemColors->getNumberOfComponents();

    for (size_t numTriangle = 0; numTriangle < nbrTriangle; ++numTriangle)
    {
        const size_t indiceTriangle = _vectorNumTriangle[numTriangle];

        const size_t indexPoint0 = cellsMesh[indiceTriangle * 3 + 0];
        const size_t indexPoint1 = cellsMesh[indiceTriangle * 3 + 1];
        const size_t indexPoint2 = cellsMesh[indiceTriangle * 3 + 2];

        itemColorsBuffer[indexPoint0 * nbComponents + 0] = _colorR;
        itemColorsBuffer[indexPoint0 * nbComponents + 1] = _colorG;
        itemColorsBuffer[indexPoint0 * nbComponents + 2] = _colorB;

        itemColorsBuffer[indexPoint1 * nbComponents + 0] = _colorR;
        itemColorsBuffer[indexPoint1 * nbComponents + 1] = _colorG;
        itemColorsBuffer[indexPoint1 * nbComponents + 2] = _colorB;

        itemColorsBuffer[indexPoint2 * nbComponents + 0] = _colorR;
        itemColorsBuffer[indexPoint2 * nbComponents + 1] = _colorG;
        itemColorsBuffer[indexPoint2 * nbComponents + 2] = _colorB;

        if (nbComponents == 4)
        {
            itemColorsBuffer[indexPoint0 * nbComponents + 3] = _colorA;
            itemColorsBuffer[indexPoint1 * nbComponents + 3] = _colorA;
            itemColorsBuffer[indexPoint2 * nbComponents + 3] = _colorA;
        }
    }
    ::fwData::Mesh::PointColorsModifiedSignalType::sptr sig;
    sig = _mesh->signal< ::fwData::Mesh::PointColorsModifiedSignalType >(
        ::fwData::Mesh::s_POINT_COLORS_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void Mesh::colorizeMeshCells(
    const ::fwData::Mesh::sptr& mesh,
    const std::uint8_t colorR,
    const std::uint8_t colorG,
    const std::uint8_t colorB,
    const std::uint8_t colorA)
{
    ::fwData::Array::sptr itemColors = mesh->getCellColorsArray();
    SLM_ASSERT("color array must be allocated", itemColors);

    ::fwDataTools::helper::Array hArray( itemColors );

    ::fwData::Mesh::ColorValueType* itemColorsBuffer =
        static_cast< ::fwData::Mesh::ColorValueType* >( hArray.getBuffer() );

    size_t triangleNbr        = mesh->getNumberOfCells();
    const size_t nbComponents = itemColors->getNumberOfComponents();

    for (size_t triangleNum = 0; triangleNum < triangleNbr; ++triangleNum)
    {
        itemColorsBuffer[triangleNum * nbComponents + 0] = colorR;
        itemColorsBuffer[triangleNum * nbComponents + 1] = colorG;
        itemColorsBuffer[triangleNum * nbComponents + 2] = colorB;
        if (nbComponents == 4)
        {
            itemColorsBuffer[triangleNum * nbComponents + 3] = colorA;
        }
    }

    ::fwData::Mesh::CellColorsModifiedSignalType::sptr sig;
    sig = mesh->signal< ::fwData::Mesh::CellColorsModifiedSignalType >(
        ::fwData::Mesh::s_CELL_COLORS_MODIFIED_SIG);
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

void Mesh::colorizeMeshCells(
    const ::fwData::Mesh::sptr& mesh,
    const std::vector < size_t >& triangleIndexVector,
    const std::uint8_t colorR,
    const std::uint8_t colorG,
    const std::uint8_t colorB,
    const std::uint8_t colorA)
{
    ::fwData::Array::sptr itemColors = mesh->getCellColorsArray();
    SLM_ASSERT("color array must be allocated", itemColors);

    ::fwDataTools::helper::Array hArray( itemColors );

    ::fwData::Mesh::ColorValueType* itemColorsBuffer =
        static_cast< ::fwData::Mesh::ColorValueType* >( hArray.getBuffer() );

    const size_t triangleNbr  = triangleIndexVector.size();
    const size_t nbComponents = itemColors->getNumberOfComponents();

    for (size_t triangleNum = 0; triangleNum < triangleNbr; ++triangleNum)
    {
        const size_t triangleIndex = triangleIndexVector[triangleNum];
        itemColorsBuffer[triangleIndex * nbComponents + 0] = colorR;
        itemColorsBuffer[triangleIndex * nbComponents + 1] = colorG;
        itemColorsBuffer[triangleIndex * nbComponents + 2] = colorB;
        if (nbComponents == 4)
        {
            itemColorsBuffer[triangleIndex * nbComponents + 3] = colorA;
        }
    }

    ::fwData::Mesh::CellColorsModifiedSignalType::sptr sig;
    sig = mesh->signal< ::fwData::Mesh::CellColorsModifiedSignalType >(
        ::fwData::Mesh::s_CELL_COLORS_MODIFIED_SIG);
    sig->asyncEmit();
}
//------------------------------------------------------------------------------

} // namespace fwDataTools
