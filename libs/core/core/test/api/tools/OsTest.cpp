/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "OsTest.hpp"

#include <core/base.hpp>
#include <core/tools/Os.hpp>

#include <boost/dll.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::tools::ut::Os);

namespace sight::core::tools
{

namespace ut
{

//------------------------------------------------------------------------------

void Os::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void Os::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void Os::getSharedLibraryPath()
{
    namespace fs = std::filesystem;
    const auto cwd = fs::current_path();

    {
        const auto execPath = ::boost::dll::program_location().remove_filename();

#if defined(WIN32)
        const auto actualPath       = core::tools::os::getSharedLibraryPath("sight_core.dll");
        const fs::path expectedPath = fs::path(execPath.string()) / "sight_core.dll";
#else
        const auto actualPath =
            core::tools::os::getSharedLibraryPath("sight_core").replace_extension().replace_extension();
        const fs::path expectedPath = fs::path(execPath.parent_path().string()) / MODULE_LIB_PREFIX
                                      / "libsight_core.so";
#endif
        CPPUNIT_ASSERT_EQUAL(expectedPath, actualPath);
    }

    // Test that a call with a not loaded library throws an error
    CPPUNIT_ASSERT_THROW(core::tools::os::getSharedLibraryPath("foo"), core::tools::Exception);

    // Test that a call with a not loaded library throws an error
    CPPUNIT_ASSERT_THROW(core::tools::os::getSharedLibraryPath("Qt5Core"), core::tools::Exception);

    // Now load that library and check that we find it
#if defined(WIN32)
    #if defined(_DEBUG)
    const auto campPath = fs::weakly_canonical(fs::path(CAMP_LIB_DIR) / "campd.dll");
    #else
    const auto campPath = fs::weakly_canonical(fs::path(CAMP_LIB_DIR) / "camp.dll");
    #endif
#else
    const auto campPath = fs::path(CAMP_LIB_DIR) / "libcamp.so.0.8";
#endif
    auto handle = ::boost::dll::shared_library(campPath.string());
    CPPUNIT_ASSERT_MESSAGE("Could not load camp for testing", handle);

    CPPUNIT_ASSERT_EQUAL(campPath, core::tools::os::getSharedLibraryPath("camp"));
}

} // namespace ut

} // namespace sight::core::tools
