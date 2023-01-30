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

#include "Matrix4ReaderTest.hpp"

#include <data/Matrix4.hpp>

#include <io/base/reader/Matrix4Reader.hpp>

#include <filesystem>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::base::ut::Matrix4ReaderTest);

namespace sight::io::base::ut
{

//------------------------------------------------------------------------------

void Matrix4ReaderTest::basicTest()
{
    constexpr std::string_view fileContent = R"(
0 1 2 3
10 11 12 13
20 21 22 23
30 31 32 33
)";
    auto matrixReader                      = reader::Matrix4Reader::New();
    std::filesystem::path filepath         = std::filesystem::temp_directory_path()
                                             / ("test" + matrixReader->extension());
    {
        std::ofstream out(filepath);
        out << fileContent;
    }
    auto matrixOut = data::Matrix4::New();
    matrixReader->setObject(matrixOut);
    matrixReader->setFile(filepath);
    CPPUNIT_ASSERT_NO_THROW(matrixReader->read());
    for(std::uint8_t i = 0 ; i < 4 ; i++)
    {
        for(std::uint8_t j = 0 ; j < 4 ; j++)
        {
            CPPUNIT_ASSERT_EQUAL(j + 10. * i, (*matrixOut)(i, j));
        }
    }
}

} // namespace sight::io::base::ut
