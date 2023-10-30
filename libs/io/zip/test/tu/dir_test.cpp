/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "dir_test.hpp"

#include <core/os/temp_path.hpp>

#include <io/zip/read_dir_archive.hpp>
#include <io/zip/write_dir_archive.hpp>

#include <utest_data/Data.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::zip::ut::dir_test);

namespace sight::io::zip::ut
{

//------------------------------------------------------------------------------

void dir_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void dir_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void dir_test::write_read_file_test()
{
    core::os::temp_dir tmp_dir;

    auto writer = std::make_shared<write_dir_archive>(tmp_dir);
    auto reader = std::make_shared<read_dir_archive>(tmp_dir);

    const std::filesystem::path test_file = "test.txt";

    CPPUNIT_ASSERT_NO_THROW(writer->create_file(test_file));

    CPPUNIT_ASSERT_NO_THROW(reader->get_file(test_file));
}

//------------------------------------------------------------------------------

void dir_test::write_dir_test()
{
    core::os::temp_dir tmp_dir;

    auto writer = std::make_shared<write_dir_archive>(tmp_dir);

    const std::filesystem::path test_dir = "test";

    CPPUNIT_ASSERT_NO_THROW(writer->create_dir(test_dir));

    CPPUNIT_ASSERT(std::filesystem::exists(tmp_dir / test_dir));
}

//------------------------------------------------------------------------------

void dir_test::put_file_test()
{
    core::os::temp_dir tmp_dir;

    auto writer = std::make_shared<write_dir_archive>(tmp_dir);

    const std::filesystem::path test_dir = "test";
    //cspell: ignore makao
    const std::filesystem::path test_file = utest_data::dir() / "sight/image/jpg/makao01.jpg";
    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + test_file.string() + "' does not exist",
        std::filesystem::exists(test_file)
    );

    CPPUNIT_ASSERT_NO_THROW(writer->put_file(test_file, "image.jpg"));

    CPPUNIT_ASSERT(std::filesystem::exists(tmp_dir / "image.jpg"));
}

} // namespace sight::io::zip::ut
