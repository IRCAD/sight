/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include <core/os/temp_path.hpp>
#include <core/progress/observer.hpp>

#include <data/matrix4.hpp>

#include <io/__/reader/matrix4_reader.hpp>

#include <doctest/doctest.h>

#include <filesystem>

TEST_SUITE("sight::io::matrix4")
{
    TEST_CASE("reader")
    {
        constexpr std::string_view file_content = R"(
0 1 2 3
10 11 12 13
20 21 22 23
30 31 32 33
)";
        auto matrix_reader                      = std::make_shared<sight::io::reader::matrix4_reader>();
        sight::core::os::temp_dir temp_dir;
        std::filesystem::path filepath = temp_dir / ("test" + matrix_reader->extension());
        {
            std::ofstream out(filepath);
            out << file_content;
        }
        auto matrix_out = std::make_shared<sight::data::matrix4>();
        matrix_reader->set_object(matrix_out);
        matrix_reader->set_file(filepath);
        const auto observer = std::make_shared<sight::core::progress::observer>("Matrix4 Reader Test");
        CHECK_NOTHROW(matrix_reader->read(observer));
        for(std::uint8_t i = 0 ; i < 4 ; i++)
        {
            for(std::uint8_t j = 0 ; j < 4 ; j++)
            {
                CHECK_EQ(j + 10. * i, (*matrix_out)(i, j));
            }
        }
    }
}
