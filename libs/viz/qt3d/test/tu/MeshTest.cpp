/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "MeshTest.hpp"

#include "TestApplication.hpp"

#include "viz/qt3d/core/GenericScene.hpp"
#include "viz/qt3d/data/Mesh.hpp"

#include <data/Mesh.hpp>

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometryRenderer>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::qt3dTest::ut::MeshTest);

namespace sight::viz::qt3dTest::ut
{

//------------------------------------------------------------------------------

MeshTest::MeshTest()
= default;

//------------------------------------------------------------------------------

MeshTest::~MeshTest()
= default;

//------------------------------------------------------------------------------

void MeshTest::setUp()
{
}

//------------------------------------------------------------------------------

void MeshTest::tearDown()
{
}

//------------------------------------------------------------------------------

void MeshTest::setCubeMesh()
{
    TestApplication app;

    // Arranges a cube data::Mesh and a generic scene.
    auto* scene = new sight::viz::qt3d::core::GenericScene(false);
    scene->setCamera(scene->getCamera());

    const data::Mesh::sptr mesh = data::Mesh::New();
    mesh->reserve(8, 12, data::Mesh::CellType::TRIANGLE, data::Mesh::Attributes::POINT_NORMALS);

    const auto lock = mesh->dump_lock();

    mesh->pushPoint(0.0, 0.0, 0.0);
    mesh->pushPoint(1.0, 0.0, 0.0);
    mesh->pushPoint(1.0, 1.0, 0.0);
    mesh->pushPoint(0.0, 1.0, 0.0);
    mesh->pushPoint(0.0, 1.0, 1.0);
    mesh->pushPoint(0.0, 0.0, 1.0);
    mesh->pushPoint(1.0, 0.0, 1.0);
    mesh->pushPoint(1.0, 1.0, 1.0);

    mesh->pushCell(0, 1, 2);
    mesh->pushCell(2, 3, 0);
    mesh->pushCell(0, 3, 4);
    mesh->pushCell(4, 5, 0);
    mesh->pushCell(4, 5, 6);
    mesh->pushCell(6, 7, 4);
    mesh->pushCell(1, 6, 7);
    mesh->pushCell(7, 2, 1);
    mesh->pushCell(2, 3, 4);
    mesh->pushCell(4, 7, 2);
    mesh->pushCell(0, 1, 6);
    mesh->pushCell(6, 5, 0);

    // Sets Qt3D mesh.
    auto* qt3dMesh = new viz::qt3d::data::Mesh(scene);
    qt3dMesh->setMesh(mesh);

    // Asserts.
    auto* geomRenderer    = qobject_cast<Qt3DRender::QGeometryRenderer*>(qt3dMesh->components()[0]);
    auto* posAttribute    = geomRenderer->geometry()->attributes()[0];
    auto* normalAttribute = geomRenderer->geometry()->attributes()[1];
    auto* indexAttribute  = geomRenderer->geometry()->attributes()[2];

    // Asserts primitive type.
    CPPUNIT_ASSERT_EQUAL(Qt3DRender::QGeometryRenderer::Triangles, geomRenderer->primitiveType());

    // Asserts number of vertices, normals, and indexes.
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(mesh->numPoints()), posAttribute->count());
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(mesh->numPoints()), normalAttribute->count());
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(mesh->numCells() * 3), indexAttribute->count());

    // Asserts each point is at the right position.
    const QByteArray posBufferDataByte = posAttribute->buffer()->data();
    const auto* const posBufferData    = reinterpret_cast<const float*>(posBufferDataByte.data());
    unsigned int count                 = 0;
    for(const auto& p : mesh->crange<data::iterator::point::xyz>())
    {
        CPPUNIT_ASSERT(static_cast<float>(p.x) - posBufferData[count] < 0.01F);
        CPPUNIT_ASSERT(static_cast<float>(p.y) - posBufferData[count + 1] < 0.01F);
        CPPUNIT_ASSERT(static_cast<float>(p.z) - posBufferData[count + 2] < 0.01F);
        count += 3;
    }

    // Asserts indexes are in the right order.
    const QByteArray indexBufferDataByte = indexAttribute->buffer()->data();
    const auto* const indexBufferData    = reinterpret_cast<const unsigned int*>(indexBufferDataByte.data());
    count = 0;
    for(const auto& cell : mesh->crange<data::iterator::cell::triangle>())
    {
        for(unsigned int i = 0 ; i < 3 ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(cell.pt[i]), indexBufferData[count]);
            count++;
        }
    }

    delete qt3dMesh;
    delete scene;
}

