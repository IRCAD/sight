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

#include "matrix4reader_test.hpp"

#include <core/os/temp_path.hpp>

#include <data/matrix4.hpp>

#include <io/__/reader/matrix4_reader.hpp>

#include <filesystem>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::ut::matrix4_reader_test);

namespace sight::io::ut
{

//------------------------------------------------------------------------------

void matrix4_reader_test::basic_test()
{
    constexpr std::string_view file_content = R"(
0 1 2 3
10 11 12 13
20 21 22 23
30 31 32 33
)";
    auto matrix_reader                      = std::make_shared<reader::matrix4_reader>();
    core::os::temp_dir temp_dir;
    std::filesystem::path filepath = temp_dir / ("test" + matrix_reader->extension());
    {
        std::ofstream out(filepath);
        out << file_content;
    }
    auto matrix_out = std::make_shared<data::matrix4>();
    matrix_reader->set_object(matrix_out);
    matrix_reader->set_file(filepath);
    CPPUNIT_ASSERT_NO_THROW(matrix_reader->read());
    for(std::uint8_t i = 0 ; i < 4 ; i++)
    {
        for(std::uint8_t j = 0 ; j < 4 ; j++)
        {
            CPPUNIT_ASSERT_EQUAL(j + 10. * i, (*matrix_out)(i, j));
        }
    }
}

} // namespace sight::io::ut
