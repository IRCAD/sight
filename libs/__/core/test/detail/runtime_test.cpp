/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "runtime_test.hpp"

#include <core/runtime/detail/dl/posix.hpp>
#include <core/runtime/detail/dl/win32.hpp>
#include <core/runtime/detail/module.hpp>
#include <core/runtime/detail/runtime.hpp>
#include <core/runtime/runtime.hpp>

#include <filesystem>
#include <regex>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::runtime::detail::ut::runtime_test);

namespace sight::core::runtime::detail::ut
{

//------------------------------------------------------------------------------

runtime_test::runtime_test()
= default;

//------------------------------------------------------------------------------

void runtime_test::setUp()
{
}

//------------------------------------------------------------------------------

void runtime_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

#if defined(__unix__)
void runtime_test::test_posix()
{
    const auto location = core::runtime::detail::runtime::get().working_path() / MODULE_RC_PREFIX;
    auto module         = std::make_shared<core::runtime::detail::module>(
        location / "module_utest-0.1",
        "module_utest",
        "0.1"
    );

    auto native_library = std::make_unique<dl::posix>("sight_module_utest");
    native_library->set_search_path(module->get_library_location());
    const auto name = native_library->name();

    CPPUNIT_ASSERT_EQUAL(std::string("sight_module_utest"), name);

    const auto path = native_library->get_full_path();
    CPPUNIT_ASSERT_EQUAL(
        (core::runtime::detail::runtime::get().working_path() / MODULE_LIB_PREFIX
         / std::filesystem::path("libsight_module_utest.so")).string(),
        path.string()
    );
}
#elif defined(WIN32)
//------------------------------------------------------------------------------

void runtime_test::test_win32()
{
    const auto location = core::runtime::detail::runtime::get().working_path() / MODULE_RC_PREFIX;
    const auto module   = std::make_shared<detail::module>(location / "utest-0.1", "utest", "0.1");

    auto native_library = std::make_unique<dl::win32>("sight_module_utest");
    native_library->set_search_path(module->get_library_location());
    auto native_name = native_library->name();

    CPPUNIT_ASSERT_EQUAL(std::string("sight_module_utest"), native_name);

    auto path = native_library->get_full_path();
    CPPUNIT_ASSERT_EQUAL(
        (core::runtime::detail::runtime::get().working_path() / MODULE_LIB_PREFIX
         / std::filesystem::path("sight_module_utest.dll")).string(),
        path.string()
    );
}
#endif

//------------------------------------------------------------------------------

void runtime_test::test_runtime()
{
    // Initialize the runtime
    core::runtime::init();

    const core::runtime::detail::runtime& runtime = core::runtime::detail::runtime::get();
    const auto* const module_str                  = "sight::module::utest";
    // Test module utest
    CPPUNIT_ASSERT(runtime.find_module(module_str));
    auto bundle = std::dynamic_pointer_cast<core::runtime::detail::module>(runtime.find_module(module_str));
    bundle->set_enable(true);
    CPPUNIT_ASSERT(bundle->enabled());

    // Test module utest
    CPPUNIT_ASSERT(runtime.find_module(module_str));
    auto bundle2 = std::dynamic_pointer_cast<core::runtime::detail::module>(runtime.find_module(module_str));
    bundle2->set_enable(true);
    CPPUNIT_ASSERT(bundle2->enabled());
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime::detail::ut
