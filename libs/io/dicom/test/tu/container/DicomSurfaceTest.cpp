/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "DicomSurfaceTest.hpp"

#include <data/Mesh.hpp>
#include <data/Reconstruction.hpp>

#include <io/dicom/container/DicomSurface.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::container::ut::DicomSurfaceTest);

namespace sight::io::dicom::container::ut
{

//------------------------------------------------------------------------------

void DicomSurfaceTest::basicTest()
{
    auto mesh = data::Mesh::New();
    mesh->reserve(4, 3, data::Mesh::CellType::TRIANGLE, data::Mesh::Attributes::POINT_NORMALS);
    {
        auto meshLock = mesh->dump_lock();
        mesh->pushPoint(0, 1, 2);
        mesh->pushPoint(3, 4, 5);
        mesh->pushPoint(6, 7, 8);
        mesh->pushPoint(9, 10, 11);
        mesh->setPointNormal(0, 0, 1, 2);
        mesh->setPointNormal(1, 3, 4, 5);
        mesh->setPointNormal(2, 6, 7, 8);
        mesh->setPointNormal(3, 9, 10, 11);
        mesh->pushCell(0, 1, 2);
        mesh->pushCell(3, 0, 1);
        mesh->pushCell(2, 3, 0);
    }
    auto reconstruction = data::Reconstruction::New();
    reconstruction->setMesh(mesh);
    auto meshLock = mesh->dump_lock(); // TODO: Fix failure because mesh isn't dump locked.
    DicomSurface dicomSurface(reconstruction);

    CPPUNIT_ASSERT_EQUAL(std::size_t(4 * 3), dicomSurface.getPointBufferSize());
    std::uint8_t i = 0;
    for(float e : dicomSurface.getPointBuffer())
    {
        CPPUNIT_ASSERT_EQUAL(float(i), e);
        i++;
    }

    CPPUNIT_ASSERT_EQUAL(std::size_t(4 * 3), dicomSurface.getNormalBufferSize());
    i = 0;
    for(float e : dicomSurface.getNormalBuffer())
    {
        CPPUNIT_ASSERT_EQUAL(float(i), e);
        i++;
    }

    CPPUNIT_ASSERT_EQUAL(std::size_t(3 * 3), dicomSurface.getCellBufferSize());
    i = 0;
    for(std::uint32_t e : dicomSurface.getCellBuffer())
    {
        // Indices start a 1 in DICOM, hence the "+ 1".
        CPPUNIT_ASSERT_EQUAL_MESSAGE("i=" + std::to_string(i), i % 4U + 1, e);
        i++;
    }

    CPPUNIT_ASSERT(*mesh == *dicomSurface.convertToData());
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::container::ut
