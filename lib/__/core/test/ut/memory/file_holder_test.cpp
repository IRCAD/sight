/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

#include <core/memory/file_holder.hpp>
#include <core/os/temp_path.hpp>

#include <doctest/doctest.h>

#include <filesystem>
#include <fstream>

TEST_SUITE("sight::core::memory::file_holder")
{
//------------------------------------------------------------------------------

    TEST_CASE("file_auto_delete")
    {
        sight::core::os::temp_file temp_file;
        {
            sight::core::memory::file_holder holder(temp_file, true);
            std::ofstream out(temp_file);
            out << 1;
            out.close();

            REQUIRE(std::filesystem::exists(temp_file));
            CHECK(std::filesystem::is_regular_file(temp_file));
            CHECK(std::filesystem::file_size(temp_file) > 0);
        }

        CHECK(!std::filesystem::exists(temp_file));
    }
} // TEST_SUITE
