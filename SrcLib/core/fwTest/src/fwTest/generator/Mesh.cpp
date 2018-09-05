/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwTest/generator/Mesh.hpp"

#include <fwDataTools/Mesh.hpp>

#include <fwTools/NumericRoundCast.hxx>

#include <cstdlib>
#include <ctime>

namespace fwTest
{
namespace generator
{

struct RandFloat {
    //------------------------------------------------------------------------------

    float operator()()
    {
        return ((rand()%101-50.f))/500.f;
    }
};

//------------------------------------------------------------------------------

void Mesh::initRand()
{
    std::srand(::fwTools::numericRoundCast<unsigned int>(std::time(NULL)));
}

//------------------------------------------------------------------------------

void Mesh::generateMesh( ::fwData::Mesh::sptr mesh )
{
    ::fwTest::generator::Mesh::generateTriangleMesh(mesh);
    ::fwTest::generator::Mesh::initRand();
    ::fwDataTools::Mesh::shakePoint(mesh);
    ::fwDataTools::Mesh::generateCellNormals(mesh);
    ::fwDataTools::Mesh::generatePointNormals(mesh);
    ::fwDataTools::Mesh::colorizeMeshPoints(mesh);
    ::fwDataTools::Mesh::colorizeMeshCells(mesh);
    mesh->adjustAllocatedMemory();
}

//------------------------------------------------------------------------------

void Mesh::generateTriangleQuadMesh(::fwData::Mesh::sptr mesh)
{
    size_t nbPointsByEdge = 10;
    float edgeDim         = 100.;
    Mesh::PointsMapType points;

    mesh->clear();
    Mesh::addTriangleMesh(mesh, points, nbPointsByEdge, edgeDim);
    Mesh::addQuadMesh(mesh, points, nbPointsByEdge, edgeDim);
}

//------------------------------------------------------------------------------

void Mesh::generateTriangleMesh(::fwData::Mesh::sptr mesh)
{
    size_t nbPointsByEdge = 10;
    float edgeDim         = 100.;
    Mesh::PointsMapType points;

    mesh->clear();
    Mesh::addTriangleMesh(mesh, points, nbPointsByEdge, edgeDim);
}

//------------------------------------------------------------------------------

void Mesh::generateQuadMesh(::fwData::Mesh::sptr mesh)
{
    size_t nbPointsByEdge = 10;
    float edgeDim         = 100.;
    Mesh::PointsMapType points;

    mesh->clear();
    Mesh::addQuadMesh(mesh, points, nbPointsByEdge, edgeDim);
}
//------------------------------------------------------------------------------

void Mesh::addQuadMesh(::fwData::Mesh::sptr mesh, PointsMapType& points, size_t nbPointsByEdge, float edgeDim)
{
    ::fwDataTools::helper::Mesh::sptr meshHelper;
    meshHelper = ::fwDataTools::helper::Mesh::New(mesh);

    ::fwData::Mesh::PointValueType pt1[3], pt2[3], pt3[3], pt4[3];
    ::fwData::Mesh::Id idx1, idx2, idx3, idx4;
    float step = edgeDim / nbPointsByEdge;

    //Face Y = edgeDim
    for(size_t x = 0; x < nbPointsByEdge; x++)
    {
        for(size_t z = 0; z < nbPointsByEdge; z++)
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

            idx1 = Mesh::addPoint(pt1, meshHelper, points);
            idx2 = Mesh::addPoint(pt2, meshHelper, points);
            idx3 = Mesh::addPoint(pt3, meshHelper, points);
            idx4 = Mesh::addPoint(pt4, meshHelper, points);

            meshHelper->insertNextCell(idx1, idx3, idx4, idx2);
        }
    }

    //Face X = edgeDim
    for(size_t y = 0; y < nbPointsByEdge; y++)
    {
        for(size_t z = 0; z < nbPointsByEdge; z++)
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

            idx1 = Mesh::addPoint(pt1, meshHelper, points);
            idx2 = Mesh::addPoint(pt2, meshHelper, points);
            idx3 = Mesh::addPoint(pt3, meshHelper, points);
            idx4 = Mesh::addPoint(pt4, meshHelper, points);

            meshHelper->insertNextCell(idx1, idx3, idx4, idx2);
        }
    }
}

//------------------------------------------------------------------------------

void Mesh::addTriangleMesh(::fwData::Mesh::sptr mesh, PointsMapType& points, size_t nbPointsByEdge, float edgeDim)
{
    ::fwDataTools::helper::Mesh::sptr meshHelper;
    meshHelper = ::fwDataTools::helper::Mesh::New(mesh);

    ::fwData::Mesh::PointValueType pt1[3], pt2[3], pt3[3], pt4[3];
    ::fwData::Mesh::Id idx1, idx2, idx3, idx4;
    float step = edgeDim / nbPointsByEdge;

    //Face Z = 0
    for(size_t x = 0; x < nbPointsByEdge; x++)
    {
        for(size_t y = 0; y < nbPointsByEdge; y++)
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

            idx1 = Mesh::addPoint(pt1, meshHelper, points);
            idx2 = Mesh::addPoint(pt2, meshHelper, points);
            idx3 = Mesh::addPoint(pt3, meshHelper, points);
            idx4 = Mesh::addPoint(pt4, meshHelper, points);

            meshHelper->insertNextCell(idx1, idx4, idx2);
            meshHelper->insertNextCell(idx1, idx3, idx4);
        }
    }

    //Face X = 0
    for(size_t y = 0; y < nbPointsByEdge; y++)
    {
        for(size_t z = 0; z < nbPointsByEdge; z++)
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

            idx1 = Mesh::addPoint(pt1, meshHelper, points);
            idx2 = Mesh::addPoint(pt2, meshHelper, points);
            idx3 = Mesh::addPoint(pt3, meshHelper, points);
            idx4 = Mesh::addPoint(pt4, meshHelper, points);

            meshHelper->insertNextCell(idx2, idx4, idx3);
            meshHelper->insertNextCell(idx1, idx2, idx3);
        }
    }
}

//------------------------------------------------------------------------------

::fwData::Mesh::Id Mesh::addPoint(::fwData::Mesh::PointValueType* pt,
                                  ::fwDataTools::helper::Mesh::sptr meshHelper,
                                  PointsMapType& points)
{
    ::fwDataTools::Point myPoint(pt[0], pt[1], pt[2]);

    PointsMapType::iterator it = points.find(myPoint);
    if(it != points.end())
    {
        return it->second;
    }
    ::fwData::Mesh::Id idx = meshHelper->insertNextPoint(pt[0], pt[1], pt[2]);
    points[myPoint]        = idx;
    return idx;
}

//------------------------------------------------------------------------------

} // namespace generator
} // namespace fwTest
