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

#include "download_test.hpp"

#include "io/http/download.hpp"

#include <core/exception.hpp>
#include <core/os/temp_path.hpp>

#include <filesystem>

namespace fs = std::filesystem;

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::http::ut::download_test);

namespace sight::io::http::ut
{

//------------------------------------------------------------------------------

void download_test::setUp()
{
}

//------------------------------------------------------------------------------

void download_test::tearDown()
{
}

//------------------------------------------------------------------------------

void download_test::download_test_success()
{
    core::os::temp_dir tmp_dir;

    const auto& sample_bin_path = tmp_dir / "sample.bin";
    const auto& sample_txt_path = tmp_dir / "sample.txt";

    CPPUNIT_ASSERT_NO_THROW(
        io::http::download_file(
            "https://cloud.ircad.fr/s/tqYHyjZ2cgHT4mG/download",
            sample_bin_path
        )
    );

    CPPUNIT_ASSERT_NO_THROW(
        io::http::download_file(
            "https://cloud.ircad.fr/s/5GA5bjqsEYwenPT/download",
            sample_txt_path
        )
    );

    CPPUNIT_ASSERT(fs::exists(sample_bin_path));
    CPPUNIT_ASSERT(fs::exists(sample_txt_path));

    CPPUNIT_ASSERT_EQUAL(std::uintmax_t(1024), fs::file_size(sample_bin_path));
    CPPUNIT_ASSERT_EQUAL(std::uintmax_t(54), fs::file_size(sample_txt_path));
}

//------------------------------------------------------------------------------

void download_test::download_test_failure()
{
    core::os::temp_dir tmp_dir;
    const auto& test_bin_path = tmp_dir / "test.bin";

    CPPUNIT_ASSERT_THROW(
        io::http::download_file(
            "https://whateverTheCloud.com/fileDoesNotExists.txt/download",
            test_bin_path
        ),
        sight::core::exception
    );

    CPPUNIT_ASSERT(!fs::exists(test_bin_path));
}

//------------------------------------------------------------------------------

void download_test::download_test_wrong_inputs()
{
    core::os::temp_dir tmp_dir;
    const auto& test2_bin_path = tmp_dir / "test2.bin";

    CPPUNIT_ASSERT_THROW(io::http::download_file("", test2_bin_path), sight::core::exception);

    CPPUNIT_ASSERT_THROW(
        io::http::download_file("https://whateverTheCloud.com/fileDoesNotExists.txt/download", ""),
        sight::core::exception
    );
}

//------------------------------------------------------------------------------

void download_test::download_test_overwrites_file()
{
    core::os::temp_dir tmp_dir;
    const auto& sample_bin_path = tmp_dir / "sample.bin";

    CPPUNIT_ASSERT_NO_THROW(
        io::http::download_file(
            "https://cloud.ircad.fr/s/tqYHyjZ2cgHT4mG/download",
            sample_bin_path
        )
    );

    CPPUNIT_ASSERT(fs::exists(sample_bin_path));

    CPPUNIT_ASSERT_THROW(
        io::http::download_file(
            "https://cloud.ircad.fr/s/tqYHyjZ2cgHT4mG/download",
            sample_bin_path
        ),
        sight::core::exception
    );
}

} // namespace sight::io::http::ut
