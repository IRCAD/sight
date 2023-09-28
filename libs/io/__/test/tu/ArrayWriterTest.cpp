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

#include "ArrayWriterTest.hpp"

#include <core/type.hpp>

#include <data/Array.hpp>

#include <io/__/writer/ArrayWriter.hpp>

#include <filesystem>
#include <numeric>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::ut::ArrayWriterTest);

namespace sight::io::ut
{

//------------------------------------------------------------------------------

void ArrayWriterTest::basicTest()
{
    auto arrayWriter               = std::make_shared<writer::ArrayWriter>();
    std::filesystem::path filepath = std::filesystem::temp_directory_path() / ("test" + arrayWriter->extension());
    auto arrayIn                   = std::make_shared<data::Array>();
    arrayIn->resize({16}, core::type::UINT8);
    {
        auto arrayLock = arrayIn->dump_lock();
        std::iota(arrayIn->begin<std::uint8_t>(), arrayIn->end<std::uint8_t>(), std::uint8_t(0));
    }
    arrayWriter->setObject(arrayIn);
    std::filesystem::remove(filepath);
    arrayWriter->set_file(filepath);
    CPPUNIT_ASSERT_NO_THROW(arrayWriter->write());
    std::array<std::uint8_t, 16> array {};
    {
        std::ifstream in(filepath);
        in.read(reinterpret_cast<char*>(array.data()), 16);
    }
    for(std::uint8_t i = 0 ; i < 16 ; i++)
    {
        CPPUNIT_ASSERT_EQUAL(i, array[i]);
    }
}

} // namespace sight::io::ut
