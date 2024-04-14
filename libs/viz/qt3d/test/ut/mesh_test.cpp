/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "mesh_test.hpp"

#include "test_application.hpp"

#include "viz/qt3d/core/generic_scene.hpp"
#include "viz/qt3d/data/mesh.hpp"

#include <data/mesh.hpp>

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometryRenderer>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::qt3d_test::ut::mesh_test);

namespace sight::viz::qt3d_test::ut
{

//------------------------------------------------------------------------------

mesh_test::mesh_test()
= default;

//------------------------------------------------------------------------------

mesh_test::~mesh_test()
= default;

//------------------------------------------------------------------------------

void mesh_test::setUp()
{
}

//------------------------------------------------------------------------------

void mesh_test::tearDown()
{
}

//------------------------------------------------------------------------------

void mesh_test::set_cube_mesh()
{
    test_application app;

    // Arranges a cube data::mesh and a generic scene.
    auto* scene = new sight::viz::qt3d::core::generic_scene(false);
    scene->set_camera(scene->get_camera());

    const data::mesh::sptr mesh = std::make_shared<data::mesh>();
    mesh->reserve(8, 12, data::mesh::cell_type_t::triangle, data::mesh::attribute::point_normals);

    const auto lock = mesh->dump_lock();

    mesh->push_point(0.0, 0.0, 0.0);
    mesh->push_point(1.0, 0.0, 0.0);
    mesh->push_point(1.0, 1.0, 0.0);
    mesh->push_point(0.0, 1.0, 0.0);
    mesh->push_point(0.0, 1.0, 1.0);
    mesh->push_point(0.0, 0.0, 1.0);
    mesh->push_point(1.0, 0.0, 1.0);
    mesh->push_point(1.0, 1.0, 1.0);

    mesh->push_cell(0, 1, 2);
    mesh->push_cell(2, 3, 0);
    mesh->push_cell(0, 3, 4);
    mesh->push_cell(4, 5, 0);
    mesh->push_cell(4, 5, 6);
    mesh->push_cell(6, 7, 4);
    mesh->push_cell(1, 6, 7);
    mesh->push_cell(7, 2, 1);
    mesh->push_cell(2, 3, 4);
    mesh->push_cell(4, 7, 2);
    mesh->push_cell(0, 1, 6);
    mesh->push_cell(6, 5, 0);

    // Sets Qt3D mesh.
    auto* qt3d_mesh = new viz::qt3d::data::mesh(scene);
    qt3d_mesh->set_mesh(mesh);

    // Asserts.
    auto* geom_renderer    = qobject_cast<Qt3DRender::QGeometryRenderer*>(qt3d_mesh->components()[0]);
    auto* pos_attribute    = geom_renderer->geometry()->attributes()[0];
    auto* normal_attribute = geom_renderer->geometry()->attributes()[1];
    auto* index_attribute  = geom_renderer->geometry()->attributes()[2];

    // Asserts primitive type.
    CPPUNIT_ASSERT_EQUAL(Qt3DRender::QGeometryRenderer::Triangles, geom_renderer->primitiveType());

    // Asserts number of vertices, normals, and indexes.
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(mesh->num_points()), pos_attribute->count());
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(mesh->num_points()), normal_attribute->count());
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(mesh->num_cells() * 3), index_attribute->count());

    // Asserts each point is at the right position.
    const QByteArray pos_buffer_data_byte = pos_attribute->buffer()->data();
    const auto* const pos_buffer_data     = reinterpret_cast<const float*>(pos_buffer_data_byte.data());
    unsigned int count                    = 0;
    for(const auto& p : mesh->crange<data::iterator::point::xyz>())
    {
        CPPUNIT_ASSERT(static_cast<float>(p.x) - pos_buffer_data[count] < 0.01F);
        CPPUNIT_ASSERT(static_cast<float>(p.y) - pos_buffer_data[count + 1] < 0.01F);
        CPPUNIT_ASSERT(static_cast<float>(p.z) - pos_buffer_data[count + 2] < 0.01F);
        count += 3;
    }

    // Asserts indexes are in the right order.
    const QByteArray index_buffer_data_byte = index_attribute->buffer()->data();
    const auto* const index_buffer_data     = reinterpret_cast<const unsigned int*>(index_buffer_data_byte.data());
    count = 0;
    for(const auto& cell : mesh->crange<data::iterator::cell::triangle>())
    {
        for(unsigned int i = 0 ; i < 3 ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(cell.pt[i]), index_buffer_data[count]);
            count++;
        }
    }

    delete qt3d_mesh;
    delete scene;
}

