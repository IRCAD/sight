/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
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

#include "fwRenderQt3D/core/GenericScene.hpp"
#include "fwRenderQt3D/data/Mesh.hpp"

#include <fwData/Mesh.hpp>

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometryRenderer>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(::fwRenderQt3DTest::ut::MeshTest);

namespace fwRenderQt3DTest
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
    // Arranges a cube ::fwData::Mesh and a generic scene.
    auto scene = new ::fwRenderQt3D::core::GenericScene(false);
    scene->setCamera(scene->getCamera());

    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    mesh->reserve(8, 12, ::fwData::Mesh::CellType::TRIANGLE, ::fwData::Mesh::Attributes::POINT_NORMALS);

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
    auto qt3dMesh = new ::fwRenderQt3D::data::Mesh(scene);
    qt3dMesh->setMesh(mesh);

    // Asserts.
    auto geomRenderer    = qobject_cast< Qt3DRender::QGeometryRenderer* >(qt3dMesh->components()[0]);
    auto posAttribute    = geomRenderer->geometry()->attributes()[0];
    auto normalAttribute = geomRenderer->geometry()->attributes()[1];
    auto indexAttribute  = geomRenderer->geometry()->attributes()[2];

    // Asserts primitive type.
    CPPUNIT_ASSERT_EQUAL(Qt3DRender::QGeometryRenderer::Triangles, geomRenderer->primitiveType());

    // Asserts number of vertices, normals, and indexes.
    CPPUNIT_ASSERT_EQUAL(static_cast< unsigned int >(mesh->getNumberOfPoints()), posAttribute->count());
    CPPUNIT_ASSERT_EQUAL(static_cast< unsigned int >(mesh->getNumberOfPoints()), normalAttribute->count());
    CPPUNIT_ASSERT_EQUAL(static_cast< unsigned int >(mesh->getNumberOfCells() * 3), indexAttribute->count());

    // Asserts each point is at the right position.
    auto itrPt          = mesh->begin< ::fwData::iterator::ConstPointIterator >();
    const auto endItrPt = mesh->end< ::fwData::iterator::ConstPointIterator >();

    float* posBufferData = reinterpret_cast<float*>( posAttribute->buffer()->data().data() );
    unsigned int count   = 0;
    for(; itrPt != endItrPt; ++itrPt)
    {
        CPPUNIT_ASSERT(static_cast< float >(itrPt->point->x) - posBufferData[count] < 0.01f);
        CPPUNIT_ASSERT(static_cast< float >(itrPt->point->y) - posBufferData[count+1] < 0.01f);
        CPPUNIT_ASSERT(static_cast< float >(itrPt->point->z) - posBufferData[count+2] < 0.01f);
        count += 3;
    }

    // Asserts indexes are in the right order.
    auto itrCell      = mesh->begin< ::fwData::iterator::ConstCellIterator >();
    const auto endItr = mesh->end< ::fwData::iterator::ConstCellIterator >();

    unsigned int* indexBufferData = reinterpret_cast<unsigned int*>( indexAttribute->buffer()->data().data() );
    count = 0;
    for(; itrCell != endItr; ++itrCell)
    {
        for(unsigned int i = 0; i < itrCell->nbPoints; ++i)
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
    // Arranges a cube ::fwData::Mesh and a generic scene.
    const auto scene = new ::fwRenderQt3D::core::GenericScene(false);
    const auto camera = scene->getCamera();

    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    mesh->reserve(8, 12, ::fwData::Mesh::CellType::TRIANGLE, ::fwData::Mesh::Attributes::POINT_NORMALS);

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
    expectedCamera->lens()->setPerspectiveProjection(camera->lens()->fieldOfView(),
                                                     camera->lens()->aspectRatio(),
                                                     camera->lens()->nearPlane(), camera->lens()->farPlane());
    expectedCamera->setUpVector(camera->upVector());
    expectedCamera->setPosition(camera->position());
    expectedCamera->setViewCenter(camera->viewCenter());
    expectedCamera->viewSphere(QVector3D(0.5f, 0.5f, 0.5f), 1);

    // Sets Qt3D mesh and center camera on it.
    auto qt3dMesh = new ::fwRenderQt3D::data::Mesh(scene);
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

    delete camera;
    delete expectedCamera;
    delete qt3dMesh;
    delete scene;
}

//------------------------------------------------------------------------------

} // namespace ut.

} // namespace fwRenderQt3DTest.
