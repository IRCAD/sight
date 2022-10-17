/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include <core/tools/UUID.hpp>

#include <boost/dll.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::tools::ut::Os);

namespace sight::core::tools::ut
{

static const std::string default_company("sight");

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
        const auto execPath = boost::dll::program_location().remove_filename();

        // cspell: disable
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
    CPPUNIT_ASSERT_THROW(core::tools::os::getSharedLibraryPath("foo"), core::Exception);

    // Test that a call with a not loaded library throws an error
    CPPUNIT_ASSERT_THROW(core::tools::os::getSharedLibraryPath("Qt5Core"), core::Exception);

    // Now load that library and check that we find it
#if defined(WIN32)
    const auto lib     = "zstd";
    const auto libPath = fs::weakly_canonical(fs::path(BOOST_LIB_DIR) / "zstd.dll");
#else
    const auto* const lib = "boost_date_time";
    const auto libPath    = fs::path(BOOST_LIB_DIR) / "libboost_date_time.so.1.74.0";
#endif
    auto handle = boost::dll::shared_library(libPath.string());
    CPPUNIT_ASSERT_MESSAGE("Could not load library for testing", handle);

    CPPUNIT_ASSERT_EQUAL(libPath, core::tools::os::getSharedLibraryPath(lib));
    // cspell: enable
}

//------------------------------------------------------------------------------

inline static std::filesystem::path getUserDir(const std::string& variable)
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

class PathCleaner final
{
public:

    explicit PathCleaner(std::filesystem::path toBeCleaned) :
        m_toBeCleaned(std::move(toBeCleaned))
    {
    }

    ~PathCleaner()
    {
        if(std::filesystem::exists(m_toBeCleaned))
        {
            std::filesystem::remove_all(m_toBeCleaned);
        }
    }

    std::filesystem::path m_toBeCleaned {};
};

//------------------------------------------------------------------------------

void Os::getUserConfigDirTest()
{
#ifdef WIN32
    const std::string variable("APPDATA");
#else
    const std::string variable("XDG_CONFIG_DIR");
#endif

    // Default (Do not "clean" default paths... )
    {
        const std::filesystem::path company_name = "sight";
        const auto& expected_config_dir          = getUserDir(variable) / company_name;
        const auto& config_dir                   = core::tools::os::getUserConfigDir();

        CPPUNIT_ASSERT_EQUAL(expected_config_dir.string(), config_dir.string());
        CPPUNIT_ASSERT(std::filesystem::is_directory(config_dir));
    }

    // With an application name
    {
        const std::filesystem::path application_name = core::tools::UUID::generateUUID();
        const std::filesystem::path company_name     = "sight";
        const auto& suffix                           = company_name / application_name;
        const auto& expected_config_dir              = getUserDir(variable) / suffix;
        const auto& config_dir                       = core::tools::os::getUserConfigDir(application_name.string());

        // Prepare cleanup with an RAII
        PathCleaner config_dir_cleaner(expected_config_dir);

        CPPUNIT_ASSERT_EQUAL(expected_config_dir.string(), config_dir.string());
        CPPUNIT_ASSERT(std::filesystem::exists(config_dir) && std::filesystem::is_directory(config_dir));
    }

    // With a Company, an application name, without creating dir
    {
        const std::filesystem::path application_name = core::tools::UUID::generateUUID();
        const std::filesystem::path company_name     = core::tools::UUID::generateUUID();
        const auto& suffix                           = company_name / application_name;
        const auto& expected_config_dir              = getUserDir(variable) / suffix;
        const auto& config_dir                       = core::tools::os::getUserConfigDir(
            application_name.string(),
            false,
            company_name.string()
        );

        CPPUNIT_ASSERT_EQUAL(expected_config_dir.string(), config_dir.string());
        CPPUNIT_ASSERT(!std::filesystem::exists(config_dir) && !std::filesystem::is_directory(config_dir));
    }
}

//------------------------------------------------------------------------------

