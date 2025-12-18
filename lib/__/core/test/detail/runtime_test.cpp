/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include <core/runtime/detail/dl/posix.hpp>
#include <core/runtime/detail/dl/win32.hpp>
#include <core/runtime/detail/module.hpp>
#include <core/runtime/detail/runtime.hpp>
#include <core/runtime/runtime.hpp>

#include <doctest/doctest.h>

#include <filesystem>
#include <regex>

//------------------------------------------------------------------------------

TEST_SUITE("sight::core::runtime::detail")
{
#if defined(__unix__)
    TEST_CASE("posix")
    {
        const auto location = sight::core::runtime::detail::runtime::get().working_path() / MODULE_RC_PREFIX;
        auto module         = std::make_shared<sight::core::runtime::detail::module>(
            location / "module_utest-0.1",
            "module_utest",
            "0.1"
        );

        auto native_library = std::make_unique<sight::core::runtime::detail::dl::posix>("sight_module_utest");
        native_library->set_search_path(module->get_library_location());
        const auto name = native_library->name();

        CHECK_EQ(std::string("sight_module_utest"), name);

        const auto path = native_library->get_full_path();
        CHECK_EQ(
            (sight::core::runtime::detail::runtime::get().working_path() / MODULE_LIB_PREFIX
             / std::filesystem::path("libsight_module_utest.so")).string(),
            path.string()
        );
    }
#elif defined(WIN32)
//------------------------------------------------------------------------------

    TEST_CASE("win32")
    {
        const auto location = sight::core::runtime::detail::runtime::get().working_path() / MODULE_RC_PREFIX;
        const auto module   = std::make_shared<sight::core::runtime::detail::module>(
            location / "utest-0.1",
            "utest",
            "0.1"
        );

        auto native_library = std::make_unique<sight::core::runtime::detail::dl::win32>("sight_module_utest");
        native_library->set_search_path(module->get_library_location());
        auto native_name = native_library->name();

        CHECK_EQ(std::string("sight_module_utest"), native_name);

        auto path = native_library->get_full_path();
        CHECK_EQ(
            (sight::core::runtime::detail::runtime::get().working_path() / MODULE_LIB_PREFIX
             / std::filesystem::path("sight_module_utest.dll")).string(),
            path.string()
        );
    }
#endif

//------------------------------------------------------------------------------

    TEST_CASE("runtime")
    {
        // Initialize the runtime
        sight::core::runtime::init();

        const auto& runtime          = sight::core::runtime::detail::runtime::get();
        const auto* const module_str = "sight::module::utest";
        // Test module utest
        CHECK(runtime.find_module(module_str));
        auto bundle = std::dynamic_pointer_cast<sight::core::runtime::detail::module>(runtime.find_module(module_str));
        bundle->set_enable(true);
        CHECK(bundle->enabled());

        // Test module utest
        CHECK(runtime.find_module(module_str));
        auto bundle2 = std::dynamic_pointer_cast<sight::core::runtime::detail::module>(runtime.find_module(module_str));
        bundle2->set_enable(true);
        CHECK(bundle2->enabled());
    }
}
