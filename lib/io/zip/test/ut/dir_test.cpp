/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <io/zip/read_dir_archive.hpp>
#include <io/zip/write_dir_archive.hpp>

#include <utest_data/data.hpp>

#include <doctest/doctest.h>

#include <filesystem>

TEST_SUITE("sight::io::zip::dir")
{
    TEST_CASE("write_read_file")
    {
        sight::core::os::temp_dir tmp_dir;

        auto writer = std::make_shared<sight::io::zip::write_dir_archive>(tmp_dir);
        auto reader = std::make_shared<sight::io::zip::read_dir_archive>(tmp_dir);

        const std::filesystem::path test_file = "test.txt";

        // Test that create_file doesn't throw
        CHECK_NOTHROW(writer->create_file(test_file));

        // Test that get_file doesn't throw
        CHECK_NOTHROW(reader->get_file(test_file));
    }

    TEST_CASE("write_dir")
    {
        sight::core::os::temp_dir tmp_dir;

        auto writer = std::make_shared<sight::io::zip::write_dir_archive>(tmp_dir);

        const std::filesystem::path test_dir = "test";

        // Test that create_dir doesn't throw
        CHECK_NOTHROW(writer->create_dir(test_dir));

        CHECK(std::filesystem::exists(tmp_dir / test_dir));
    }

    TEST_CASE("put_file")
    {
        sight::core::os::temp_dir tmp_dir;

        auto writer = std::make_shared<sight::io::zip::write_dir_archive>(tmp_dir);

        //cspell: ignore makao
        const std::filesystem::path test_file = sight::utest_data::dir() / "sight/image/jpg/makao01.jpg";
        CHECK_MESSAGE(
            std::filesystem::exists(test_file),
            "The file '",
            test_file.string(),
            "' does not exist"
        );

        // Test that put_file doesn't throw
        CHECK_NOTHROW(writer->put_file(test_file, "image.jpg"));

        CHECK(std::filesystem::exists(tmp_dir / "image.jpg"));
    }
} // TEST_SUITE
