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

#include "matrix_test.hpp"

// #include <data/transfer_function.hpp>

// #include <io/vtk/helper/transfer_function.hpp>

// #include <cmath>

#include <data/matrix4.hpp>
#include <vtkMatrix4x4.h>
#include <io/vtk/vtk.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::vtk::ut::matrix_test);

namespace sight::io::vtk::ut
{

//------------------------------------------------------------------------------

void matrix_test::to_vtk_matrix_test()
{
    auto matrix = std::make_shared<data::matrix4>();
    for(std::uint8_t i = 0 ; i < 4 ; i++)
    {
        for(std::uint8_t j = 0 ; j < 4 ; j++)
        {
            (*matrix)(i, j) = i * 10 + j;
        }
    }

    vtkSmartPointer<vtkMatrix4x4> vtk_matrix = to_vtk_matrix(matrix);

    for(std::uint8_t i = 0 ; i < 4 ; i++)
    {
        for(std::uint8_t j = 0 ; j < 4 ; j++)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "i=" + std::to_string(i) + " j=" + std::to_string(j),
                i * 10. + j,
                vtk_matrix->GetElement(i, j)
            );
        }
    }
}

//------------------------------------------------------------------------------

void matrix_test::from_vtk_matrix_test()
{
    auto vtk_matrix = vtkSmartPointer<vtkMatrix4x4>::New();
    for(std::uint8_t i = 0 ; i < 4 ; i++)
    {
        for(std::uint8_t j = 0 ; j < 4 ; j++)
        {
            vtk_matrix->SetElement(i, j, 10 * i + j);
        }
    }

    auto matrix = std::make_shared<data::matrix4>();
    from_vtk_matrix(vtk_matrix, matrix);

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
