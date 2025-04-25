/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include "profile_test.hpp"

#include <core/runtime/path.hpp>
#include <core/runtime/profile.hpp>
#include <core/runtime/profile/profile.hpp>
#include <core/runtime/runtime.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::app::ut::profile_test);

namespace sight::module::app::ut
{

//------------------------------------------------------------------------------

void profile_test::setUp()
{
}

//------------------------------------------------------------------------------

void profile_test::tearDown()
{
}

//------------------------------------------------------------------------------

void profile_test::create_profile_test()
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
    CPPUNIT_ASSERT_EQUAL(profile->name(), std::string("app_test_app"));
    CPPUNIT_ASSERT_EQUAL(profile->version(), std::string(PROFILE_VERSION));
    CPPUNIT_ASSERT_EQUAL(profile->file_path(), profile_file_path);

    sight::core::runtime::profile::params_container in_params {"-one", "--two"};
    profile->set_params(in_params);

    sight::core::runtime::profile::params_container params = profile->get_params();
    CPPUNIT_ASSERT_EQUAL(params[0], in_params[0]);
    CPPUNIT_ASSERT_EQUAL(params[1], in_params[1]);
}

//------------------------------------------------------------------------------

} // namespace sight::module::app::ut
