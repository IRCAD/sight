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

#include "RuntimeTest.hpp"

#include <core/runtime/detail/dl/Posix.hpp>
#include <core/runtime/detail/dl/Win32.hpp>
#include <core/runtime/detail/ExtensionPoint.hpp>
#include <core/runtime/detail/Module.hpp>
#include <core/runtime/detail/Runtime.hpp>
#include <core/runtime/Extension.hpp>
#include <core/runtime/Module.hpp>
#include <core/runtime/operations.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( sight::core::runtime::detail::ut::RuntimeTest );

namespace sight::core::runtime
{
namespace detail
{
namespace ut
{

//------------------------------------------------------------------------------

RuntimeTest::RuntimeTest()
{
}

//------------------------------------------------------------------------------

void RuntimeTest::setUp()
{
}

//------------------------------------------------------------------------------

void RuntimeTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

#if defined(linux) || defined(__linux) || defined(__APPLE__)

void RuntimeTest::testPosix()
{
    const auto location = core::runtime::Runtime::getDefault()->getWorkingPath() / MODULE_RC_PREFIX;
    auto module         = std::make_shared<Module>(location / "module_utest-0.1", "module_utest", "0.1");

    auto nativeLibrary = std::make_unique<dl::Posix>("module_utest");
    nativeLibrary->setSearchPath(module->getLibraryLocation());
    const auto name = nativeLibrary->getName();

    CPPUNIT_ASSERT_EQUAL( std::string("module_utest"), name);

    const auto path = nativeLibrary->getFullPath();
    CPPUNIT_ASSERT_EQUAL(  (core::runtime::Runtime::getDefault()->getWorkingPath() / MODULE_LIB_PREFIX /
                            std::filesystem::path("libsight_module_utest.so")).string(), path.string() );
}

#elif defined(WIN32)

//------------------------------------------------------------------------------

void RuntimeTest::testWin32()
{
    const auto location = core::runtime::Runtime::getDefault()->getWorkingPath() / MODULE_RC_PREFIX;
    auto module         = std::make_shared<Module>(location / "utest-0.1", "utest", "0.1");

    auto nativeLibrary = std::make_unique<dl::Win32>("utest");
    nativeLibrary->setSearchPath(module->getLibraryLocation());
    auto nativeName = nativeLibrary->getNativeName();

    CPPUNIT_ASSERT( std::regex_match("sight_module_utest.dll", nativeName));
    CPPUNIT_ASSERT(!std::regex_match("libsight_module_utest.so", nativeName));
    CPPUNIT_ASSERT(!std::regex_match("libsight_module_utest", nativeName));
    CPPUNIT_ASSERT(!std::regex_match("sight_module_utest", nativeName));
    CPPUNIT_ASSERT(!std::regex_match("libfoo.so", nativeName));

    auto path = nativeLibrary->getPath();
    CPPUNIT_ASSERT_EQUAL( std::filesystem::path("utest.dll"), path );
}
#endif

//------------------------------------------------------------------------------

void RuntimeTest::testRuntime()
{
    // Initialize the runtime
    core::runtime::init();

    core::runtime::detail::Runtime& runtime = core::runtime::detail::Runtime::get();
    const auto moduleStr                    = "::sight::modules::utest";
    // Test module utest
    CPPUNIT_ASSERT(runtime.findModule(moduleStr));
    auto bundle = std::dynamic_pointer_cast< core::runtime::detail::Module >(runtime.findModule(moduleStr));
    bundle->setEnable(true);
    CPPUNIT_ASSERT(bundle->isEnabled());

    // Test module utest
    CPPUNIT_ASSERT(runtime.findModule(moduleStr));
    auto bundle2 = std::dynamic_pointer_cast< core::runtime::detail::Module >(runtime.findModule(moduleStr));
    bundle2->setEnable(true);
    CPPUNIT_ASSERT(bundle2->isEnabled());
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace detail
} // namespace fwRuntime