void Os::getUserDataDirTest()
{
#ifdef WIN32
    const std::string variable("APPDATA");
#else
    const std::string variable("XDG_DATA_DIR");
#endif

    // Default (Do not "clean" default paths... )
    {
        const std::filesystem::path company_name = "sight";
        const auto& expected_data_dir            = getUserDir(variable) / company_name;
        const auto& data_dir                     = core::tools::os::getUserDataDir();

        CPPUNIT_ASSERT_EQUAL(expected_data_dir.string(), data_dir.string());
        CPPUNIT_ASSERT(std::filesystem::is_directory(data_dir));
    }

    // With an application name
    {
        const std::filesystem::path application_name = core::tools::UUID::generateUUID();
        const std::filesystem::path company_name     = "sight";
        const auto& suffix                           = company_name / application_name;
        const auto& expected_data_dir                = getUserDir(variable) / suffix;
        const auto& data_dir                         = core::tools::os::getUserDataDir(application_name.string());

        // Prepare cleanup with an RAII
        PathCleaner data_dir_cleaner(expected_data_dir);

        CPPUNIT_ASSERT_EQUAL(expected_data_dir.string(), data_dir.string());
        CPPUNIT_ASSERT(std::filesystem::exists(data_dir) && std::filesystem::is_directory(data_dir));
    }

    // With a Company, an application name, without creating dir
    {
        const std::filesystem::path application_name = core::tools::UUID::generateUUID();
        const std::filesystem::path company_name     = core::tools::UUID::generateUUID();
        const auto& suffix                           = company_name / application_name;
        const auto& expected_data_dir                = getUserDir(variable) / suffix;
        const auto& data_dir                         = core::tools::os::getUserDataDir(
            application_name.string(),
            false,
            company_name.string()
        );

        CPPUNIT_ASSERT_EQUAL(expected_data_dir.string(), data_dir.string());
        CPPUNIT_ASSERT(!std::filesystem::exists(data_dir) && !std::filesystem::is_directory(data_dir));
    }
}

//------------------------------------------------------------------------------

void Os::getUserCacheDirTest()
{
#ifdef WIN32
    const std::string variable("APPDATA");
#else
    const std::string variable("XDG_CACHE_DIR");
#endif

    // Default (Do not "clean" default paths... )
    {
        const std::filesystem::path company_name = "sight";
        const auto& expected_cache_dir           = getUserDir(variable) / company_name;
        const auto& cache_dir                    = core::tools::os::getUserCacheDir();

        CPPUNIT_ASSERT_EQUAL(expected_cache_dir.string(), cache_dir.string());
        CPPUNIT_ASSERT(std::filesystem::is_directory(cache_dir));
    }

    // With an application name
    {
        const std::filesystem::path application_name = core::tools::UUID::generateUUID();
        const std::filesystem::path company_name     = "sight";
        const auto& suffix                           = company_name / application_name;
        const auto& expected_cache_dir               = getUserDir(variable) / suffix;
        const auto& cache_dir                        = core::tools::os::getUserCacheDir(application_name.string());

        // Prepare cleanup with an RAII
        PathCleaner cache_dir_cleaner(expected_cache_dir);

        CPPUNIT_ASSERT_EQUAL(expected_cache_dir.string(), cache_dir.string());
        CPPUNIT_ASSERT(std::filesystem::exists(cache_dir) && std::filesystem::is_directory(cache_dir));
    }

    // With a Company, an application name, without creating dir
    {
        const std::filesystem::path application_name = core::tools::UUID::generateUUID();
        const std::filesystem::path company_name     = core::tools::UUID::generateUUID();
        const auto& suffix                           = company_name / application_name;
        const auto& expected_cache_dir               = getUserDir(variable) / suffix;
        const auto& cache_dir                        = core::tools::os::getUserCacheDir(
            application_name.string(),
            false,
            company_name.string()
        );

        CPPUNIT_ASSERT_EQUAL(expected_cache_dir.string(), cache_dir.string());
        CPPUNIT_ASSERT(!std::filesystem::exists(cache_dir) && !std::filesystem::is_directory(cache_dir));
    }
}

} // namespace sight::core::tools::ut
