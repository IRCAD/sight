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

#include "GzArrayReaderTest.hpp"

#include <data/array.hpp>

#include <io/__/reader/gz_array_reader.hpp>

#include <zlib.h>

#include <filesystem>
#include <numeric>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::ut::GzArrayReaderTest);

namespace sight::io::ut
{

//------------------------------------------------------------------------------

void GzArrayReaderTest::basicTest()
{
    auto gzArrayReader             = std::make_shared<reader::gz_array_reader>();
    std::filesystem::path filepath = std::filesystem::temp_directory_path() / ("test" + gzArrayReader->extension());
    std::array<std::uint8_t, 16> arrayIn {};
    std::iota(arrayIn.begin(), arrayIn.end(), std::uint8_t(0));
    gzFile out = gzopen(filepath.string().c_str(), "wb");
    gzwrite(out, reinterpret_cast<char*>(arrayIn.data()), 16);
    gzclose(out);
    auto arrayOut = std::make_shared<data::array>();
    arrayOut->resize({16}, core::type::UINT8);
    gzArrayReader->set_object(arrayOut);
    gzArrayReader->set_file(filepath);
    CPPUNIT_ASSERT_NO_THROW(gzArrayReader->read());
    {
        auto arrayLock = arrayOut->dump_lock();
        std::uint8_t i = 0;
        for(auto it = arrayOut->begin<std::uint8_t>(), end = arrayOut->end<std::uint8_t>() ; it != end ; ++it, ++i)
        {
            CPPUNIT_ASSERT_EQUAL(i, *it);
        }
    }
}

} // namespace sight::io::ut
