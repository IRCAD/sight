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

#include "os_test.hpp"

#include <core/base.hpp>
#include <core/tools/os.hpp>
#include <core/tools/uuid.hpp>

#include <boost/dll.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::tools::ut::os);

namespace sight::core::tools::ut
{

//------------------------------------------------------------------------------

void os::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void os::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void os::get_shared_library_path()
{
    namespace fs = std::filesystem;
    const auto cwd = fs::current_path();

    {
        const auto exec_path = boost::dll::program_location().remove_filename();

        // cspell: disable
#if defined(WIN32)
        const auto actual_path       = core::tools::os::get_shared_library_path("sight_core.dll");
        const fs::path expected_path = fs::path(exec_path.string()) / "sight_core.dll";
#else
        const auto actual_path =
            core::tools::os::get_shared_library_path("sight_core").replace_extension().replace_extension();
        const fs::path expected_path = fs::path(exec_path.parent_path().string()) / MODULE_LIB_PREFIX
                                       / "libsight_core.so";
#endif
        CPPUNIT_ASSERT_EQUAL(expected_path, actual_path);
    }

    // Test that a call with a not loaded library throws an error
    CPPUNIT_ASSERT_THROW(core::tools::os::get_shared_library_path("foo"), core::exception);

    // Test that a call with a not loaded library throws an error
    CPPUNIT_ASSERT_THROW(core::tools::os::get_shared_library_path("Qt5Core"), core::exception);

    // Now load that library and check that we find it
#if defined(WIN32)
    const auto lib      = "zstd";
    const auto lib_path = fs::weakly_canonical(fs::path(BOOST_LIB_DIR) / "zstd.dll");
#else
    const auto* const lib = "boost_date_time";
    const auto lib_path   = fs::path(BOOST_LIB_DIR) / "libboost_date_time.so.1.74.0";
#endif
    auto handle = boost::dll::shared_library(lib_path.string());
    CPPUNIT_ASSERT_MESSAGE("Could not load library for testing", handle);

    CPPUNIT_ASSERT_EQUAL(lib_path, core::tools::os::get_shared_library_path(lib));
    // cspell: enable
}

//------------------------------------------------------------------------------

inline static std::filesystem::path get_user_dir(const std::string& variable)
{
    if(const char* const value = std::getenv(variable.c_str()); value != nullptr)
    {
        return {value};
    }

#ifndef WIN32
    if(const char* const home_value = std::getenv("HOME"); home_value != nullptr)
    {
        const std::filesystem::path home(home_value);

        if(variable == "XDG_CONFIG_DIR")
        {
            return home / ".config";
        }

        if(variable == "XDG_DATA_DIR")
        {
            return home / ".local" / "share";
        }

        if(variable == "XDG_CACHE_DIR")
        {
            return home / ".cache";
        }
    }
#endif

    return {};
}

class path_cleaner final
{
public:

    explicit path_cleaner(std::filesystem::path to_be_cleaned) :
        m_to_be_cleaned(std::move(to_be_cleaned))
    {
    }

    ~path_cleaner()
    {
        if(std::filesystem::exists(m_to_be_cleaned))
        {
            std::filesystem::remove_all(m_to_be_cleaned);
        }
    }

