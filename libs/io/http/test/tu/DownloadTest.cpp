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

#include "DownloadTest.hpp"

#include "io/http/Download.hpp"

#include <core/exception.hpp>
#include <core/os/temp_path.hpp>

#include <filesystem>

namespace fs = std::filesystem;

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::http::ut::DownloadTest);

namespace sight::io::http::ut
{

//------------------------------------------------------------------------------

void DownloadTest::setUp()
{
}

//------------------------------------------------------------------------------

void DownloadTest::tearDown()
{
}

//------------------------------------------------------------------------------

void DownloadTest::downloadTestSuccess()
{
    core::os::temp_dir tmpDir;

    const auto& sample_bin_path = tmpDir / "sample.bin";
    const auto& sample_txt_path = tmpDir / "sample.txt";

    CPPUNIT_ASSERT_NO_THROW(
        io::http::downloadFile(
            "https://cloud.ircad.fr/s/tqYHyjZ2cgHT4mG/download",
            sample_bin_path
        )
    );

    CPPUNIT_ASSERT_NO_THROW(
        io::http::downloadFile(
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

void DownloadTest::downloadTestFailure()
{
    core::os::temp_dir tmpDir;
    const auto& test_bin_path = tmpDir / "test.bin";

    CPPUNIT_ASSERT_THROW(
        io::http::downloadFile(
            "https://whateverTheCloud.com/fileDoesNotExists.txt/download",
            test_bin_path
        ),
        sight::core::exception
    );

    CPPUNIT_ASSERT(!fs::exists(test_bin_path));
}

//------------------------------------------------------------------------------

void DownloadTest::downloadTestWrongInputs()
{
    core::os::temp_dir tmpDir;
    const auto& test2_bin_path = tmpDir / "test2.bin";

    CPPUNIT_ASSERT_THROW(io::http::downloadFile("", test2_bin_path), sight::core::exception);

    CPPUNIT_ASSERT_THROW(
        io::http::downloadFile("https://whateverTheCloud.com/fileDoesNotExists.txt/download", ""),
        sight::core::exception
    );
}

//------------------------------------------------------------------------------

void DownloadTest::downloadTestOverwritesFile()
{
    core::os::temp_dir tmpDir;
    const auto& sample_bin_path = tmpDir / "sample.bin";

    CPPUNIT_ASSERT_NO_THROW(
        io::http::downloadFile(
            "https://cloud.ircad.fr/s/tqYHyjZ2cgHT4mG/download",
            sample_bin_path
        )
    );

    CPPUNIT_ASSERT(fs::exists(sample_bin_path));

    CPPUNIT_ASSERT_THROW(
        io::http::downloadFile(
            "https://cloud.ircad.fr/s/tqYHyjZ2cgHT4mG/download",
            sample_bin_path
        ),
        sight::core::exception
    );
}

} // namespace sight::io::http::ut