//------------------------------------------------------------------------------

void MeshTest::centerCameraOnCube()
{
    TestApplication app;

    // Arranges a cube data::Mesh and a generic scene.
    auto* const scene  = new sight::viz::qt3d::core::GenericScene(false);
    auto* const camera = scene->getCamera();

    const data::Mesh::sptr mesh = data::Mesh::New();
    mesh->reserve(8, 12, data::Mesh::CellType::TRIANGLE, data::Mesh::Attributes::POINT_NORMALS);

    const auto lock = mesh->dump_lock();

    mesh->pushPoint(0.0, 0.0, 0.0);
    mesh->pushPoint(1.0, 0.0, 0.0);
    mesh->pushPoint(1.0, 1.0, 0.0);
    mesh->pushPoint(0.0, 1.0, 0.0);
    mesh->pushPoint(0.0, 1.0, 1.0);
    mesh->pushPoint(0.0, 0.0, 1.0);
    mesh->pushPoint(1.0, 0.0, 1.0);
    mesh->pushPoint(1.0, 1.0, 1.0);

    mesh->pushCell(0, 1, 2);
    mesh->pushCell(2, 3, 0);
    mesh->pushCell(0, 3, 4);
    mesh->pushCell(4, 5, 0);
    mesh->pushCell(4, 5, 6);
    mesh->pushCell(6, 7, 4);
    mesh->pushCell(1, 6, 7);
    mesh->pushCell(7, 2, 1);
    mesh->pushCell(2, 3, 4);
    mesh->pushCell(4, 7, 2);
    mesh->pushCell(0, 1, 6);
    mesh->pushCell(6, 5, 0);

    // Sets expected camera after beeing centered on the cube.
    auto* expectedCamera = new Qt3DRender::QCamera();
    expectedCamera->lens()->setPerspectiveProjection(
        camera->lens()->fieldOfView(),
        camera->lens()->aspectRatio(),
        camera->lens()->nearPlane(),
        camera->lens()->farPlane()
    );
    expectedCamera->setUpVector(camera->upVector());
    expectedCamera->setPosition(camera->position());
    expectedCamera->setViewCenter(camera->viewCenter());
    expectedCamera->viewSphere(QVector3D(0.5F, 0.5F, 0.5F), 1);

    // Sets Qt3D mesh and center camera on it.
    auto* qt3dMesh = new viz::qt3d::data::Mesh(scene);
    qt3dMesh->setMesh(mesh);
    qt3dMesh->centerCameraOnMesh();

    // Asserts actual camera's view center is equal to expected camera's one.
    CPPUNIT_ASSERT(expectedCamera->viewCenter().x() - qt3dMesh->getScene()->getCamera()->viewCenter().x() < 0.01F);
    CPPUNIT_ASSERT(expectedCamera->viewCenter().y() - qt3dMesh->getScene()->getCamera()->viewCenter().y() < 0.01F);
    CPPUNIT_ASSERT(expectedCamera->viewCenter().z() - qt3dMesh->getScene()->getCamera()->viewCenter().z() < 0.01F);

    // Asserts actual camera's position is equal to expected camera's one.
    CPPUNIT_ASSERT(expectedCamera->position().x() - qt3dMesh->getScene()->getCamera()->position().x() < 0.01F);
    CPPUNIT_ASSERT(expectedCamera->position().y() - qt3dMesh->getScene()->getCamera()->position().y() < 0.01F);
    CPPUNIT_ASSERT(expectedCamera->position().z() - qt3dMesh->getScene()->getCamera()->position().z() < 0.01F);

    delete expectedCamera;
    delete qt3dMesh;
    delete scene;
}

//------------------------------------------------------------------------------

} // namespace sight::viz::qt3dTest::ut
