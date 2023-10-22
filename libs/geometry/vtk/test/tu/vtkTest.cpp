/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include "vtkTest.hpp"

#include <data/mesh.hpp>
#include <data/point.hpp>

#include <geometry/vtk/mesh.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::geometry::vtk::ut::vtkTest);

namespace sight::geometry::vtk::ut
{

//------------------------------------------------------------------------------

void vtkTest::setUp()
{
}

//------------------------------------------------------------------------------

void vtkTest::tearDown()
{
}

//------------------------------------------------------------------------------

void vtkTest::computeCenterOfMass()
{
    sight::data::mesh::sptr mesh               = std::make_shared<sight::data::mesh>();
    const auto dump_lock                       = mesh->dump_lock();
    const std::array<double, 3> center_of_mass = {0.5, 1.5, 2.5};

    mesh->pushPoint(0.F, 0.F, 0.F);
    mesh->pushPoint(1.F, 0.F, 0.F);
    mesh->pushPoint(1.F, 3.F, 0.F);
    mesh->pushPoint(0.F, 3.F, 0.F);
    mesh->pushPoint(0.F, 0.F, 5.F);
    mesh->pushPoint(1.F, 0.F, 5.F);
    mesh->pushPoint(1.F, 3.F, 5.F);
    mesh->pushPoint(0.F, 3.F, 5.F);

    mesh->pushCell(0, 1, 2, 3);
    mesh->pushCell(0, 4, 5, 1);
    mesh->pushCell(1, 5, 6, 2);
    mesh->pushCell(4, 5, 6, 7);
    mesh->pushCell(0, 4, 7, 3);
    mesh->pushCell(3, 7, 6, 2);
    sight::data::point::sptr computed_center_of_mass = geometry::vtk::compute_center_of_mass(mesh);

    CPPUNIT_ASSERT_EQUAL(center_of_mass[0], computed_center_of_mass->getCoord()[0]);
    CPPUNIT_ASSERT_EQUAL(center_of_mass[1], computed_center_of_mass->getCoord()[1]);
    CPPUNIT_ASSERT_EQUAL(center_of_mass[2], computed_center_of_mass->getCoord()[2]);
}

//------------------------------------------------------------------------------

} // namespace sight::geometry::vtk::ut
