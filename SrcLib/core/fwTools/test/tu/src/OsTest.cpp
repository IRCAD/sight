/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include <fwTools/Os.hpp>

#include <fwCore/base.hpp>

#include <boost/filesystem.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwTools::ut::Os );

namespace fwTools
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

void Os::getExecutablePath()
{
    namespace fs = ::boost::filesystem;
    const auto cwd = fs::current_path();

    const auto execPath = ::fwTools::os::getExecutablePath();

    // TODO: Annoying to test....
    //CPPUNIT_ASSERT_EQUAL(cwd, execPath);
}

//------------------------------------------------------------------------------

void Os::getSharedLibraryPath()
{
    namespace fs = ::boost::filesystem;
    const auto cwd = fs::current_path();

    const auto fwCorePath = ::fwTools::os::getSharedLibraryPath("fwCore");

    const auto execPath = ::fwTools::os::getExecutablePath().remove_filename();

#if defined(WIN32)
    const fs::path expectedPath = execPath / "fwCore.dll";
#elif defined(APPLE)
    const fs::path expectedPath = execPath.parent_path() / "lib" / "libfwCore.dylib";
#else
    const fs::path expectedPath = execPath.parent_path() / "lib" / "libfwCore.so";
#endif

    CPPUNIT_ASSERT_EQUAL(expectedPath, fwCorePath);

    // Test that a call with a not loaded library throws an error
    CPPUNIT_ASSERT_THROW( ::fwTools::os::getSharedLibraryPath("fwData"), ::fwTools::Exception );
}

} // namespace ut
} // namespace fwTools
