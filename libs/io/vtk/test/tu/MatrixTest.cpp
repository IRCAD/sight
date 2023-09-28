/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "MatrixTest.hpp"

// #include <data/TransferFunction.hpp>

// #include <io/vtk/helper/TransferFunction.hpp>

// #include <cmath>

#include <data/Matrix4.hpp>
#include <vtkMatrix4x4.h>
#include <io/vtk/vtk.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::vtk::ut::MatrixTest);

namespace sight::io::vtk::ut
{

//------------------------------------------------------------------------------

void MatrixTest::toVtkMatrixTest()
{
    auto matrix = std::make_shared<data::Matrix4>();
    for(std::uint8_t i = 0 ; i < 4 ; i++)
    {
        for(std::uint8_t j = 0 ; j < 4 ; j++)
        {
            (*matrix)(i, j) = i * 10 + j;
        }
    }

    vtkSmartPointer<vtkMatrix4x4> vtkMatrix = toVTKMatrix(matrix);

    for(std::uint8_t i = 0 ; i < 4 ; i++)
    {
        for(std::uint8_t j = 0 ; j < 4 ; j++)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "i=" + std::to_string(i) + " j=" + std::to_string(j),
                i * 10. + j,
                vtkMatrix->GetElement(i, j)
            );
        }
    }
}

//------------------------------------------------------------------------------

void MatrixTest::fromVtkMatrixTest()
{
    auto vtkMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    for(std::uint8_t i = 0 ; i < 4 ; i++)
    {
        for(std::uint8_t j = 0 ; j < 4 ; j++)
        {
            vtkMatrix->SetElement(i, j, 10 * i + j);
        }
    }

    auto matrix = std::make_shared<data::Matrix4>();
    fromVTKMatrix(vtkMatrix, matrix);

    for(std::uint8_t i = 0 ; i < 4 ; i++)
    {
        for(std::uint8_t j = 0 ; j < 4 ; j++)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "i=" + std::to_string(i) + " j=" + std::to_string(j),
                i * 10. + j,
                (*matrix)(i, j)
            );
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk::ut