    std::filesystem::path m_to_be_cleaned {};
};

//------------------------------------------------------------------------------

void os::get_user_config_dir_test()
{
#ifdef WIN32
    const std::string variable("APPDATA");
#else
    const std::string variable("XDG_CONFIG_DIR");
#endif

    // Default (Do not "clean" default paths... )
    {
        const std::filesystem::path company_name = "sight";
        const auto& expected_config_dir          = get_user_dir(variable) / company_name;
        const auto& config_dir                   = core::tools::os::get_user_config_dir();

        CPPUNIT_ASSERT_EQUAL(expected_config_dir.string(), config_dir.string());
        CPPUNIT_ASSERT(std::filesystem::is_directory(config_dir));
    }

    // With an application name
    {
        const std::filesystem::path application_name = core::tools::UUID::generate();
        const std::filesystem::path company_name     = "sight";
        const auto& suffix                           = company_name / application_name;
        const auto& expected_config_dir              = get_user_dir(variable) / suffix;
        const auto& config_dir                       = core::tools::os::get_user_config_dir(application_name.string());

        // Prepare cleanup with an RAII
        path_cleaner config_dir_cleaner(expected_config_dir);

        CPPUNIT_ASSERT_EQUAL(expected_config_dir.string(), config_dir.string());
        CPPUNIT_ASSERT(std::filesystem::exists(config_dir) && std::filesystem::is_directory(config_dir));
    }

    // With a Company, an application name, without creating dir
    {
        const std::filesystem::path application_name = core::tools::UUID::generate();
        const std::filesystem::path company_name     = core::tools::UUID::generate();
        const auto& suffix                           = company_name / application_name;
        const auto& expected_config_dir              = get_user_dir(variable) / suffix;
        const auto& config_dir                       = core::tools::os::get_user_config_dir(
            application_name.string(),
            false,
            company_name.string()
        );

        CPPUNIT_ASSERT_EQUAL(expected_config_dir.string(), config_dir.string());
        CPPUNIT_ASSERT(!std::filesystem::exists(config_dir) && !std::filesystem::is_directory(config_dir));
    }
}

//------------------------------------------------------------------------------

void os::get_user_data_dir_test()
{
#ifdef WIN32
    const std::string variable("APPDATA");
#else
    const std::string variable("XDG_DATA_DIR");
#endif

    // Default (Do not "clean" default paths... )
    {
        const std::filesystem::path company_name = "sight";
        const auto& expected_data_dir            = get_user_dir(variable) / company_name;
        const auto& data_dir                     = core::tools::os::get_user_data_dir();

        CPPUNIT_ASSERT_EQUAL(expected_data_dir.string(), data_dir.string());
        CPPUNIT_ASSERT(std::filesystem::is_directory(data_dir));
    }

    // With an application name
    {
        const std::filesystem::path application_name = core::tools::UUID::generate();
        const std::filesystem::path company_name     = "sight";
        const auto& suffix                           = company_name / application_name;
        const auto& expected_data_dir                = get_user_dir(variable) / suffix;
        const auto& data_dir                         = core::tools::os::get_user_data_dir(application_name.string());

        // Prepare cleanup with an RAII
        path_cleaner data_dir_cleaner(expected_data_dir);

        CPPUNIT_ASSERT_EQUAL(expected_data_dir.string(), data_dir.string());
        CPPUNIT_ASSERT(std::filesystem::exists(data_dir) && std::filesystem::is_directory(data_dir));
    }

    // With a Company, an application name, without creating dir
    {
        const std::filesystem::path application_name = core::tools::UUID::generate();
        const std::filesystem::path company_name     = core::tools::UUID::generate();
        const auto& suffix                           = company_name / application_name;
        const auto& expected_data_dir                = get_user_dir(variable) / suffix;
        const auto& data_dir                         = core::tools::os::get_user_data_dir(
            application_name.string(),
            false,
            company_name.string()
        );

        CPPUNIT_ASSERT_EQUAL(expected_data_dir.string(), data_dir.string());
        CPPUNIT_ASSERT(!std::filesystem::exists(data_dir) && !std::filesystem::is_directory(data_dir));
    }
}

//------------------------------------------------------------------------------

void os::get_user_cache_dir_test()
{
#ifdef WIN32
    const std::string variable("APPDATA");
#else
    const std::string variable("XDG_CACHE_DIR");
#endif

    // Default (Do not "clean" default paths... )
    {
        const std::filesystem::path company_name = "sight";
        const auto& expected_cache_dir           = get_user_dir(variable) / company_name;
        const auto& cache_dir                    = core::tools::os::get_user_cache_dir();

        CPPUNIT_ASSERT_EQUAL(expected_cache_dir.string(), cache_dir.string());
        CPPUNIT_ASSERT(std::filesystem::is_directory(cache_dir));
    }

    // With an application name
    {
        const std::filesystem::path application_name = core::tools::UUID::generate();
        const std::filesystem::path company_name     = "sight";
        const auto& suffix                           = company_name / application_name;
        const auto& expected_cache_dir               = get_user_dir(variable) / suffix;
        const auto& cache_dir                        = core::tools::os::get_user_cache_dir(application_name.string());

        // Prepare cleanup with an RAII
        path_cleaner cache_dir_cleaner(expected_cache_dir);

        CPPUNIT_ASSERT_EQUAL(expected_cache_dir.string(), cache_dir.string());
        CPPUNIT_ASSERT(std::filesystem::exists(cache_dir) && std::filesystem::is_directory(cache_dir));
    }

    // With a Company, an application name, without creating dir
    {
        const std::filesystem::path application_name = core::tools::UUID::generate();
        const std::filesystem::path company_name     = core::tools::UUID::generate();
        const auto& suffix                           = company_name / application_name;
        const auto& expected_cache_dir               = get_user_dir(variable) / suffix;
        const auto& cache_dir                        = core::tools::os::get_user_cache_dir(
            application_name.string(),
            false,
            company_name.string()
        );

        CPPUNIT_ASSERT_EQUAL(expected_cache_dir.string(), cache_dir.string());
        CPPUNIT_ASSERT(!std::filesystem::exists(cache_dir) && !std::filesystem::is_directory(cache_dir));
    }
}

} // namespace sight::core::tools::ut
