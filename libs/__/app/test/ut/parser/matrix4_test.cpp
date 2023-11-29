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

#include "matrix4_test.hpp"

#include <app/parser/matrix4.hpp>

#include <data/matrix4.hpp>

#include <boost/property_tree/ptree.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::app::parser::ut::matrix4_test);

namespace sight::app::parser::ut
{

//------------------------------------------------------------------------------

void matrix4_test::basic_test()
{
    boost::property_tree::ptree ptree;
    ptree.put("matrix", R"(
        0 1 2 3
        10 11 12 13
        20 21 22 23
        30 31 32 33
    )");
    auto matrix = std::make_shared<data::matrix4>();
    parser::matrix4 matrix_parser;
    matrix_parser.set_object_config(ptree);
    matrix_parser.create_config(matrix);
    for(std::uint8_t i = 0 ; i < 4 ; i++)
    {
        for(std::uint8_t j = 0 ; j < 4 ; j++)
        {
            CPPUNIT_ASSERT_EQUAL(j + 10. * i, (*matrix)(i, j));
        }
    }
}

} // namespace sight::app::parser::ut
