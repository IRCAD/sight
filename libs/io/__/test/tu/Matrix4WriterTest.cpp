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

#include "Matrix4WriterTest.hpp"

#include <data/matrix4.hpp>

#include <io/__/writer/matrix4_writer.hpp>

#include <boost/algorithm/string.hpp>

#include <numeric>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::ut::Matrix4WriterTest);

namespace sight::io::ut
{

//------------------------------------------------------------------------------

void Matrix4WriterTest::basicTest()
{
    auto matrix_writer             = std::make_shared<writer::matrix4_writer>();
    std::filesystem::path filepath = std::filesystem::temp_directory_path() / ("test" + matrix_writer->extension());
    auto matrix_in                 = std::make_shared<data::matrix4>();
    std::iota(matrix_in->begin(), matrix_in->end(), 0);
    matrix_writer->set_object(matrix_in);
    std::filesystem::remove(filepath);
    matrix_writer->set_file(filepath);
    CPPUNIT_ASSERT_NO_THROW(matrix_writer->write());
    constexpr std::string_view expected_content = R"(0 1 2 3
4 5 6 7
8 9 10 11
12 13 14 15)";
    std::string actual_content;
    {
        std::ifstream in(filepath);
        std::getline(in, actual_content, '\0');
        boost::trim(actual_content);
    }
    CPPUNIT_ASSERT_EQUAL(std::string(expected_content), actual_content);
}

} // namespace sight::io::ut
