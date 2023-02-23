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

#include "SystemTest.hpp"

#include <core/os/TempPath.hpp>
#include <core/tools/System.hpp>

#include <fstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::tools::ut::SystemTest);

namespace sight::core::tools::ut
{

//------------------------------------------------------------------------------

void SystemTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void SystemTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void SystemTest::robustRenameTest()
{
    core::os::TempDir tmpDir;

    const auto originFile = tmpDir / "test.sight";

    // Create fake file.
    {
        std::fstream fs;
        fs.open(originFile, std::ios::out);
        fs.close();
    }

    const auto destinationFile = tmpDir / "test1000.sight";

    //1. Basic renaming.
    CPPUNIT_ASSERT_NO_THROW(core::tools::System::robustRename(originFile, destinationFile));

    CPPUNIT_ASSERT_MESSAGE("Destination file should exist.", std::filesystem::exists(destinationFile));
    CPPUNIT_ASSERT_MESSAGE("Origin file shouldn't exist", !std::filesystem::exists(originFile));

    // 2. Should throw an exception.
    CPPUNIT_ASSERT_THROW(
        core::tools::System::robustRename(originFile, destinationFile),
        std::filesystem::filesystem_error
    );

    // 3. Should NOT throw an exception, even if the original file already exists.
    // (re)create the fake file.
    {
        std::fstream fs;
        fs.open(originFile, std::ios::out);
        fs.close();
    }

    CPPUNIT_ASSERT_NO_THROW(core::tools::System::robustRename(destinationFile, originFile, true));
    CPPUNIT_ASSERT_NO_THROW(core::tools::System::robustRename(originFile, destinationFile, true));
    CPPUNIT_ASSERT_MESSAGE("Destination file should exist.", std::filesystem::exists(destinationFile));
    CPPUNIT_ASSERT_MESSAGE("Origin file shouldn't exist", !std::filesystem::exists(originFile));

    // 4. Should do nothing if both path are indeed the same file
    CPPUNIT_ASSERT_NO_THROW(core::tools::System::robustRename(destinationFile, destinationFile));

    // Clean up.
    std::filesystem::remove(destinationFile);
}

//------------------------------------------------------------------------------

} // namespace sight::core::tools::ut
