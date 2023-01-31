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

#include "GzArrayWriterTest.hpp"

#include <core/Type.hpp>

#include <data/Array.hpp>

#include <io/base/writer/GzArrayWriter.hpp>

#include <zlib.h>

#include <numeric>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::base::ut::GzArrayWriterTest);

namespace sight::io::base::ut
{

//------------------------------------------------------------------------------

void GzArrayWriterTest::basicTest()
{
    auto gzArrayWriter             = writer::GzArrayWriter::New();
    std::filesystem::path filepath = std::filesystem::temp_directory_path() / ("test" + gzArrayWriter->extension());
    auto arrayIn                   = data::Array::New();
    arrayIn->resize({16}, core::Type::UINT8);
    {
        auto arrayLock = arrayIn->dump_lock();
        std::iota(arrayIn->begin<std::uint8_t>(), arrayIn->end<std::uint8_t>(), std::uint8_t(0));
    }
    gzArrayWriter->setObject(arrayIn);
    std::filesystem::remove(filepath);
    gzArrayWriter->setFile(filepath);
    CPPUNIT_ASSERT_NO_THROW(gzArrayWriter->write());
    std::array<std::uint8_t, 16> array {};
    gzFile out = gzopen(filepath.string().c_str(), "rb");
    gzread(out, array.data(), 16);
    gzclose(out);
    for(std::uint8_t i = 0 ; i < 16 ; i++)
    {
        CPPUNIT_ASSERT_EQUAL(i, array[i]);
    }
}

} // namespace sight::io::base::ut
