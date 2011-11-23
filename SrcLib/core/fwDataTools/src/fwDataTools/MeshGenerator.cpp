/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwDataTools/thread/RegionThreader.hpp"

#include <map>

#include <cstdlib>
#include <ctime>

#include <boost/foreach.hpp>

#include "fwDataTools/MeshGenerator.hpp"


namespace fwDataTools
{

struct RandFloat{
    float operator()()
    { return ((rand()%101-50.f))/500.f; };
} ;

//------------------------------------------------------------------------------

MeshGenerator::MeshGenerator()
{}

//------------------------------------------------------------------------------

MeshGenerator::~MeshGenerator()
{}

//------------------------------------------------------------------------------
void MeshGenerator::initRand()
{
    std::srand(std::time(NULL));
}

//------------------------------------------------------------------------------

void MeshGenerator::generateTriangleQuadMesh(::fwData::Mesh::sptr mesh)
{
    size_t nbPointsByEdge = 10;
    float edgeDim = 100.;
    MeshGenerator::PointsMapType points;

    mesh->clear();
    MeshGenerator::addTriangleMesh(mesh, points, nbPointsByEdge, edgeDim);
    MeshGenerator::addQuadMesh(mesh, points, nbPointsByEdge, edgeDim);
}

//------------------------------------------------------------------------------

void MeshGenerator::generateTriangleMesh(::fwData::Mesh::sptr mesh)
{
    size_t nbPointsByEdge = 10;
    float edgeDim = 100.;
    MeshGenerator::PointsMapType points;

    mesh->clear();
    MeshGenerator::addTriangleMesh(mesh, points, nbPointsByEdge, edgeDim);
}

//------------------------------------------------------------------------------

void MeshGenerator::generateQuadMesh(::fwData::Mesh::sptr mesh)
{
    size_t nbPointsByEdge = 10;
    float edgeDim = 100.;
    MeshGenerator::PointsMapType points;

    mesh->clear();
    MeshGenerator::addQuadMesh(mesh, points, nbPointsByEdge, edgeDim);
}
//------------------------------------------------------------------------------

void MeshGenerator::addQuadMesh(::fwData::Mesh::sptr mesh, PointsMapType& points, size_t nbPointsByEdge, float edgeDim)
{
    ::fwData::Mesh::PointValueType pt1[3], pt2[3], pt3[3], pt4[3];
    ::fwData::Mesh::Id idx1, idx2, idx3, idx4;
    float step = edgeDim / nbPointsByEdge;

    //Face Y = edgeDim
    for(size_t x=0; x<nbPointsByEdge; x++)
    {
        for(size_t z=0; z<nbPointsByEdge; z++)
        {
            pt1[0] = x*step;
            pt1[1] = edgeDim;
            pt1[2] = z*step;

            pt2[0] = (x+1)*step;
            pt2[1] = edgeDim;
            pt2[2] = (z)*step;

            pt3[0] = x*step;
            pt3[1] = edgeDim;
            pt3[2] = (z+1)*step;

            pt4[0] = (x+1)*step;
            pt4[1] = edgeDim;
            pt4[2] = (z+1)*step;

            idx1 = MeshGenerator::addPoint(pt1, mesh, points);
            idx2 = MeshGenerator::addPoint(pt2, mesh, points);
            idx3 = MeshGenerator::addPoint(pt3, mesh, points);
            idx4 = MeshGenerator::addPoint(pt4, mesh, points);

            mesh->insertNextCell(idx1, idx3, idx4, idx2);
        }
    }

    //Face X = edgeDim
    for(size_t y=0; y<nbPointsByEdge; y++)
    {
        for(size_t z=0; z<nbPointsByEdge; z++)
        {
            pt1[0] = edgeDim;
            pt1[1] = y*step;
            pt1[2] = z*step;

            pt2[0] = edgeDim;
            pt2[1] = y*step;
            pt2[2] = (z+1)*step;

            pt3[0] = edgeDim;
            pt3[1] = (y+1)*step;
            pt3[2] = z*step;

            pt4[0] = edgeDim;
            pt4[1] = (y+1)*step;
            pt4[2] = (z+1)*step;

            idx1 = MeshGenerator::addPoint(pt1, mesh, points);
            idx2 = MeshGenerator::addPoint(pt2, mesh, points);
            idx3 = MeshGenerator::addPoint(pt3, mesh, points);
            idx4 = MeshGenerator::addPoint(pt4, mesh, points);

            mesh->insertNextCell(idx1, idx3, idx4, idx2);
        }
    }
}

//------------------------------------------------------------------------------

void MeshGenerator::addTriangleMesh(::fwData::Mesh::sptr mesh, PointsMapType& points, size_t nbPointsByEdge, float edgeDim)
{
    ::fwData::Mesh::PointValueType pt1[3], pt2[3], pt3[3], pt4[3];
    ::fwData::Mesh::Id idx1, idx2, idx3, idx4;
    float step = edgeDim / nbPointsByEdge;

    //Face Z = 0
    for(size_t x=0; x<nbPointsByEdge; x++)
    {
        for(size_t y=0; y<nbPointsByEdge; y++)
        {
            pt1[0] = x*step;
            pt1[1] = y*step;
            pt1[2] = 0;

            pt2[0] = (x+1)*step;
            pt2[1] = y*step;
            pt2[2] = 0;

            pt3[0] = x*step;
            pt3[1] = (y+1)*step;
            pt3[2] = 0;

            pt4[0] = (x+1)*step;
            pt4[1] = (y+1)*step;
            pt4[2] = 0;

            idx1 = MeshGenerator::addPoint(pt1, mesh, points);
            idx2 = MeshGenerator::addPoint(pt2, mesh, points);
            idx3 = MeshGenerator::addPoint(pt3, mesh, points);
            idx4 = MeshGenerator::addPoint(pt4, mesh, points);

            mesh->insertNextCell(idx1, idx4, idx2);
            mesh->insertNextCell(idx1, idx3, idx4);
        }
    }

    //Face X = 0
    for(size_t y=0; y<nbPointsByEdge; y++)
    {
        for(size_t z=0; z<nbPointsByEdge; z++)
        {
            pt1[0] = 0;
            pt1[1] = y*step;
            pt1[2] = z*step;

            pt2[0] = 0;
            pt2[1] = y*step;
            pt2[2] = (z+1)*step;

            pt3[0] = 0;
            pt3[1] = (y+1)*step;
            pt3[2] = z*step;

            pt4[0] = 0;
            pt4[1] = (y+1)*step;
            pt4[2] = (z+1)*step;

            idx1 = MeshGenerator::addPoint(pt1, mesh, points);
            idx2 = MeshGenerator::addPoint(pt2, mesh, points);
            idx3 = MeshGenerator::addPoint(pt3, mesh, points);
            idx4 = MeshGenerator::addPoint(pt4, mesh, points);

            mesh->insertNextCell(idx2, idx4, idx3);
            mesh->insertNextCell(idx1, idx2, idx3);
        }
    }
}

//------------------------------------------------------------------------------

void MeshGenerator::toTriangularMesh(::fwData::Mesh::sptr mesh, ::fwData::TriangularMesh::sptr trian)
{
    FW_RAISE_IF("Can't convert this Mesh to TriangularMesh", !MeshGenerator::hasUniqueCellType(mesh, ::fwData::Mesh::TRIANGLE));

    // Clear the container cells and set its capacity to 0
    trian->clearCells();
    // Clear the container points and set its capacity to 0
    trian->clearPoints();

    size_t numberOfPoints = mesh->getNumberOfPoints();
    std::vector<float> vPoint(3, 0.0);
    trian->points().resize(numberOfPoints, vPoint);

    ::fwData::Mesh::PointsMultiArrayType points = mesh->getPoints();
    typedef ::fwData::Mesh::PointsMultiArrayType::index PointTypesIndex;
    for (PointTypesIndex i = 0; i != numberOfPoints; ++i)
    {
        ::fwData::TriangularMesh::PointContainer::value_type &point = trian->points()[i];
        for (PointTypesIndex j = 0; j != 3; ++j)
        {
            point[j] = points[i][j];
        }
    }

    size_t numberOfCells = mesh->getNumberOfCells();
    std::vector<int> vCell(3, 0);
    trian->cells().resize(numberOfCells, vCell);

    ::fwData::Mesh::CellDataMultiArrayType cells = mesh->getCellData();
    typedef ::fwData::Mesh::CellDataMultiArrayType::index CellTypesIndex;
    size_t cellsSize = numberOfCells*3;
    SLM_ASSERT("Wrong CellDataMultiArray size", cells.size() == cellsSize);
    for (CellTypesIndex i = 0; i < cellsSize; i+=3)
    {
        ::fwData::TriangularMesh::CellContainer::value_type &vCells = trian->cells()[i/3];
        vCells[0] = cells[i];
        vCells[1] = cells[i+1];
        vCells[2] = cells[i+2];
    }
}

//------------------------------------------------------------------------------

void MeshGenerator::fromTriangularMesh(::fwData::TriangularMesh::sptr trian, ::fwData::Mesh::sptr mesh)
{
    ::fwData::TriangularMesh::PointContainer& vPoints = trian->points();
    ::fwData::TriangularMesh::CellContainer& vCells = trian->cells();

    mesh->clear();
    mesh->allocate(vPoints.size(), vCells.size(), vCells.size()*3);

    ::fwData::Array::sptr pointsArray = mesh->getPointsArray();
    ::fwData::Array::sptr cellsArray = mesh->getCellDataArray();
    ::fwData::Array::sptr cellDataOffsetArray = mesh->getCellDataOffsetsArray();
    ::fwData::Array::sptr cellTypeArray = mesh->getCellTypesArray();

    SLM_ASSERT("Wrong CellTypeArray dim", cellTypeArray->getSize().size()==1);
    SLM_ASSERT("Wrong CellTypeArray size", cellTypeArray->getSize()[0]==vCells.size());
    SLM_ASSERT("Wrong pointsArray dim", pointsArray->getSize().size()==1);
    SLM_ASSERT("Wrong pointsArray size", pointsArray->getSize()[0]==vPoints.size());
    SLM_ASSERT("Wrong cellsArray dim", cellsArray->getSize().size()==1);
    SLM_ASSERT("Wrong cellsArray size", cellsArray->getSize()[0]==vCells.size()*3);
    SLM_ASSERT("Wrong cellDataOffsetArray dim", cellDataOffsetArray->getSize().size()==1);
    SLM_ASSERT("Wrong cellDataOffsetArray size", cellDataOffsetArray->getSize()[0]==vCells.size());

    //Initialized pointsArray
    for(size_t i=0; i<vPoints.size(); i++)
    {
        mesh->insertNextPoint(vPoints[i][0], vPoints[i][1], vPoints[i][2]);
    }

    //Initialized cellsArray
    ::fwData::Mesh::CellValueType* cells = cellsArray->begin< ::fwData::Mesh::CellValueType >();
    for(size_t i=0; i<vCells.size(); i++)
    {
        mesh->insertNextCell(vCells[i][0], vCells[i][1], vCells[i][2]);
    }
    mesh->adjustAllocatedMemory();
}

//------------------------------------------------------------------------------

bool MeshGenerator::hasUniqueCellType(::fwData::Mesh::sptr mesh, ::fwData::Mesh::CellTypes cell)
{
    bool res = true;
    ::fwData::Mesh::CellTypesMultiArrayType cellTypes = mesh->getCellTypes();

    BOOST_FOREACH(::fwData::Mesh::CellTypes type, cellTypes)
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


Vector<float> &computeTriangleNormal(const Point &p1, const Point &p2, const Point &p3, Vector<float> &n)
{
    n = Vector<float>(p1, p2);
    Vector<float> v(p1, p3);
    n.crossWith(v);
    n.normalize();
    return n;
}

//------------------------------------------------------------------------------

Vector<float> &computeTriangleNormal( const PointsMultiArrayType &points, const ::fwData::Mesh::CellValueType *cell, Vector<float> &n)
{
    const Point &p1 = points[cell[0]];
    const Point &p2 = points[cell[1]];
    const Point &p3 = points[cell[2]];

    computeTriangleNormal(p1, p2, p3, n);
    return n;
}

//------------------------------------------------------------------------------

Vector<float> &computeCellNormal( const PointsMultiArrayType &points, const ::fwData::Mesh::CellValueType *cell, size_t cellSize, Vector<float> &n)
{
    n = Vector<float>();
    Vector<float> v;

    for (size_t i=0; i< cellSize; ++i)
    {
        const Point &p1 = points[cell[i  ]];
        const Point &p2 = points[cell[(i+1)% cellSize]];
        const Point &p3 = points[cell[(i+2)% cellSize]];

        computeTriangleNormal(p1, p2, p3, n);

        n += v;
    }

    n /= cellSize;
    n.normalize();
    return n;
}

//------------------------------------------------------------------------------

void generateRegionCellNormals(::fwData::Mesh::sptr mesh, const ::fwData::Mesh::Id regionMin, const ::fwData::Mesh::Id regionMax)
{
    PointsMultiArrayType point = PointsMultiArrayType(
            static_cast<PointsMultiArrayType::element*>(mesh->getPointsArray()->getBuffer()),
            boost::extents[mesh->getNumberOfPoints()]
            );


    ::fwData::Mesh::CellTypesMultiArrayType       cellTypes       = mesh->getCellTypes();
    ::fwData::Mesh::CellDataMultiArrayType        cellData        = mesh->getCellData();
    ::fwData::Mesh::CellDataOffsetsMultiArrayType cellDataOffsets = mesh->getCellDataOffsets();

    const Vector<float> vZero;
    Vector<float> n;
    ::fwData::Mesh::CellTypes type;
    ::fwData::Mesh::CellDataOffsetType offset;
    ::fwData::Mesh::CellValueType *cell;
    ::fwData::Mesh::Id cellLen = 0;

    const ::fwData::Mesh::Id numberOfCells = mesh->getNumberOfCells();
    const ::fwData::Mesh::Id cellDataSize = mesh->getCellDataSize();

    for(::fwData::Mesh::Id i = regionMin; i<regionMax; ++i)
    {
        type = cellTypes[i];
        offset = cellDataOffsets[i];
        cell = &cellData[offset];
        switch (type)
        {
            case 0:
            case 1:
            case 2:
                n = vZero;
                mesh->setCellNormal(i, reinterpret_cast< ::fwData::Mesh::NormalValueType* >(&n));
                break;
            case 3:
                {
                    computeTriangleNormal(point, cell, n);

                    mesh->setCellNormal(i, reinterpret_cast< ::fwData::Mesh::NormalValueType* >(&n));
                }
                break;
            case 4:
            case 5:
                {
                    const ::fwData::Mesh::Id i1 = i+1;
                    cellLen = (( i1 < numberOfCells )? cellDataOffsets[i1]:cellDataSize) - cellDataOffsets[i];

                    computeCellNormal(point, cell, cellLen, n);

                    mesh->setCellNormal(i, reinterpret_cast< ::fwData::Mesh::NormalValueType* >(&n));
                }
        }
    }
}




//------------------------------------------------------------------------------

void MeshGenerator::generateCellNormals(::fwData::Mesh::sptr mesh)
{
    mesh->allocateCellNormals();

    const ::fwData::Mesh::Id numberOfCells = mesh->getNumberOfCells();

    ::fwDataTools::thread::RegionThreader rt;
    rt( ::boost::bind(&generateRegionCellNormals, mesh, _1, _2), numberOfCells );

    return;

}


typedef std::multimap< ::fwData::Mesh::CellValueType, Vector<float> > VectorMultiMap;

void generateRegionCellNormalsByPoints(std::vector< VectorMultiMap > &vectorsData, size_t dataId, ::fwData::Mesh::sptr mesh, const ::fwData::Mesh::Id regionMin, const ::fwData::Mesh::Id regionMax)
{
    VectorMultiMap &vectors = vectorsData[dataId];

    const ::fwData::Mesh::Id nbOfPoints = mesh->getNumberOfPoints();
    PointsMultiArrayType point = PointsMultiArrayType(
            static_cast<PointsMultiArrayType::element*>(mesh->getPointsArray()->getBuffer()),
            boost::extents[nbOfPoints]
            );

    ::fwData::Mesh::CellTypesMultiArrayType       cellTypes       = mesh->getCellTypes();
    ::fwData::Mesh::CellDataMultiArrayType        cellData        = mesh->getCellData();
    ::fwData::Mesh::CellDataOffsetsMultiArrayType cellDataOffsets = mesh->getCellDataOffsets();


    const Vector<float> vZero;
    Vector<float> n;
    ::fwData::Mesh::CellTypes type;
    ::fwData::Mesh::CellDataOffsetType offset;
    ::fwData::Mesh::CellValueType *cell;
    ::fwData::Mesh::Id cellLen = 0;

    const ::fwData::Mesh::Id numberOfCells = mesh->getNumberOfCells();
    const ::fwData::Mesh::Id cellDataSize = mesh->getCellDataSize();

    for(::fwData::Mesh::Id i = regionMin; i<regionMax; ++i)
    {
        type = cellTypes[i];
        offset = cellDataOffsets[i];
        cell = &cellData[offset];
        cellLen = type;
        switch (type)
        {
            case 0:
            case 1:
            case 2:
                n = vZero;
                break;
            case 3:
                computeTriangleNormal(point, cell, n);
                break;
            case 4:
            case 5:
                {
                    const ::fwData::Mesh::Id i1 = i+1;
                    cellLen = (( i1 < numberOfCells )? cellDataOffsets[i1]:cellDataSize) - cellDataOffsets[i];

                    computeCellNormal(point, cell, cellLen, n);
                }
        }

        ::fwData::Mesh::CellValueType *pointId;
        ::fwData::Mesh::CellValueType *cellEnd = cell + cellLen;
        for(pointId = cell; pointId != cellEnd; ++pointId)
        {
            vectors.insert(VectorMultiMap::value_type(*pointId, n));
        }

    }
}

void normalizeRegionCellNormals(VectorMultiMap &vectors, ::fwData::Mesh::sptr mesh, const ::fwData::Mesh::Id regionMin, const ::fwData::Mesh::Id regionMax)
{
    const Vector<float> vZero;
    Vector<float> n;


    VectorMultiMap::iterator it;
    std::pair<VectorMultiMap::iterator,VectorMultiMap::iterator> ret;

    size_t count = 0;

    for(::fwData::Mesh::Id i = regionMin; i<regionMax; ++i)
    {
        ret = vectors.equal_range(i);
        n = vZero;
        count = 0;

        for (it=ret.first; it!=ret.second; ++it, ++count)
        {
            Vector<float> &v = it->second;
            n += it->second;
        }
        if (count>0)
        {
            n /= count;
        }

        n.normalize();
        mesh->setPointNormal(i, reinterpret_cast< ::fwData::Mesh::NormalValueType* >(&n));
    }
}

//------------------------------------------------------------------------------

void MeshGenerator::generatePointNormals(::fwData::Mesh::sptr mesh)
{

    mesh->allocatePointNormals();

    std::vector< VectorMultiMap > cellNormalsByPoints;
    VectorMultiMap vectors;


    ::fwDataTools::thread::RegionThreader rt;

    cellNormalsByPoints.resize(rt.numberOfThread());

    const ::fwData::Mesh::Id numberOfCells = mesh->getNumberOfCells();
    const ::fwData::Mesh::Id nbOfPoints = mesh->getNumberOfPoints();

    rt( ::boost::bind(&generateRegionCellNormalsByPoints,
                        boost::ref(cellNormalsByPoints),
                        _3, mesh, _1, _2),
            numberOfCells);


    std::vector< VectorMultiMap >::iterator iter = cellNormalsByPoints.begin();

    vectors.swap(*iter++);

    for(; iter != cellNormalsByPoints.end(); ++iter)
    {
        VectorMultiMap &mm = *iter;
        vectors.insert(mm.begin(), mm.end());
    }

    rt( boost::bind( &normalizeRegionCellNormals,
                 boost::ref(vectors),
                 mesh, _1, _2),
            nbOfPoints);

}

//------------------------------------------------------------------------------

void MeshGenerator::shakeNormals(::fwData::Array::sptr array)
{
    void *buf;
    if(array
            && array->getType() == ::fwTools::Type::create<float>()
            && (buf = array->getBuffer())
            && array->getNumberOfComponents() == 3
            && array->getNumberOfDimensions() == 1
            )
    {
        typedef boost::multi_array_ref<Vector<float>, 1> NormalsMultiArrayType;
        const ::fwData::Mesh::Id nbOfNormals = array->getSize().at(0);
        NormalsMultiArrayType normals = NormalsMultiArrayType(
                static_cast<NormalsMultiArrayType::element*>(buf),
                boost::extents[nbOfNormals]
                );


        RandFloat randFloat;
        for (::fwData::Mesh::Id i = 0; i < nbOfNormals; ++i)
        {
            Vector<float> v(randFloat(), randFloat(), randFloat());
            normals[i] += v;
            normals[i].normalize();
        }
    }
}

//------------------------------------------------------------------------------

void MeshGenerator::shakePointNormals(::fwData::Mesh::sptr mesh)
{
    shakeNormals(mesh->getPointNormalsArray());
}


//------------------------------------------------------------------------------

void MeshGenerator::shakeCellNormals(::fwData::Mesh::sptr mesh)
{
    shakeNormals(mesh->getCellNormalsArray());
}

//------------------------------------------------------------------------------

void MeshGenerator::colorizeMeshPoints(::fwData::Mesh::sptr mesh)
{
    mesh->allocatePointColors(::fwData::Mesh::RGB);
    ::fwData::Mesh::ColorValueType color[4];
    size_t numberOfPoints = mesh->getNumberOfPoints();
    for(size_t i = 0; i<numberOfPoints; ++i)
    {
        color[0] = rand()%256;
        color[1] = 0;//rand()%256;
        color[2] = rand()%256;
        mesh->setPointColor(i, color);
    }
}
//------------------------------------------------------------------------------

void MeshGenerator::colorizeMeshCells(::fwData::Mesh::sptr mesh)
{
    mesh->allocateCellColors(::fwData::Mesh::RGBA);
    ::fwData::Mesh::ColorValueType color[4];
    size_t numberOfCells = mesh->getNumberOfCells();
    for(size_t i = 0; i<numberOfCells; ++i)
    {
        color[0] = rand()%256;
        color[1] = rand()%256;
        color[2] = 0;//rand()%256;
        color[3] = rand()%256;
        mesh->setCellColor(i, color);
    }
}

//------------------------------------------------------------------------------

void MeshGenerator::shakePoint(::fwData::Mesh::sptr mesh)
{
    size_t nbPts = mesh->getNumberOfPoints();
    ::fwData::Mesh::PointsMultiArrayType points = mesh->getPoints();
    RandFloat randFloat;
    for(size_t i=0 ; i<nbPts ; ++i )
    {
        points[i][0] += randFloat()*5;
        points[i][1] += randFloat()*5;
        points[i][2] += randFloat()*5;
    }
}

//------------------------------------------------------------------------------

::fwData::Mesh::Id MeshGenerator::addPoint(::fwData::Mesh::PointValueType* pt, ::fwData::Mesh::sptr mesh, PointsMapType& points)
{
    Point myPoint(pt[0], pt[1], pt[2]);

    PointsMapType::iterator it = points.find(myPoint);
    if(it != points.end())
    {
        return it->second;
    }
    ::fwData::Mesh::Id idx = mesh->insertNextPoint(pt[0], pt[1], pt[2]);
    points[myPoint] = idx;
    return idx;
}

//------------------------------------------------------------------------------

} // namespace fwDataTools