//------------------------------------------------------------------------------

void mesh_test::center_camera_on_cube()
{
    test_application app;

    // Arranges a cube data::mesh and a generic scene.
    auto* const scene  = new sight::viz::qt3d::core::generic_scene(false);
    auto* const camera = scene->get_camera();

    const data::mesh::sptr mesh = std::make_shared<data::mesh>();
    mesh->reserve(8, 12, data::mesh::cell_type_t::triangle, data::mesh::attribute::point_normals);

    const auto lock = mesh->dump_lock();

    mesh->push_point(0.0, 0.0, 0.0);
    mesh->push_point(1.0, 0.0, 0.0);
    mesh->push_point(1.0, 1.0, 0.0);
    mesh->push_point(0.0, 1.0, 0.0);
    mesh->push_point(0.0, 1.0, 1.0);
    mesh->push_point(0.0, 0.0, 1.0);
    mesh->push_point(1.0, 0.0, 1.0);
    mesh->push_point(1.0, 1.0, 1.0);

    mesh->push_cell(0, 1, 2);
    mesh->push_cell(2, 3, 0);
    mesh->push_cell(0, 3, 4);
    mesh->push_cell(4, 5, 0);
    mesh->push_cell(4, 5, 6);
    mesh->push_cell(6, 7, 4);
    mesh->push_cell(1, 6, 7);
    mesh->push_cell(7, 2, 1);
    mesh->push_cell(2, 3, 4);
    mesh->push_cell(4, 7, 2);
    mesh->push_cell(0, 1, 6);
    mesh->push_cell(6, 5, 0);

    // Sets expected camera after beeing centered on the cube.
    auto* expected_camera = new Qt3DRender::QCamera();
    expected_camera->lens()->setPerspectiveProjection(
        camera->lens()->fieldOfView(),
        camera->lens()->aspectRatio(),
        camera->lens()->nearPlane(),
        camera->lens()->farPlane()
    );
    expected_camera->setUpVector(camera->upVector());
    expected_camera->setPosition(camera->position());
    expected_camera->setViewCenter(camera->viewCenter());
    expected_camera->viewSphere(QVector3D(0.5F, 0.5F, 0.5F), 1);

    // Sets Qt3D mesh and center camera on it.
    auto* qt3d_mesh = new viz::qt3d::data::mesh(scene);
    qt3d_mesh->set_mesh(mesh);
    qt3d_mesh->center_camera_on_mesh();

    // Asserts actual camera's view center is equal to expected camera's one.
    CPPUNIT_ASSERT(expected_camera->viewCenter().x() - qt3d_mesh->get_scene()->get_camera()->viewCenter().x() < 0.01F);
    CPPUNIT_ASSERT(expected_camera->viewCenter().y() - qt3d_mesh->get_scene()->get_camera()->viewCenter().y() < 0.01F);
    CPPUNIT_ASSERT(expected_camera->viewCenter().z() - qt3d_mesh->get_scene()->get_camera()->viewCenter().z() < 0.01F);

    // Asserts actual camera's position is equal to expected camera's one.
    CPPUNIT_ASSERT(expected_camera->position().x() - qt3d_mesh->get_scene()->get_camera()->position().x() < 0.01F);
    CPPUNIT_ASSERT(expected_camera->position().y() - qt3d_mesh->get_scene()->get_camera()->position().y() < 0.01F);
    CPPUNIT_ASSERT(expected_camera->position().z() - qt3d_mesh->get_scene()->get_camera()->position().z() < 0.01F);

    delete expected_camera;
    delete qt3d_mesh;
    delete scene;
}

//------------------------------------------------------------------------------

} // namespace sight::viz::qt3d_test::ut
