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

#include <data/array.hpp>

#include <io/__/reader/gz_array_reader.hpp>

#include <doctest/doctest.h>

#include <zlib.h>

#include <filesystem>
#include <numeric>

TEST_SUITE("sight::io::gz_array")
{
    TEST_CASE("reader")
    {
        auto gz_array_reader = std::make_shared<sight::io::reader::gz_array_reader>();
        sight::core::os::temp_dir temp_dir;
        std::filesystem::path filepath = temp_dir / ("test" + gz_array_reader->extension());
        std::array<std::uint8_t, 16> array_in {};
        std::iota(array_in.begin(), array_in.end(), std::uint8_t(0));
        gzFile out = gzopen(filepath.string().c_str(), "wb");
        gzwrite(out, reinterpret_cast<char*>(array_in.data()), 16);
        gzclose(out);
        auto array_out = std::make_shared<sight::data::array>();
        array_out->resize({16}, sight::core::type::UINT8);
        gz_array_reader->set_object(array_out);
        gz_array_reader->set_file(filepath);
        const auto observer = std::make_shared<sight::core::progress::observer>("ARRAY Reader Test");
        CHECK_NOTHROW(gz_array_reader->read(observer));
        {
            auto array_lock = array_out->dump_lock();
            std::uint8_t i  = 0;
            for(auto it = array_out->begin<std::uint8_t>(), end = array_out->end<std::uint8_t>() ;
                it != end ;
                ++it, ++i)
            {
                CHECK_EQ(i, *it);
            }
        }
    }
}
