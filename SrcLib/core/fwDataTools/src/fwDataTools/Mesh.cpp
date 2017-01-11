/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataTools/Mesh.hpp"

#include "fwDataTools/thread/RegionThreader.hpp"

#include <fwTools/NumericRoundCast.hxx>

#include <cstdlib>
#include <ctime>
#include <functional>
#include <map>

namespace fwDataTools
{

struct RandFloat
{
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

    for (size_t i = 0; i< cellSize; ++i)
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

    for(::fwData::Mesh::Id i = regionMin; i<regionMax; ++i)
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
        for (size_t i = regionMin; i<regionMax; ++i)
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

    for(::fwData::Mesh::Id i = regionMin; i<regionMax; ++i)
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

        if(count>1)
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
    for(size_t i = 0; i<numberOfPoints; ++i)
    {
        color[0] = rand()%256;
        color[1] = 0;//rand()%256;
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
    for(size_t i = 0; i<numberOfCells; ++i)
    {
        color[0] = rand()%256;
        color[1] = rand()%256;
        color[2] = 0;//rand()%256;
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
    for(size_t i = 0; i<nbPts; ++i )
    {
        points[i][0] += randFloat()*5;
        points[i][1] += randFloat()*5;
        points[i][2] += randFloat()*5;
    }
}

//------------------------------------------------------------------------------

void Mesh::transform( ::fwData::Mesh::sptr mesh, ::fwData::TransformationMatrix3D::csptr t )
{
    size_t nbPts = mesh->getNumberOfPoints();
    ::fwDataTools::helper::Mesh meshHelper(mesh);
    ::fwData::Mesh::PointsMultiArrayType points = meshHelper.getPoints();
    ::fwData::Mesh::PointValueType x, y, z, xp, yp, zp, factor;
    for(size_t i = 0; i<nbPts; ++i )
    {
        x  = points[i][0];
        y  = points[i][1];
        z  = points[i][2];
        xp = ::fwTools::numericRoundCast< ::fwData::Mesh::PointValueType >(t->getCoefficient(0,0) * x
                                                                           + t->getCoefficient(0,1) * y
                                                                           + t->getCoefficient(0,2) * z
                                                                           + t->getCoefficient(0,3));
        yp = ::fwTools::numericRoundCast< ::fwData::Mesh::PointValueType >(t->getCoefficient(1,0) * x
                                                                           + t->getCoefficient(1,1) * y
                                                                           + t->getCoefficient(1,2) * z
                                                                           + t->getCoefficient(1,3));
        zp = ::fwTools::numericRoundCast< ::fwData::Mesh::PointValueType >(t->getCoefficient(2,0) * x
                                                                           + t->getCoefficient(2,1) * y
                                                                           + t->getCoefficient(2,2) * z
                                                                           + t->getCoefficient(2,3));
        factor = ::fwTools::numericRoundCast< ::fwData::Mesh::PointValueType >(t->getCoefficient(3,0) * x
                                                                               + t->getCoefficient(3,1) * y
                                                                               + t->getCoefficient(3,2) * z
                                                                               + t->getCoefficient(3,3));
        points[i][0] = xp/factor;
        points[i][1] = yp/factor;
        points[i][2] = zp/factor;
    }
}

//------------------------------------------------------------------------------

} // namespace fwDataTools
