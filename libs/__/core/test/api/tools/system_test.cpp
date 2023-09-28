/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "system_test.hpp"

#include <core/os/temp_path.hpp>
#include <core/tools/system.hpp>

#include <fstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::tools::ut::system_test);

namespace sight::core::tools::ut
{

//------------------------------------------------------------------------------

void system_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void system_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void system_test::robust_rename_test()
{
    core::os::temp_dir tmp_dir;

    const auto origin_file = tmp_dir / "test.sight";

    // Create fake file.
    {
        std::fstream fs;
        fs.open(origin_file, std::ios::out);
        fs.close();
    }

    const auto destination_file = tmp_dir / "test1000.sight";

    //1. Basic renaming.
    CPPUNIT_ASSERT_NO_THROW(core::tools::system::robust_rename(origin_file, destination_file));

    CPPUNIT_ASSERT_MESSAGE("Destination file should exist.", std::filesystem::exists(destination_file));
    CPPUNIT_ASSERT_MESSAGE("Origin file shouldn't exist", !std::filesystem::exists(origin_file));

    // 2. Should throw an exception.
    CPPUNIT_ASSERT_THROW(
        core::tools::system::robust_rename(origin_file, destination_file),
        std::filesystem::filesystem_error
    );

    // 3. Should NOT throw an exception, even if the original file already exists.
    // (re)create the fake file.
    {
        std::fstream fs;
        fs.open(origin_file, std::ios::out);
        fs.close();
    }

    CPPUNIT_ASSERT_NO_THROW(core::tools::system::robust_rename(destination_file, origin_file, true));
    CPPUNIT_ASSERT_NO_THROW(core::tools::system::robust_rename(origin_file, destination_file, true));
    CPPUNIT_ASSERT_MESSAGE("Destination file should exist.", std::filesystem::exists(destination_file));
    CPPUNIT_ASSERT_MESSAGE("Origin file shouldn't exist", !std::filesystem::exists(origin_file));

    // 4. Should do nothing if both path are indeed the same file
    CPPUNIT_ASSERT_NO_THROW(core::tools::system::robust_rename(destination_file, destination_file));

    // Clean up.
    std::filesystem::remove(destination_file);
}

//------------------------------------------------------------------------------

} // namespace sight::core::tools::ut
