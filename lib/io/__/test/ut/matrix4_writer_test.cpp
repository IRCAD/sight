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

#include <core/notification/observer.hpp>
#include <core/os/temp_path.hpp>

#include <data/matrix4.hpp>

#include <io/__/writer/matrix4_writer.hpp>

#include <doctest/doctest.h>

#include <numeric>

TEST_SUITE("sight::io::matrix4")
{
    TEST_CASE("writer")
    {
        auto matrix_writer = std::make_shared<sight::io::writer::matrix4_writer>();
        sight::core::os::temp_dir temp_dir;
        std::filesystem::path filepath = temp_dir / ("test" + matrix_writer->extension());
        auto matrix_in                 = std::make_shared<sight::data::matrix4>();
        std::iota(matrix_in->begin(), matrix_in->end(), 0);
        matrix_writer->set_object(matrix_in);
        std::filesystem::remove(filepath);
        matrix_writer->set_file(filepath);
        auto observer = std::make_shared<sight::core::notification::observer>("Test write");
        CHECK_NOTHROW(matrix_writer->write(observer));
        constexpr std::string_view expected_content = R"(0 1 2 3
4 5 6 7
8 9 10 11
12 13 14 15)";
        std::string actual_content;
        {
            std::ifstream in(filepath);
            std::getline(in, actual_content, '\0');
            // Trim whitespace
            const auto start = actual_content.find_first_not_of(" \t\n\r\f\v");
            const auto end   = actual_content.find_last_not_of(" \t\n\r\f\v");
            if(start != std::string::npos)
            {
                actual_content = actual_content.substr(start, end - start + 1);
            }
        }
        CHECK_EQ(std::string(expected_content), actual_content);
    }
}
