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

#include "io/http/download.hpp"

#include <core/exception.hpp>
#include <core/os/temp_path.hpp>

#include <doctest/doctest.h>

#include <filesystem>

namespace fs = std::filesystem;

TEST_SUITE("sight::io::http::download")
{
    TEST_CASE("download_success")
    {
        sight::core::os::temp_dir tmp_dir;

        const auto& sample_bin_path = tmp_dir / "sample.bin";
        const auto& sample_txt_path = tmp_dir / "sample.txt";

        CHECK_NOTHROW(
            sight::io::http::download_file(
                "https://cloud.ircad.fr/s/tqYHyjZ2cgHT4mG/download",
                sample_bin_path
            )
        );

        CHECK_NOTHROW(
            sight::io::http::download_file(
                "https://cloud.ircad.fr/s/5GA5bjqsEYwenPT/download",
                sample_txt_path
            )
        );

        CHECK(fs::exists(sample_bin_path));
        CHECK(fs::exists(sample_txt_path));

        CHECK_EQ(std::uintmax_t(1024), fs::file_size(sample_bin_path));
        CHECK_EQ(std::uintmax_t(54), fs::file_size(sample_txt_path));
    }

    TEST_CASE("download_failure")
    {
        sight::core::os::temp_dir tmp_dir;
        const auto& test_bin_path = tmp_dir / "test.bin";

        CHECK_THROWS_AS(
            sight::io::http::download_file(
                "https://whateverTheCloud.com/fileDoesNotExists.txt/download",
                test_bin_path
            ),
            sight::core::exception
        );

        CHECK(!fs::exists(test_bin_path));
    }

    TEST_CASE("download_wrong_inputs")
    {
        sight::core::os::temp_dir tmp_dir;
        const auto& test2_bin_path = tmp_dir / "test2.bin";

        CHECK_THROWS_AS(sight::io::http::download_file("", test2_bin_path), sight::core::exception);

        CHECK_THROWS_AS(
            sight::io::http::download_file("https://whateverTheCloud.com/fileDoesNotExists.txt/download", ""),
            sight::core::exception
        );
    }

    TEST_CASE("download_overwrites_file")
    {
        sight::core::os::temp_dir tmp_dir;
        const auto& sample_bin_path = tmp_dir / "sample.bin";

        CHECK_NOTHROW(
            sight::io::http::download_file(
                "https://cloud.ircad.fr/s/tqYHyjZ2cgHT4mG/download",
                sample_bin_path
            )
        );

        CHECK(fs::exists(sample_bin_path));

        CHECK_THROWS_AS(
            sight::io::http::download_file(
                "https://cloud.ircad.fr/s/tqYHyjZ2cgHT4mG/download",
                sample_bin_path
            ),
            sight::core::exception
        );
    }
} // TEST_SUITE
