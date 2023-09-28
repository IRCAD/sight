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

#include "GzBufferImageWriterTest.hpp"

#include <data/Image.hpp>

#include <io/__/writer/GzBufferImageWriter.hpp>

#include <zlib.h>

#include <numeric>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::ut::GzBufferImageWriterTest);

namespace sight::io::ut
{

//------------------------------------------------------------------------------

void GzBufferImageWriterTest::basicTest()
{
    auto gzBufferImageWriter       = std::make_shared<writer::GzBufferImageWriter>();
    std::filesystem::path filepath = std::filesystem::temp_directory_path()
                                     / ("test" + gzBufferImageWriter->extension());
    auto imageIn = std::make_shared<data::Image>();
    imageIn->resize({16}, core::type::UINT8, data::Image::PixelFormat::GRAY_SCALE);
    {
        auto imageLock = imageIn->dump_lock();
        std::iota(imageIn->begin<std::uint8_t>(), imageIn->end<std::uint8_t>(), std::uint8_t(0));
    }
    gzBufferImageWriter->setObject(imageIn);
    std::filesystem::remove(filepath);
    gzBufferImageWriter->set_file(filepath);
    /* TODO: fix
       CPPUNIT_ASSERT_NO_THROW(gzBufferImageWriter->write());
       std::array<std::uint8_t, 16> array;
       {
        gzFile out = gzopen(filepath.c_str(), "rb");
        gzread(out, array.data(), 16);
        gzclose(out);
       }
       for(std::uint8_t i = 0; i < 16; i++){
        CPPUNIT_ASSERT_EQUAL(i, array[i]);
       }
     */
}

} // namespace sight::io::ut
