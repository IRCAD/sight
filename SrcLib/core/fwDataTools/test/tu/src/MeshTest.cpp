/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "MeshTest.hpp"

#include <fwDataTools/helper/ArrayGetter.hpp>
#include <fwDataTools/Mesh.hpp>
#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <fwTest/generator/Mesh.hpp>

#include <fwTools/Type.hpp>

#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <sstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataTools::ut::MeshTest );

namespace fwDataTools
{
namespace ut
{

//------------------------------------------------------------------------------

void MeshTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void MeshTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void MeshTest::colorizePointsTest()
{
    // Check to colorize all the mesh with RGB
    {
        const std::uint8_t R = 24;
        const std::uint8_t G = 55;
        const std::uint8_t B = 3;

        ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
        ::fwTest::generator::Mesh::generateTriangleMesh(mesh);

        mesh->allocatePointColors(::fwData::Mesh::RGB);

        ::fwDataTools::Mesh::colorizeMeshPoints(mesh, R, G, B);

        ::fwData::Array::csptr pointcolorArray = mesh->getPointColorsArray();
        ::fwDataTools::helper::ArrayGetter helper(pointcolorArray);

        const ::fwData::Mesh::ColorValueType* colorBuff    = helper.begin< ::fwData::Mesh::ColorValueType >();
        const ::fwData::Mesh::ColorValueType* colorBuffEnd = helper.end< ::fwData::Mesh::ColorValueType >();
        for (; colorBuff != colorBuffEnd; colorBuff += 3 )
        {
            CPPUNIT_ASSERT_EQUAL(R, colorBuff[0]);
            CPPUNIT_ASSERT_EQUAL(G, colorBuff[1]);
            CPPUNIT_ASSERT_EQUAL(B, colorBuff[2]);
        }
    }

    // Check to colorize all the mesh with RGBA
    {
        const std::uint8_t R = 214;
        const std::uint8_t G = 25;
        const std::uint8_t B = 33;
        const std::uint8_t A = 63;

        ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
        ::fwTest::generator::Mesh::generateTriangleMesh(mesh);

        mesh->allocatePointColors(::fwData::Mesh::RGBA);

        ::fwDataTools::Mesh::colorizeMeshPoints(mesh, R, G, B, A);

        ::fwData::Array::csptr pointcolorArray = mesh->getPointColorsArray();
        ::fwDataTools::helper::ArrayGetter helper(pointcolorArray);

        const ::fwData::Mesh::ColorValueType* colorBuff    = helper.begin< ::fwData::Mesh::ColorValueType >();
        const ::fwData::Mesh::ColorValueType* colorBuffEnd = helper.end< ::fwData::Mesh::ColorValueType >();
        for (; colorBuff != colorBuffEnd; colorBuff += 4 )
        {
            CPPUNIT_ASSERT_EQUAL(R, colorBuff[0]);
            CPPUNIT_ASSERT_EQUAL(G, colorBuff[1]);
            CPPUNIT_ASSERT_EQUAL(B, colorBuff[2]);
            CPPUNIT_ASSERT_EQUAL(A, colorBuff[3]);
        }
    }

    // Check to colorize few points with RGB
    {
        const std::uint8_t R = 24;
        const std::uint8_t G = 55;
        const std::uint8_t B = 3;

        ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
        ::fwTest::generator::Mesh::generateTriangleMesh(mesh);

        mesh->allocatePointColors(::fwData::Mesh::RGB);

        std::vector< size_t >vectorNumTriangle = {{ 2, 3, 18, 23, 6 }};

        // fill color with 0
        ::fwDataTools::Mesh::colorizeMeshPoints(mesh, 0, 0, 0);

        ::fwDataTools::Mesh::colorizeMeshPoints(mesh, vectorNumTriangle, R, G, B);

        ::fwData::Array::csptr cellsArray = mesh->getCellDataArray();
        ::fwDataTools::helper::ArrayGetter cellHelper(cellsArray);
        const ::fwData::Mesh::CellValueType* cells = cellHelper.begin< ::fwData::Mesh::CellValueType >();

        // get the 3 points of each triangles
        std::set< size_t > vertexIndices;
        for (size_t i = 0; i < vectorNumTriangle.size(); ++i)
        {
            vertexIndices.insert(cells[vectorNumTriangle[i] * 3 + 0]);
            vertexIndices.insert(cells[vectorNumTriangle[i] * 3 + 1]);
            vertexIndices.insert(cells[vectorNumTriangle[i] * 3 + 2]);
        }

        ::fwData::Array::csptr pointcolorArray = mesh->getPointColorsArray();
        ::fwDataTools::helper::ArrayGetter helper(pointcolorArray);
        const ::fwData::Mesh::ColorValueType* colorBuff    = helper.begin< ::fwData::Mesh::ColorValueType >();
        const ::fwData::Mesh::ColorValueType* colorBuffEnd = helper.end< ::fwData::Mesh::ColorValueType >();

        size_t count = 0;
        for (; colorBuff != colorBuffEnd; colorBuff += 3 )
        {
            auto iter = std::find(vertexIndices.begin(), vertexIndices.end(), count);
            if (iter != vertexIndices.end())
            {
                CPPUNIT_ASSERT_EQUAL(R, colorBuff[0]);
                CPPUNIT_ASSERT_EQUAL(G, colorBuff[1]);
                CPPUNIT_ASSERT_EQUAL(B, colorBuff[2]);
            }
            else
            {
                CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), colorBuff[0]);
                CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), colorBuff[1]);
                CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), colorBuff[2]);
            }
            ++count;
        }
    }

    // Check to colorize few points with RGBA
    {
        const std::uint8_t R = 214;
        const std::uint8_t G = 155;
        const std::uint8_t B = 34;
        const std::uint8_t A = 124;

        ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
        ::fwTest::generator::Mesh::generateTriangleMesh(mesh);

        mesh->allocatePointColors(::fwData::Mesh::RGBA);

        std::vector< size_t >vectorNumTriangle = {{ 0, 12, 1, 3, 21 }};

        // fill color with 0
        ::fwDataTools::Mesh::colorizeMeshPoints(mesh, 0, 0, 0, 0);

        ::fwDataTools::Mesh::colorizeMeshPoints(mesh, vectorNumTriangle, R, G, B, A);

        ::fwData::Array::csptr cellsArray = mesh->getCellDataArray();
        ::fwDataTools::helper::ArrayGetter cellHelper(cellsArray);
        const ::fwData::Mesh::CellValueType* cells = cellHelper.begin< ::fwData::Mesh::CellValueType >();

        // get the 3 points of each triangles
        std::set< size_t > vertexIndices;
        for (size_t i = 0; i < vectorNumTriangle.size(); ++i)
        {
            vertexIndices.insert(cells[vectorNumTriangle[i] * 3 + 0]);
            vertexIndices.insert(cells[vectorNumTriangle[i] * 3 + 1]);
            vertexIndices.insert(cells[vectorNumTriangle[i] * 3 + 2]);
        }

        ::fwData::Array::csptr pointcolorArray = mesh->getPointColorsArray();
        ::fwDataTools::helper::ArrayGetter helper(pointcolorArray);
        const ::fwData::Mesh::ColorValueType* colorBuff    = helper.begin< ::fwData::Mesh::ColorValueType >();
        const ::fwData::Mesh::ColorValueType* colorBuffEnd = helper.end< ::fwData::Mesh::ColorValueType >();

        size_t count = 0;
        for (; colorBuff != colorBuffEnd; colorBuff += 4 )
        {
            auto iter = std::find(vertexIndices.begin(), vertexIndices.end(), count);
            if (iter != vertexIndices.end())
            {
                CPPUNIT_ASSERT_EQUAL(R, colorBuff[0]);
                CPPUNIT_ASSERT_EQUAL(G, colorBuff[1]);
                CPPUNIT_ASSERT_EQUAL(B, colorBuff[2]);
                CPPUNIT_ASSERT_EQUAL(A, colorBuff[3]);
            }
            else
            {
                CPPUNIT_ASSERT_EQUAL( std::uint8_t(0), colorBuff[0]);
                CPPUNIT_ASSERT_EQUAL( std::uint8_t(0), colorBuff[1]);
                CPPUNIT_ASSERT_EQUAL( std::uint8_t(0), colorBuff[2]);
                CPPUNIT_ASSERT_EQUAL( std::uint8_t(0), colorBuff[3]);
            }
            ++count;
        }
    }
}

