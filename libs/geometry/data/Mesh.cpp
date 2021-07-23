/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "geometry/data/Mesh.hpp"

#include "data/thread/RegionThreader.hpp"

#include "geometry/data/Matrix4.hpp"

#include <core/com/Signal.hxx>
#include <core/tools/NumericRoundCast.hxx>

#include <geometry/data/MeshFunctions.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cstdlib>
#include <ctime>
#include <functional>
#include <map>

namespace sight::geometry::data
{

struct RandFloat
{
    //------------------------------------------------------------------------------

    float operator()()
    {
        return ((static_cast<float>(rand() % 101) - 50.f)) / 500.f;
    }
};

//------------------------------------------------------------------------------

void Mesh::initRand()
{
    std::srand(core::tools::numericRoundCast<unsigned int>(std::time(NULL)));
}

//------------------------------------------------------------------------------

bool Mesh::hasUniqueCellType(sight::data::Mesh::csptr mesh, sight::data::Mesh::CellTypes cell)
{
    return hasUniqueCellType(mesh, static_cast<sight::data::Mesh::CellTypes>(cell));
}

//------------------------------------------------------------------------------

bool Mesh::hasUniqueCellType(sight::data::Mesh::csptr mesh, sight::data::Mesh::CellType cell)
{
    bool res            = true;
    const auto dumpLock = mesh->lock();

    auto itr          = mesh->begin<sight::data::iterator::ConstCellIterator>();
    const auto itrEnd = mesh->end<sight::data::iterator::ConstCellIterator>();

    for( ; itr != itrEnd ; ++itr)
    {
        if(*itr->type != cell)
        {
            return false;
        }
    }

    return res;
}

//------------------------------------------------------------------------------

Vector<float> computeTriangleNormal(const Point& p1, const Point& p2, const Point& p3)
{
    Vector<float> n(p1, p2);
    Vector<float> v(p1, p3);
    n.crossWith(v);
    n.normalize();
    return n;
}

//------------------------------------------------------------------------------

void generateRegionCellNormals(
    const sight::data::Mesh::sptr& mesh,
    const sight::data::Mesh::CellId regionMin,
    const sight::data::Mesh::CellId regionMax
)
{
    const auto pointBegin = mesh->begin<sight::data::iterator::ConstPointIterator>();

    auto cellItr          = mesh->begin<sight::data::iterator::CellIterator>() + regionMin;
    const auto cellItrEnd = mesh->begin<sight::data::iterator::CellIterator>() + regionMax;

    const Vector<float> vZero;

    for( ; cellItr != cellItrEnd ; ++cellItr)
    {
        Vector<float> n;

        const sight::data::Mesh::CellType type = *cellItr->type;
        switch(type)
        {
            case sight::data::Mesh::CellType::NO_CELL:
            case sight::data::Mesh::CellType::POINT:
            case sight::data::Mesh::CellType::EDGE:
                n = vZero;
                break;

            case sight::data::Mesh::CellType::TRIANGLE:
            {
                auto pItr = pointBegin + cellItr->pointIdx[0];
                const Point p1(pItr->point->x, pItr->point->y, pItr->point->z);
                pItr = pointBegin + cellItr->pointIdx[1];
                const Point p2(pItr->point->x, pItr->point->y, pItr->point->z);
                pItr = pointBegin + cellItr->pointIdx[2];
                const Point p3(pItr->point->x, pItr->point->y, pItr->point->z);
                n = computeTriangleNormal(p1, p2, p3);
                break;
            }

            case sight::data::Mesh::CellType::QUAD:
            case sight::data::Mesh::CellType::POLY:
            case sight::data::Mesh::CellType::TETRA:
            {
                const auto nbPoints = cellItr->nbPoints;
                for(size_t i = 0 ; i < nbPoints ; ++i)
                {
                    Vector<float> v;
                    auto pItr = pointBegin + cellItr->pointIdx[i];
                    const Point p1(pItr->point->x, pItr->point->y, pItr->point->z);
                    pItr = pointBegin + cellItr->pointIdx[(i + 1) % nbPoints];
                    const Point p2(pItr->point->x, pItr->point->y, pItr->point->z);
                    pItr = pointBegin + cellItr->pointIdx[(i + 2) % nbPoints];
                    const Point p3(pItr->point->x, pItr->point->y, pItr->point->z);

                    v = computeTriangleNormal(p1, p2, p3);

                    n += v;
                }

                n /= core::tools::numericRoundCast<float>(nbPoints);
                n.normalize();
            }
        }

        cellItr->normal->nx = n.x;
        cellItr->normal->ny = n.y;
        cellItr->normal->nz = n.z;
    }
}

//------------------------------------------------------------------------------

template<typename T>
void vectorSum(std::vector<std::vector<T> >& vectors, size_t regionMin, size_t regionMax)
{
    if(vectors.empty())
    {
        return;
    }

    typename std::vector<std::vector<T> >::iterator vIter = vectors.begin();

    std::vector<T>& res = vectors[0];

    for(++vIter ; vIter != vectors.end() ; ++vIter)
    {
        for(size_t i = regionMin ; i < regionMax ; ++i)
        {
            res[i] += (*vIter)[i];
        }
    }
}

//------------------------------------------------------------------------------

void Mesh::generateCellNormals(sight::data::Mesh::sptr mesh)
{
    const sight::data::Mesh::Size numberOfCells = mesh->getNumberOfCells();
    if(numberOfCells > 0)
    {
        if(!mesh->hasCellNormals())
        {
            mesh->resize(
                mesh->getNumberOfPoints(),
                mesh->getNumberOfCells(),
                mesh->getCellDataSize(),
                sight::data::Mesh::Attributes::CELL_NORMALS
            );
        }

        const auto dumpLock = mesh->lock();

        sight::data::thread::RegionThreader rt((numberOfCells >= 200000) ? 4 : 1);
        rt(
            std::bind(&generateRegionCellNormals, mesh, std::placeholders::_1, std::placeholders::_2),
            numberOfCells
        );
    }
}

//------------------------------------------------------------------------------

typedef std::vector<std::vector<float> > FloatVectors;

//------------------------------------------------------------------------------

void generateRegionCellNormalsByPoints(
    FloatVectors& normalsData,
    size_t dataId,
    const sight::data::Mesh::sptr& mesh,
    const sight::data::Mesh::CellId regionMin,
    const sight::data::Mesh::CellId regionMax
)
{
    FloatVectors::value_type& normalsResults = normalsData[dataId];

    const sight::data::Mesh::Size nbOfPoints = mesh->getNumberOfPoints();
    normalsResults.resize(3 * nbOfPoints, 0.f);

    auto cellItr          = mesh->begin<sight::data::iterator::ConstCellIterator>() + regionMin;
    const auto cellItrEnd = mesh->begin<sight::data::iterator::ConstCellIterator>() + regionMax;

    const Vector<float> vZero;

    for( ; cellItr != cellItrEnd ; ++cellItr)
    {
        const auto normal = cellItr->normal;
        for(size_t i = 0 ; i < cellItr->nbPoints ; ++i)
        {
            normalsData[dataId][3 * cellItr->pointIdx[i]]     += normal->nx;
            normalsData[dataId][3 * cellItr->pointIdx[i] + 1] += normal->ny;
            normalsData[dataId][3 * cellItr->pointIdx[i] + 2] += normal->nz;
        }
    }
}

//------------------------------------------------------------------------------

void normalizeRegionCellNormalsByPoints(
    FloatVectors::value_type& normalsData,
    sight::data::Mesh::sptr mesh,
    const sight::data::Mesh::CellId regionMin,
    const sight::data::Mesh::CellId regionMax
)
{
    Vector<sight::data::Mesh::NormalValueType>* normalSum =
        reinterpret_cast<Vector<sight::data::Mesh::NormalValueType>*>(&(*normalsData.begin()));

    auto pointItr          = mesh->begin<sight::data::iterator::PointIterator>() + regionMin;
    const auto pointItrEnd = mesh->begin<sight::data::iterator::PointIterator>() + regionMax;

    for(sight::data::Mesh::CellId i = regionMin ; i < regionMax ; ++i, ++pointItr)
    {
        Vector<sight::data::Mesh::NormalValueType> normal = normalSum[i];

        normal.normalize();
        pointItr->normal->nx = normal.x;
        pointItr->normal->ny = normal.y;
        pointItr->normal->nz = normal.z;
    }
}

//------------------------------------------------------------------------------

void Mesh::generatePointNormals(sight::data::Mesh::sptr mesh)
{
    const sight::data::Mesh::Size nbOfPoints = mesh->getNumberOfPoints();
    if(nbOfPoints > 0)
    {
        const sight::data::Mesh::Size numberOfCells = mesh->getNumberOfCells();

        // To generate point normals, we need to use the cell normals
        if(!mesh->hasCellNormals())
        {
            generateCellNormals(mesh);
        }

        if(!mesh->hasPointNormals())
        {
            mesh->resize(
                mesh->getNumberOfPoints(),
                mesh->getNumberOfCells(),
                mesh->getCellDataSize(),
                sight::data::Mesh::Attributes::POINT_NORMALS
            );
        }

        const auto dumpLock = mesh->lock();

        sight::data::thread::RegionThreader rt((nbOfPoints >= 100000) ? 4 : 1);

        FloatVectors normalsData(rt.numberOfThread());

        rt(
            std::bind(
                &generateRegionCellNormalsByPoints,
                std::ref(normalsData),
                std::placeholders::_3,
                mesh,
                std::placeholders::_1,
                std::placeholders::_2
            ),
            numberOfCells
        );

        rt(
            std::bind(
                &vectorSum<FloatVectors::value_type::value_type>,
                std::ref(normalsData),
                std::placeholders::_1,
                std::placeholders::_2
            ),
            nbOfPoints * 3
        );

        rt(
            std::bind(
                &normalizeRegionCellNormalsByPoints,
                std::ref(normalsData[0]),
                mesh,
                std::placeholders::_1,
                std::placeholders::_2
            ),
            nbOfPoints
        );
    }
}

//------------------------------------------------------------------------------

template<typename T>
void regionShakeNormals(T normals, const sight::data::Mesh::CellId regionMin, const sight::data::Mesh::CellId regionMax)
{
    RandFloat randFloat;
    for(sight::data::Mesh::CellId i = regionMin ; i < regionMax ; ++i)
    {
        Vector<float> v(randFloat(), randFloat(), randFloat());
        normals[i] += v;
        normals[i].normalize();
    }
}

//------------------------------------------------------------------------------

void Mesh::shakeNormals(sight::data::Array::sptr array)
{
    if(array
       && array->getType() == core::tools::Type::create<float>()
       && !array->empty()
       && array->getNumberOfComponents() == 3
       && array->getNumberOfDimensions() == 1)
    {
        const auto dumpLock = array->lock();
        void* buf;
        buf = array->getBuffer();
        const auto nbOfNormals = array->getSize().at(0);
        typedef boost::multi_array_ref<Vector<float>, 1> NormalsMultiArrayType;
        NormalsMultiArrayType normals = NormalsMultiArrayType(
            static_cast<NormalsMultiArrayType::element*>(buf),
            ::boost::extents[nbOfNormals]
        );

        sight::data::thread::RegionThreader rt((nbOfNormals >= 150000) ? 4 : 1);
        rt(
            std::bind(
                &regionShakeNormals<NormalsMultiArrayType>,
                std::ref(normals),
                std::placeholders::_1,
                std::placeholders::_2
            ),
            nbOfNormals
        );
    }
}

//------------------------------------------------------------------------------

void Mesh::shakePointNormals(sight::data::Mesh::sptr mesh)
{
    const auto dumpLock = mesh->lock();

    auto pointIter          = mesh->begin<sight::data::iterator::PointIterator>();
    const auto pointIterEnd = mesh->begin<sight::data::iterator::PointIterator>();

    RandFloat randFloat;

    for( ; pointIter != pointIterEnd ; ++pointIter)
    {
        Vector<float> v(randFloat(), randFloat(), randFloat());
        Vector<float> normal(pointIter->normal->nx, pointIter->normal->ny, pointIter->normal->nz);
        normal += v;
        normal.normalize();
        pointIter->normal->nx = normal.x;
        pointIter->normal->ny = normal.y;
        pointIter->normal->nz = normal.z;
    }
}

//------------------------------------------------------------------------------

void Mesh::shakeCellNormals(sight::data::Mesh::sptr mesh)
{
    const auto dumpLock = mesh->lock();

    auto cellIter          = mesh->begin<sight::data::iterator::CellIterator>();
    const auto cellIterEnd = mesh->begin<sight::data::iterator::CellIterator>();

    RandFloat randFloat;

    for( ; cellIter != cellIterEnd ; ++cellIter)
    {
        Vector<float> v(randFloat(), randFloat(), randFloat());
        Vector<float> normal(cellIter->normal->nx, cellIter->normal->ny, cellIter->normal->nz);
        normal += v;
        normal.normalize();
        cellIter->normal->nx = normal.x;
        cellIter->normal->ny = normal.y;
        cellIter->normal->nz = normal.z;
    }
}

//------------------------------------------------------------------------------

void Mesh::colorizeMeshPoints(sight::data::Mesh::sptr mesh)
{
    if(!mesh->hasPointColors())
    {
        mesh->resize(
            mesh->getNumberOfPoints(),
            mesh->getNumberOfCells(),
            mesh->getCellDataSize(),
            sight::data::Mesh::Attributes::POINT_COLORS
        );
    }

    const auto dumpLock = mesh->lock();

    auto itr          = mesh->begin<sight::data::iterator::PointIterator>();
    const auto itrEnd = mesh->end<sight::data::iterator::PointIterator>();

    for( ; itr != itrEnd ; ++itr)
    {
        itr->rgba->r = static_cast<std::uint8_t>(rand() % 256);
        itr->rgba->g = static_cast<std::uint8_t>(rand() % 256);
        itr->rgba->b = static_cast<std::uint8_t>(rand() % 256);
        itr->rgba->a = static_cast<std::uint8_t>(rand() % 256);
    }
}

//------------------------------------------------------------------------------

void Mesh::colorizeMeshCells(sight::data::Mesh::sptr mesh)
{
    if(!mesh->hasCellColors())
    {
        mesh->resize(
            mesh->getNumberOfPoints(),
            mesh->getNumberOfCells(),
            mesh->getCellDataSize(),
            sight::data::Mesh::Attributes::CELL_COLORS
        );
    }

    const auto dumpLock = mesh->lock();

    auto itr          = mesh->begin<sight::data::iterator::CellIterator>();
    const auto itrEnd = mesh->end<sight::data::iterator::CellIterator>();

    for( ; itr != itrEnd ; ++itr)
    {
        itr->rgba->r = static_cast<std::uint8_t>(rand() % 256);
        itr->rgba->g = static_cast<std::uint8_t>(rand() % 256);
        itr->rgba->b = static_cast<std::uint8_t>(rand() % 256);
        itr->rgba->a = static_cast<std::uint8_t>(rand() % 256);
    }
}

//------------------------------------------------------------------------------

void Mesh::shakePoint(sight::data::Mesh::sptr mesh)
{
    RandFloat randFloat;

    const auto dumpLock = mesh->lock();

    auto itr          = mesh->begin<sight::data::iterator::PointIterator>();
    const auto itrEnd = mesh->end<sight::data::iterator::PointIterator>();

    for( ; itr != itrEnd ; ++itr)
    {
        itr->point->x += randFloat() * 5;
        itr->point->y += randFloat() * 5;
        itr->point->z += randFloat() * 5;
    }
}

//------------------------------------------------------------------------------

void Mesh::transform(
    sight::data::Mesh::csptr inMesh,
    sight::data::Mesh::sptr outMesh,
    const sight::data::Matrix4& t
)
{
    const auto inDumpLock  = inMesh->lock();
    const auto outDumpLock = outMesh->lock();

    auto inItr = inMesh->begin<sight::data::iterator::ConstPointIterator>();

    auto itr          = outMesh->begin<sight::data::iterator::PointIterator>();
    const auto itrEnd = outMesh->end<sight::data::iterator::PointIterator>();

    const glm::dmat4x4 matrix = sight::geometry::data::getMatrixFromTF3D(t);

    [[maybe_unused]] const size_t numPts = inMesh->getNumberOfPoints();
    SIGHT_ASSERT("In and out meshes should have the same number of points", numPts == outMesh->getNumberOfPoints());

    SIGHT_ASSERT(
        "in and out meshes must have the same point normals attribute",
        (inMesh->hasPointNormals() && outMesh->hasPointNormals())
        || (!inMesh->hasPointNormals() && !outMesh->hasPointNormals())
    );

    for( ; itr != itrEnd ; ++itr, ++inItr)
    {
        const ::glm::vec4 pt(inItr->point->x, inItr->point->y, inItr->point->z, 1.);
        const ::glm::vec4 transformedPt = matrix * pt;

        itr->point->x = transformedPt.x;
        itr->point->y = transformedPt.y;
        itr->point->z = transformedPt.z;

        if(inMesh->hasPointNormals())
        {
            const ::glm::vec4 normal(inItr->normal->nx, inItr->normal->ny, inItr->normal->nz, 0.);
            const ::glm::vec4 transformedNormal = ::glm::normalize(matrix * normal);

            itr->normal->nx = transformedNormal.x;
            itr->normal->ny = transformedNormal.y;
            itr->normal->nz = transformedNormal.z;
        }
    }

    if(inMesh->hasCellNormals())
    {
        SIGHT_ASSERT("out mesh must have normals", outMesh->hasCellNormals());

        auto inCellItr = inMesh->begin<sight::data::iterator::ConstCellIterator>();

        auto itrCell          = outMesh->begin<sight::data::iterator::CellIterator>();
        const auto itrCellEnd = outMesh->end<sight::data::iterator::CellIterator>();

        for( ; itrCell != itrCellEnd ; ++itrCell, ++inCellItr)
        {
            const ::glm::vec4 normal(inCellItr->normal->nx, inCellItr->normal->ny, inCellItr->normal->nz, 0.);
            const ::glm::vec4 transformedNormal = ::glm::normalize(matrix * normal);

            itrCell->normal->nx = transformedNormal.x;
            itrCell->normal->ny = transformedNormal.y;
            itrCell->normal->nz = transformedNormal.z;
        }
    }
}

//------------------------------------------------------------------------------

void Mesh::transform(sight::data::Mesh::sptr mesh, const sight::data::Matrix4& t)
{
    Mesh::transform(mesh, mesh, t);
}

//------------------------------------------------------------------------------

void Mesh::colorizeMeshPoints(
    const sight::data::Mesh::sptr& mesh,
    const std::uint8_t colorR,
    const std::uint8_t colorG,
    const std::uint8_t colorB,
    const std::uint8_t colorA
)
{
    const auto dumpLock = mesh->lock();

    SIGHT_ASSERT("color array must be allocated", mesh->hasPointColors());

    auto itr          = mesh->begin<sight::data::iterator::PointIterator>();
    const auto itrEnd = mesh->end<sight::data::iterator::PointIterator>();

    for( ; itr != itrEnd ; ++itr)
    {
        itr->rgba->r = colorR;
        itr->rgba->g = colorG;
        itr->rgba->b = colorB;
        itr->rgba->a = colorA;
    }

    auto sig = mesh->signal<sight::data::Mesh::PointColorsModifiedSignalType>(
        sight::data::Mesh::s_POINT_COLORS_MODIFIED_SIG
    );
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void Mesh::colorizeMeshPoints(
    const sight::data::Mesh::sptr& _mesh,
    const std::vector<size_t>& _vectorNumTriangle,
    const std::uint8_t _colorR,
    const std::uint8_t _colorG,
    const std::uint8_t _colorB,
    const std::uint8_t _colorA
)
{
    const auto dumpLock = _mesh->lock();

    auto itrCell          = _mesh->begin<sight::data::iterator::ConstCellIterator>();
    const auto itrCellEnd = _mesh->end<sight::data::iterator::ConstCellIterator>();

    auto itrPoint = _mesh->begin<sight::data::iterator::PointIterator>();

    for(size_t index : _vectorNumTriangle)
    {
        auto cell                        = itrCell + static_cast<std::ptrdiff_t>(index);
        const std::ptrdiff_t indexPoint0 = static_cast<std::ptrdiff_t>(cell->pointIdx[0]);
        const std::ptrdiff_t indexPoint1 = static_cast<std::ptrdiff_t>(cell->pointIdx[1]);
        const std::ptrdiff_t indexPoint2 = static_cast<std::ptrdiff_t>(cell->pointIdx[2]);

        auto point1 = itrPoint + indexPoint0;
        auto point2 = itrPoint + indexPoint1;
        auto point3 = itrPoint + indexPoint2;

        point1->rgba->r = _colorR;
        point1->rgba->g = _colorG;
        point1->rgba->b = _colorB;
        point1->rgba->a = _colorA;

        point2->rgba->r = _colorR;
        point2->rgba->g = _colorG;
        point2->rgba->b = _colorB;
        point2->rgba->a = _colorA;

        point3->rgba->r = _colorR;
        point3->rgba->g = _colorG;
        point3->rgba->b = _colorB;
        point3->rgba->a = _colorA;
    }

    sight::data::Mesh::PointColorsModifiedSignalType::sptr sig;
    sig = _mesh->signal<sight::data::Mesh::PointColorsModifiedSignalType>(
        sight::data::Mesh::s_POINT_COLORS_MODIFIED_SIG
    );
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void Mesh::colorizeMeshCells(
    const sight::data::Mesh::sptr& mesh,
    const std::uint8_t colorR,
    const std::uint8_t colorG,
    const std::uint8_t colorB,
    const std::uint8_t colorA
)
{
    const auto dumpLock = mesh->lock();

    SIGHT_ASSERT("color array must be allocated", mesh->hasCellColors());

    auto itr          = mesh->begin<sight::data::iterator::CellIterator>();
    const auto itrEnd = mesh->end<sight::data::iterator::CellIterator>();

    for( ; itr != itrEnd ; ++itr)
    {
        itr->rgba->r = colorR;
        itr->rgba->g = colorG;
        itr->rgba->b = colorB;
        itr->rgba->a = colorA;
    }

    auto sig = mesh->signal<sight::data::Mesh::CellColorsModifiedSignalType>(
        sight::data::Mesh::s_CELL_COLORS_MODIFIED_SIG
    );
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

void Mesh::colorizeMeshCells(
    const sight::data::Mesh::sptr& mesh,
    const std::vector<size_t>& triangleIndexVector,
    const std::uint8_t colorR,
    const std::uint8_t colorG,
    const std::uint8_t colorB,
    const std::uint8_t colorA
)
{
    const auto dumpLock = mesh->lock();

    auto itrCell          = mesh->begin<sight::data::iterator::CellIterator>();
    const auto itrCellEnd = mesh->end<sight::data::iterator::CellIterator>();

    for(size_t index : triangleIndexVector)
    {
        auto cell = itrCell + static_cast<std::ptrdiff_t>(index);

        cell->rgba->r = colorR;
        cell->rgba->g = colorG;
        cell->rgba->b = colorB;
        cell->rgba->a = colorA;
    }

    auto sig = mesh->signal<sight::data::Mesh::CellColorsModifiedSignalType>(
        sight::data::Mesh::s_CELL_COLORS_MODIFIED_SIG
    );
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

bool Mesh::isClosed(const sight::data::Mesh::csptr& mesh)
{
    bool isClosed = true;

    typedef std::pair<sight::data::Mesh::CellId, sight::data::Mesh::CellId> Edge;
    typedef std::map<Edge, int> EdgeHistogram;
    EdgeHistogram edgesHistogram;

    const auto dumpLock = mesh->lock();

    auto itr          = mesh->begin<sight::data::iterator::ConstCellIterator>();
    const auto itrEnd = mesh->end<sight::data::iterator::ConstCellIterator>();

    size_t count = 0;
    for( ; itr != itrEnd ; ++itr)
    {
        ++count;
        const auto nbPoints = itr->nbPoints;
        for(size_t i = 0 ; i < nbPoints - 1 ; ++i)
        {
            const auto edge1 = geometry::data::makeOrderedPair(itr->pointIdx[i], itr->pointIdx[i + 1]);

            if(edgesHistogram.find(edge1) == edgesHistogram.end())
            {
                edgesHistogram[edge1] = 1;
            }
            else
            {
                ++edgesHistogram[edge1];
            }
        }

        if(nbPoints > 2)
        {
            const auto edge2 = geometry::data::makeOrderedPair(itr->pointIdx[0], itr->pointIdx[nbPoints - 1]);

            if(edgesHistogram.find(edge2) == edgesHistogram.end())
            {
                edgesHistogram[edge2] = 1;
            }
            else
            {
                ++edgesHistogram[edge2];
            }
        }
    }

    for(const EdgeHistogram::value_type& histo : edgesHistogram)
    {
        if(histo.second != 2)
        {
            isClosed = false;
            break;
        }
    }

    return isClosed;
}

//------------------------------------------------------------------------------

} // namespace sight::geometry::data
