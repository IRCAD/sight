/************************************************************************
 *
 * Copyright (C) 2024-2025 IRCAD France
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

#include <core/runtime/path.hpp>
#include <core/runtime/profile.hpp>
#include <core/runtime/profile/profile.hpp>
#include <core/runtime/runtime.hpp>

#include <doctest/doctest.h>

TEST_CASE("sight::module::app::profile")
{
    sight::core::runtime::init();

    const std::filesystem::path cwd = sight::core::runtime::working_path();
    const auto profile_file_path    = cwd / "share/sight/app_test_app/profile.xml";

    //load the profiles' project modules
    const auto profile_module_path = profile_file_path.parent_path().parent_path();
    if(std::filesystem::exists(profile_module_path) && std::filesystem::is_directory(profile_module_path))
    {
        sight::core::runtime::add_modules(profile_module_path);
    }
    else
    {
        SIGHT_ERROR("Module path " << profile_module_path << " does not exists or is not a directory.");
    }

    const auto profile = sight::core::runtime::io::profile_reader::create_profile(profile_file_path);
    CHECK_EQ(profile->name(), std::string("app_test_app"));
    CHECK_EQ(profile->version(), std::string(PROFILE_VERSION));
    CHECK_EQ(profile->file_path(), profile_file_path);

    sight::core::runtime::profile::params_container in_params {"-one", "--two"};
    profile->set_params(in_params);

    sight::core::runtime::profile::params_container params = profile->get_params();
    CHECK_EQ(params[0], in_params[0]);
    CHECK_EQ(params[1], in_params[1]);
}