//------------------------------------------------------------------------------

void MeshTest::colorizeCellsTest()
{
    // Check to colorize all the mesh with RGB
    {
        const std::uint8_t R = 24;
        const std::uint8_t G = 55;
        const std::uint8_t B = 3;

        ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
        ::fwTest::generator::Mesh::generateTriangleMesh(mesh);

        mesh->allocateCellColors(::fwData::Mesh::RGB);

        ::fwDataTools::Mesh::colorizeMeshCells(mesh, R, G, B);

        ::fwData::Array::csptr cellcolorArray = mesh->getCellColorsArray();
        ::fwDataTools::helper::ArrayGetter helper(cellcolorArray);

        const ::fwData::Mesh::ColorValueType* colorBuff    = helper.begin< ::fwData::Mesh::ColorValueType >();
        const ::fwData::Mesh::ColorValueType* colorBuffEnd = helper.end< ::fwData::Mesh::ColorValueType >();
        for (; colorBuff != colorBuffEnd; colorBuff += 3 )
        {
            CPPUNIT_ASSERT_EQUAL(R, colorBuff[0]);
            CPPUNIT_ASSERT_EQUAL(G, colorBuff[1]);
            CPPUNIT_ASSERT_EQUAL(B, colorBuff[2]);
        }
    }

    // Check to colorize all the mesh with RGBA
    {
        const std::uint8_t R = 214;
        const std::uint8_t G = 25;
        const std::uint8_t B = 33;
        const std::uint8_t A = 63;

        ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
        ::fwTest::generator::Mesh::generateTriangleMesh(mesh);

        mesh->allocateCellColors(::fwData::Mesh::RGBA);

        ::fwDataTools::Mesh::colorizeMeshCells(mesh, R, G, B, A);

        ::fwData::Array::csptr cellcolorArray = mesh->getCellColorsArray();
        ::fwDataTools::helper::ArrayGetter helper(cellcolorArray);

        const ::fwData::Mesh::ColorValueType* colorBuff    = helper.begin< ::fwData::Mesh::ColorValueType >();
        const ::fwData::Mesh::ColorValueType* colorBuffEnd = helper.end< ::fwData::Mesh::ColorValueType >();
        for (; colorBuff != colorBuffEnd; colorBuff += 4 )
        {
            CPPUNIT_ASSERT_EQUAL(R, colorBuff[0]);
            CPPUNIT_ASSERT_EQUAL(G, colorBuff[1]);
            CPPUNIT_ASSERT_EQUAL(B, colorBuff[2]);
            CPPUNIT_ASSERT_EQUAL(A, colorBuff[3]);
        }
    }

    // Check to colorize few points with RGB
    {
        const std::uint8_t R = 24;
        const std::uint8_t G = 55;
        const std::uint8_t B = 3;

        ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
        ::fwTest::generator::Mesh::generateTriangleMesh(mesh);

        mesh->allocateCellColors(::fwData::Mesh::RGB);

        std::vector< size_t >vectorNumTriangle = {{ 2, 3, 18, 23, 6 }};

        // fill color with 0
        ::fwDataTools::Mesh::colorizeMeshCells(mesh, 0, 0, 0);

        ::fwDataTools::Mesh::colorizeMeshCells(mesh, vectorNumTriangle, R, G, B);

        ::fwData::Array::csptr cellcolorArray = mesh->getCellColorsArray();
        ::fwDataTools::helper::ArrayGetter helper(cellcolorArray);
        const ::fwData::Mesh::ColorValueType* colorBuff    = helper.begin< ::fwData::Mesh::ColorValueType >();
        const ::fwData::Mesh::ColorValueType* colorBuffEnd = helper.end< ::fwData::Mesh::ColorValueType >();

        size_t count = 0;
        for (; colorBuff != colorBuffEnd; colorBuff += 3 )
        {
            auto iter = std::find(vectorNumTriangle.begin(), vectorNumTriangle.end(), count);
            if (iter != vectorNumTriangle.end())
            {
                CPPUNIT_ASSERT_EQUAL(R, colorBuff[0]);
                CPPUNIT_ASSERT_EQUAL(G, colorBuff[1]);
                CPPUNIT_ASSERT_EQUAL(B, colorBuff[2]);
            }
            else
            {
                CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), colorBuff[0]);
                CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), colorBuff[1]);
                CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), colorBuff[2]);
            }
            ++count;
        }
    }

    // Check to colorize few points with RGB
    {
        const std::uint8_t R = 2;
        const std::uint8_t G = 125;
        const std::uint8_t B = 75;
        const std::uint8_t A = 55;

        ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
        ::fwTest::generator::Mesh::generateTriangleMesh(mesh);

        mesh->allocateCellColors(::fwData::Mesh::RGBA);

        std::vector< size_t >vectorNumTriangle = {{ 2, 3, 18, 23, 6, 5 }};

        // fill color with 0
        ::fwDataTools::Mesh::colorizeMeshCells(mesh, 0, 0, 0, 0);

        ::fwDataTools::Mesh::colorizeMeshCells(mesh, vectorNumTriangle, R, G, B, A);

        ::fwData::Array::csptr cellcolorArray = mesh->getCellColorsArray();
        ::fwDataTools::helper::ArrayGetter helper(cellcolorArray);
        const ::fwData::Mesh::ColorValueType* colorBuff    = helper.begin< ::fwData::Mesh::ColorValueType >();
        const ::fwData::Mesh::ColorValueType* colorBuffEnd = helper.end< ::fwData::Mesh::ColorValueType >();

        size_t count = 0;
        for (; colorBuff != colorBuffEnd; colorBuff += 4 )
        {
            auto iter = std::find(vectorNumTriangle.begin(), vectorNumTriangle.end(), count);
            if (iter != vectorNumTriangle.end())
            {
                CPPUNIT_ASSERT_EQUAL(R, colorBuff[0]);
                CPPUNIT_ASSERT_EQUAL(G, colorBuff[1]);
                CPPUNIT_ASSERT_EQUAL(B, colorBuff[2]);
                CPPUNIT_ASSERT_EQUAL(A, colorBuff[3]);
            }
            else
            {
                CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), colorBuff[0]);
                CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), colorBuff[1]);
                CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), colorBuff[2]);
                CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), colorBuff[3]);
            }
            ++count;
        }
    }
}

