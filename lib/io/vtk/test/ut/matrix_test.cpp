/************************************************************************
 *
 * Copyright (C) 2022-2025 IRCAD France
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

#include <data/matrix4.hpp>

#include <io/vtk/vtk.hpp>

#include <doctest/doctest.h>

#include <vtkMatrix4x4.h>

TEST_SUITE("sight::io::vtk::matrix")
{
//------------------------------------------------------------------------------

    TEST_CASE("to_vtk_matrix")
    {
        auto matrix = std::make_shared<sight::data::matrix4>();
        for(std::uint8_t i = 0 ; i < 4 ; i++)
        {
            for(std::uint8_t j = 0 ; j < 4 ; j++)
            {
                (*matrix)(i, j) = i * 10 + j;
            }
        }

        vtkSmartPointer<vtkMatrix4x4> vtk_matrix = sight::io::vtk::to_vtk_matrix(matrix);

        for(std::uint8_t i = 0 ; i < 4 ; i++)
        {
            for(std::uint8_t j = 0 ; j < 4 ; j++)
            {
                CHECK_MESSAGE(
                    i * 10. + j == vtk_matrix->GetElement(i, j),
                    "i=",
                    std::to_string(i),
                    " j=",
                    std::to_string(j)
                );
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("from_vtk_matrix")
    {
        auto vtk_matrix = vtkSmartPointer<vtkMatrix4x4>::New();
        for(std::uint8_t i = 0 ; i < 4 ; i++)
        {
            for(std::uint8_t j = 0 ; j < 4 ; j++)
            {
                vtk_matrix->SetElement(i, j, 10 * i + j);
            }
        }

        auto matrix = std::make_shared<sight::data::matrix4>();
        sight::io::vtk::from_vtk_matrix(vtk_matrix, matrix);

        for(std::uint8_t i = 0 ; i < 4 ; i++)
        {
            for(std::uint8_t j = 0 ; j < 4 ; j++)
            {
                CHECK_MESSAGE(
                    i * 10. + j == (*matrix)(i, j),
                    "i=",
                    std::to_string(i),
                    " j=",
                    std::to_string(j)
                );
            }
        }
    }

//------------------------------------------------------------------------------
} // TEST_SUITE
