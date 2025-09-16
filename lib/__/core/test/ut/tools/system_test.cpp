/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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
#include <core/tools/system.hpp>

#include <doctest/doctest.h>

#include <fstream>

TEST_SUITE("sight::core::tools::system")
{
//------------------------------------------------------------------------------

    TEST_CASE("robust_rename")
    {
        sight::core::os::temp_dir tmp_dir;

        const auto origin_file = tmp_dir / "test.sight";

        // Create fake file.
        {
            std::fstream fs;
            fs.open(origin_file, std::ios::out);
            fs.close();
        }

        const auto destination_file = tmp_dir / "test1000.sight";

        //1. Basic renaming.
        CHECK_NOTHROW(sight::core::tools::system::robust_rename(origin_file, destination_file));

        CHECK_MESSAGE("Destination file should exist.", std::filesystem::exists(destination_file));
        CHECK_MESSAGE("Origin file shouldn't exist", !std::filesystem::exists(origin_file));

        // 2. Should throw an exception.
        CHECK_THROWS_AS(
            sight::core::tools::system::robust_rename(origin_file, destination_file),
            std::filesystem::filesystem_error
        );

        // 3. Should NOT throw an exception, even if the original file already exists.
        // (re)create the fake file.
        {
            std::fstream fs;
            fs.open(origin_file, std::ios::out);
            fs.close();
        }

        CHECK_NOTHROW(sight::core::tools::system::robust_rename(destination_file, origin_file, true));
        CHECK_NOTHROW(sight::core::tools::system::robust_rename(origin_file, destination_file, true));
        CHECK_MESSAGE("Destination file should exist.", std::filesystem::exists(destination_file));
        CHECK_MESSAGE("Origin file shouldn't exist", !std::filesystem::exists(origin_file));

        // 4. Should do nothing if both path are indeed the same file
        CHECK_NOTHROW(sight::core::tools::system::robust_rename(destination_file, destination_file));

        // Clean up.
        std::filesystem::remove(destination_file);
    }
} // TEST_SUITE