//------------------------------------------------------------------------------

void MeshTest::transformTest()
{
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    ::fwTest::generator::Mesh::generateTriangleMesh(mesh);
    ::fwDataTools::Mesh::generatePointNormals(mesh);
    ::fwDataTools::Mesh::generateCellNormals(mesh);

    ::fwData::Mesh::sptr meshOrig = ::fwData::Mesh::copy(mesh);

    ::glm::dmat4x4 matrix(1.);
    matrix = ::glm::rotate(matrix, ::glm::radians(90.), ::glm::dvec3(0., 0., 1.));
    matrix = ::glm::translate(matrix, ::glm::dvec3(10., 20., 30.));
    matrix = ::glm::scale(matrix, ::glm::dvec3(2., 2., 2.));

    ::fwData::TransformationMatrix3D::sptr trans = ::fwData::TransformationMatrix3D::New();
    ::fwDataTools::TransformationMatrix3D::setTF3DFromMatrix(trans, matrix);

    ::fwDataTools::Mesh::transform(meshOrig, mesh, trans);

    const size_t numPts = mesh->getNumberOfPoints();

    // Test points transform
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(mesh->getPointsArray());
        ::fwDataTools::helper::ArrayGetter arrayOrigHelper(meshOrig->getPointsArray());
        const ::fwData::Mesh::PointValueType* values     = arrayHelper.begin< ::fwData::Mesh::PointValueType >();
        const ::fwData::Mesh::PointValueType* valuesOrig = arrayOrigHelper.begin< ::fwData::Mesh::PointValueType >();

        for(size_t i = 0; i < numPts; ++i )
        {
            const ::glm::vec4 pt(valuesOrig[i*3], valuesOrig[i*3 + 1], valuesOrig[i*3 + 2], 1.);
            const ::glm::vec4 transformedPt = matrix * pt;

            CPPUNIT_ASSERT_EQUAL(transformedPt.x, values[i*3]);
            CPPUNIT_ASSERT_EQUAL(transformedPt.y, values[i*3 + 1]);
            CPPUNIT_ASSERT_EQUAL(transformedPt.z, values[i*3 + 2]);
        }
    }

    // Test points normals transform
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(mesh->getPointNormalsArray());
        ::fwDataTools::helper::ArrayGetter arrayOrigHelper(meshOrig->getPointNormalsArray());
        const ::fwData::Mesh::PointValueType* values     = arrayHelper.begin< ::fwData::Mesh::PointValueType >();
        const ::fwData::Mesh::PointValueType* valuesOrig = arrayOrigHelper.begin< ::fwData::Mesh::PointValueType >();

        for(size_t i = 0; i < numPts; ++i )
        {
            const ::glm::vec4 pt(valuesOrig[i*3], valuesOrig[i*3 + 1], valuesOrig[i*3 + 2], 0.);
            const ::glm::vec4 transformedPt = ::glm::normalize(matrix * pt);

            CPPUNIT_ASSERT_EQUAL(transformedPt.x, values[i*3]);
            CPPUNIT_ASSERT_EQUAL(transformedPt.y, values[i*3 + 1]);
            CPPUNIT_ASSERT_EQUAL(transformedPt.z, values[i*3 + 2]);
        }
    }

    // Test cells normals transform
    {
        ::fwDataTools::helper::ArrayGetter arrayHelper(mesh->getCellNormalsArray());
        ::fwDataTools::helper::ArrayGetter arrayOrigHelper(meshOrig->getCellNormalsArray());
        const ::fwData::Mesh::PointValueType* values     = arrayHelper.begin< ::fwData::Mesh::PointValueType >();
        const ::fwData::Mesh::PointValueType* valuesOrig = arrayOrigHelper.begin< ::fwData::Mesh::PointValueType >();

        for(size_t i = 0; i < numPts; ++i )
        {
            const ::glm::vec4 pt(valuesOrig[i*3], valuesOrig[i*3 + 1], valuesOrig[i*3 + 2], 0.);
            const ::glm::vec4 transformedPt = ::glm::normalize(matrix * pt);

            CPPUNIT_ASSERT_EQUAL(transformedPt.x, values[i*3]);
            CPPUNIT_ASSERT_EQUAL(transformedPt.y, values[i*3 + 1]);
            CPPUNIT_ASSERT_EQUAL(transformedPt.z, values[i*3 + 2]);
        }
    }

}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwDataTools
