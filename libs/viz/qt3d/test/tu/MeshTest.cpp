/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

namespace sight::viz::qt3dTest
{

namespace ut
{

//------------------------------------------------------------------------------

MeshTest::MeshTest()
{
}

//------------------------------------------------------------------------------

MeshTest::~MeshTest()
{
}

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
    auto scene = new sight::viz::qt3d::core::GenericScene(false);
    scene->setCamera(scene->getCamera());

    const data::Mesh::sptr mesh = data::Mesh::New();
    mesh->reserve(8, 12, data::Mesh::CellType::TRIANGLE, data::Mesh::Attributes::POINT_NORMALS);

    const auto lock = mesh->lock();

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
    auto qt3dMesh = new viz::qt3d::data::Mesh(scene);
    qt3dMesh->setMesh(mesh);

    // Asserts.
    auto geomRenderer    = qobject_cast<Qt3DRender::QGeometryRenderer*>(qt3dMesh->components()[0]);
    auto posAttribute    = geomRenderer->geometry()->attributes()[0];
    auto normalAttribute = geomRenderer->geometry()->attributes()[1];
    auto indexAttribute  = geomRenderer->geometry()->attributes()[2];

    // Asserts primitive type.
    CPPUNIT_ASSERT_EQUAL(Qt3DRender::QGeometryRenderer::Triangles, geomRenderer->primitiveType());

    // Asserts number of vertices, normals, and indexes.
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(mesh->getNumberOfPoints()), posAttribute->count());
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(mesh->getNumberOfPoints()), normalAttribute->count());
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(mesh->getNumberOfCells() * 3), indexAttribute->count());

    // Asserts each point is at the right position.
    auto itrPt          = mesh->begin<data::iterator::ConstPointIterator>();
    const auto endItrPt = mesh->end<data::iterator::ConstPointIterator>();

    const QByteArray posBufferDataByte = posAttribute->buffer()->data();
    const float* const posBufferData   = reinterpret_cast<const float*>(posBufferDataByte.data());
    unsigned int count                 = 0;
    for( ; itrPt != endItrPt ; ++itrPt)
    {
        CPPUNIT_ASSERT(static_cast<float>(itrPt->point->x) - posBufferData[count] < 0.01f);
        CPPUNIT_ASSERT(static_cast<float>(itrPt->point->y) - posBufferData[count + 1] < 0.01f);
        CPPUNIT_ASSERT(static_cast<float>(itrPt->point->z) - posBufferData[count + 2] < 0.01f);
        count += 3;
    }

    // Asserts indexes are in the right order.
    auto itrCell      = mesh->begin<data::iterator::ConstCellIterator>();
    const auto endItr = mesh->end<data::iterator::ConstCellIterator>();

    const QByteArray indexBufferDataByte      = indexAttribute->buffer()->data();
    const unsigned int* const indexBufferData = reinterpret_cast<const unsigned int*>(indexBufferDataByte.data());
    count = 0;
    for( ; itrCell != endItr ; ++itrCell)
    {
        for(unsigned int i = 0 ; i < itrCell->nbPoints ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(itrCell->pointIdx[i]), indexBufferData[count]);
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
    const auto scene  = new sight::viz::qt3d::core::GenericScene(false);
    const auto camera = scene->getCamera();

    const data::Mesh::sptr mesh = data::Mesh::New();
    mesh->reserve(8, 12, data::Mesh::CellType::TRIANGLE, data::Mesh::Attributes::POINT_NORMALS);

    const auto lock = mesh->lock();

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
    auto expectedCamera = new Qt3DRender::QCamera();
    expectedCamera->lens()->setPerspectiveProjection(
        camera->lens()->fieldOfView(),
        camera->lens()->aspectRatio(),
        camera->lens()->nearPlane(),
        camera->lens()->farPlane()
    );
    expectedCamera->setUpVector(camera->upVector());
    expectedCamera->setPosition(camera->position());
    expectedCamera->setViewCenter(camera->viewCenter());
    expectedCamera->viewSphere(QVector3D(0.5f, 0.5f, 0.5f), 1);

    // Sets Qt3D mesh and center camera on it.
    auto qt3dMesh = new viz::qt3d::data::Mesh(scene);
    qt3dMesh->setMesh(mesh);
    qt3dMesh->centerCameraOnMesh();

    // Asserts actual camera's view center is equal to expected camera's one.
    CPPUNIT_ASSERT(expectedCamera->viewCenter().x() - qt3dMesh->getScene()->getCamera()->viewCenter().x() < 0.01f);
    CPPUNIT_ASSERT(expectedCamera->viewCenter().y() - qt3dMesh->getScene()->getCamera()->viewCenter().y() < 0.01f);
    CPPUNIT_ASSERT(expectedCamera->viewCenter().z() - qt3dMesh->getScene()->getCamera()->viewCenter().z() < 0.01f);

    // Asserts actual camera's position is equal to expected camera's one.
    CPPUNIT_ASSERT(expectedCamera->position().x() - qt3dMesh->getScene()->getCamera()->position().x() < 0.01f);
    CPPUNIT_ASSERT(expectedCamera->position().y() - qt3dMesh->getScene()->getCamera()->position().y() < 0.01f);
    CPPUNIT_ASSERT(expectedCamera->position().z() - qt3dMesh->getScene()->getCamera()->position().z() < 0.01f);

    delete expectedCamera;
    delete qt3dMesh;
    delete scene;
}

//------------------------------------------------------------------------------

} // namespace ut.

} // namespace sight::viz::qt3dTest.
