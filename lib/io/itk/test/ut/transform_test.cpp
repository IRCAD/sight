/************************************************************************
 *
 * Copyright (C) 2017-2025 IRCAD France
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

#include <data/matrix4.hpp>

#include <io/itk/helper/transform.hpp>

#include <doctest/doctest.h>

#include <itkMatrix.h>

TEST_SUITE("sight::io::itk::transform")
{
    TEST_CASE("conversion")
    {
        ::itk::Matrix<double, 4, 4> mat;
        auto sight_mat = std::make_shared<sight::data::matrix4>();

        for(std::uint8_t i = 0 ; i < 4 ; ++i)
        {
            for(std::uint8_t j = 0 ; j < 4 ; j++)
            {
                mat(i, j) = i + j * 4.0;
            }
        }

        sight::io::itk::helper::transform::convert_from_itk(mat, sight_mat);

        for(std::uint8_t i = 0 ; i < 4 ; ++i)
        {
            for(std::uint8_t j = 0 ; j < 4 ; j++)
            {
                CHECK_EQ(mat(i, j), (*sight_mat)(i, j));
            }
        }

        sight::data::matrix4::csptr sight_mat2 = sight_mat;

        ::itk::Matrix<double, 4, 4> mat2 = sight::io::itk::helper::transform::convert_to_itk(sight_mat2);

        for(std::uint8_t i = 0 ; i < 4 ; ++i)
        {
            for(std::uint8_t j = 0 ; j < 4 ; j++)
            {
                CHECK_EQ(mat(i, j), mat2(i, j));
            }
        }
    }
} // TEST_SUITE
