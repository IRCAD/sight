/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
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

#include <data/Mesh.hpp>
#include <data/Point.hpp>

#include <geometry/vtk/mesh.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::geometry::vtk::ut::vtkTest );

namespace sight::geometry::vtk
{
namespace ut
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
    sight::data::Mesh::sptr mesh             = sight::data::Mesh::New();
    const auto dumpLock                      = mesh->lock();
    const std::array<double, 3> centerOfMass = {0.5, 0.5, 0.5};

    mesh->pushPoint(0.f, 0.f, 0.f);
    mesh->pushPoint(1.f, 0.f, 0.f);
    mesh->pushPoint(1.f, 1.f, 0.f);
    mesh->pushPoint(0.f, 1.f, 0.f);
    mesh->pushPoint(0.f, 0.f, 1.f);
    mesh->pushPoint(1.f, 0.f, 1.f);
    mesh->pushPoint(1.f, 1.f, 1.f);
    mesh->pushPoint(0.f, 1.f, 1.f);

    mesh->pushCell(0, 1, 2, 3);
    mesh->pushCell(0, 4, 5, 1);
    mesh->pushCell(1, 5, 6, 2);
    mesh->pushCell(4, 5, 6, 7);
    mesh->pushCell(0, 4, 7, 3);
    mesh->pushCell(3, 7, 6, 2);
    sight::data::Point::sptr computedCenterOfMass = geometry::vtk::computeCenterOfMass(mesh);

    CPPUNIT_ASSERT_EQUAL(centerOfMass[0], computedCenterOfMass->getCoord()[0]);
    CPPUNIT_ASSERT_EQUAL(centerOfMass[1], computedCenterOfMass->getCoord()[1]);
    CPPUNIT_ASSERT_EQUAL(centerOfMass[2], computedCenterOfMass->getCoord()[2]);
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace sight::geometry::vtk
