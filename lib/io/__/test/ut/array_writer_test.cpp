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
#include <core/type.hpp>

#include <data/array.hpp>

#include <io/__/writer/array_writer.hpp>

#include <doctest/doctest.h>

#include <filesystem>
#include <numeric>

TEST_SUITE("sight::io::array")
{
    TEST_CASE("writer")
    {
        auto array_writer = std::make_shared<sight::io::writer::array_writer>();
        sight::core::os::temp_dir temp_dir;
        std::filesystem::path filepath = temp_dir / ("test" + array_writer->extension());
        auto array_in                  = std::make_shared<sight::data::array>();
        array_in->resize({16}, sight::core::type::UINT8);
        {
            auto array_lock = array_in->dump_lock();
            std::iota(array_in->begin<std::uint8_t>(), array_in->end<std::uint8_t>(), std::uint8_t(0));
        }
        array_writer->set_object(array_in);
        std::filesystem::remove(filepath);
        array_writer->set_file(filepath);
        const auto observer = std::make_shared<sight::core::progress::observer>("ARRAY Writer Test");
        CHECK_NOTHROW(array_writer->write(observer));
        std::array<std::uint8_t, 16> array {};
        {
            std::ifstream in(filepath);
            in.read(reinterpret_cast<char*>(array.data()), 16);
        }
        for(std::uint8_t i = 0 ; i < 16 ; i++)
        {
            CHECK_EQ(i, array[i]);
        }
    }
}
