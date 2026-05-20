/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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

#include "service/base.hpp"

#include <core/exception.hpp>
#include <core/os/temp_path.hpp>
#include <core/runtime/profile.hpp>
#include <core/runtime/runtime.hpp>
#include <core/tools/os.hpp>
#include <core/tools/uuid.hpp>

#include <io/__/service/writer.hpp>

#include <ui/__/preferences.hpp>

#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/property_tree/ptree.hpp>

#include <doctest/doctest.h>

#include <filesystem>

// Forward declarations - test writer service class (assumed to be defined elsewhere)
namespace
{

// Defines a Writer class, with a dummy open_location_dialog implementation
class test_writer : public sight::io::service::writer
{
public:

    SIGHT_DECLARE_SERVICE(test_writer, sight::io::service::writer);

    explicit test_writer(sight::io::service::path_type_t _pt) :
        m_path_type(_pt)
    {
    }

    //------------------------------------------------------------------------------

    void set_path_type(sight::io::service::path_type_t _pt)
    {
        m_path_type = _pt;
    }

protected:

    //------------------------------------------------------------------------------

    sight::io::service::path_type_t get_path_type() const override
    {
        return m_path_type;
    }

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }

    //------------------------------------------------------------------------------

    void updating() override
    {
    }

    //------------------------------------------------------------------------------

    void open_location_dialog() override
    {
    }

private:

    sight::io::service::path_type_t m_path_type {sight::io::service::path_type_t::file};
};

struct fixture
{
    fixture() :
        m_root_path(sight::core::os::temp_dir::shared_directory())
    {
        // Set the profile name (needed for the preferences to work)
        sight::core::runtime::init();
        const std::string& profile_name = sight::core::tools::uuid::generate();
        sight::core::runtime::get_current_profile()->set_name(profile_name);

        // Enables the preferences
        sight::ui::preferences::set_enabled(true);
    }

    //------------------------------------------------------------------------------

    ~fixture()
    {
        sight::ui::preferences preferences;

        // Clear the preferences
        sight::ui::preferences::clear();

        // Disables the preferences
        sight::ui::preferences::set_enabled(false);
    }

    std::string m_file {"test.png"};
    std::string m_folder {"test"};
    std::filesystem::path m_root_path;
};

} // namespace

TEST_SUITE("sight::io::writer")
{
    TEST_CASE_FIXTURE(fixture, "test_type_ok")
    {
        // Test using the wrong method for accessing a file
        {
            // Test adding service
            auto srv = std::make_shared<test_writer>(sight::io::service::file);

            sight::service::config_t config;
            config.add("file", (m_root_path / m_file).string());
            srv->set_config(config);
            srv->configure();
            srv->start();

            CHECK_NOTHROW(srv->get_file());
            CHECK_THROWS_AS(srv->get_folder(), sight::core::exception);

            srv->stop();
        }

        // Test using the wrong method for accessing a folder
        {
            // Test adding service
            auto srv = std::make_shared<test_writer>(sight::io::service::folder);

            sight::service::config_t config;
            config.add("folder", m_root_path.string());
            srv->set_config(config);
            srv->configure();
            srv->start();

            CHECK_THROWS_AS(srv->get_file(), sight::core::exception);
            CHECK_NOTHROW(srv->get_folder());

            srv->stop();
        }
    }

    TEST_CASE_FIXTURE(fixture, "test_has_location_defined")
    {
        // No location defined
        {
            auto srv = std::make_shared<test_writer>(sight::io::service::file);

            srv->configure();
            srv->start();

            CHECK(!srv->has_location_defined());

            srv->stop();
        }

        // No absolute location defined
        {
            auto srv = std::make_shared<test_writer>(sight::io::service::file);

            sight::service::config_t config;
            config.add("file", m_file);
            srv->set_config(config);
            srv->configure();
            srv->start();

            CHECK(!srv->has_location_defined());

            srv->stop();
        }

        // Absolute location defined
        {
            auto srv = std::make_shared<test_writer>(sight::io::service::file);
            sight::service::config_t config;
            config.add("file", (m_root_path / m_file).string());
            srv->set_config(config);
            srv->configure();
            srv->start();

            CHECK(srv->has_location_defined());

            srv->stop();
        }
    }

    TEST_CASE_FIXTURE(fixture, "test_has_location_defined_with_base_folder")
    {
        // Base location defined but no file in FILE mode
        {
            auto srv = std::make_shared<test_writer>(sight::io::service::file);

            sight::service::config_t config;
            config.add("baseFolder", m_root_path.string());
            srv->set_config(config);
            srv->configure();
            srv->start();

            CHECK(!srv->has_location_defined());

            srv->stop();
        }

        // Base location defined and a file in FILE mode
        {
            auto srv = std::make_shared<test_writer>(sight::io::service::file);

            sight::service::config_t config;
            config.add("baseFolder", m_root_path.string());
            config.add("file", m_file);
            srv->set_config(config);
            srv->configure();
            srv->start();

            CHECK(srv->has_location_defined());

            srv->stop();
        }

        // Base location defined and in FOLDER mode
        {
            auto srv = std::make_shared<test_writer>(sight::io::service::folder);

            sight::service::config_t config;
            config.add("baseFolder", m_root_path.string());
            srv->set_config(config);
            srv->configure();
            srv->start();

            CHECK(srv->has_location_defined());

            srv->stop();
        }

        // Base location defined and in FOLDER mode with an additional folder
        {
            auto srv = std::make_shared<test_writer>(sight::io::service::folder);

            sight::service::config_t config;
            config.add("baseFolder", m_root_path.string());
            config.add("folder", m_folder);
            srv->set_config(config);
            srv->configure();
            srv->start();

            CHECK(srv->has_location_defined());

            srv->stop();
        }
    }

    TEST_CASE_FIXTURE(fixture, "test_output_paths")
    {
        // Test folder access
        {
            auto srv = std::make_shared<test_writer>(sight::io::service::folder);

            sight::service::config_t config;
            config.add("folder", (m_root_path / m_folder).string());
            srv->set_config(config);
            srv->configure();
            srv->start();

            CHECK_EQ(srv->get_folder(), (m_root_path / m_folder).string());

            srv->stop();
        }

        // Test file access
        {
            auto srv = std::make_shared<test_writer>(sight::io::service::file);

            sight::service::config_t config;
            config.add("file", (m_root_path / m_file).string());
            srv->set_config(config);
            srv->configure();
            srv->start();

            CHECK_EQ(srv->get_file(), (m_root_path / m_file).string());

            srv->stop();
        }

        // Test base folder with file
        {
            auto srv = std::make_shared<test_writer>(sight::io::service::file);

            sight::service::config_t config;
            config.add("baseFolder", m_root_path.string());
            config.add("file", m_file);
            srv->set_config(config);
            srv->configure();
            srv->start();

            CHECK_EQ(srv->get_file(), (m_root_path / m_file).string());

            srv->stop();
        }

        // Test base folder with folder
        {
            auto srv = std::make_shared<test_writer>(sight::io::service::folder);

            sight::service::config_t config;
            config.add("baseFolder", m_root_path.string());
            config.add("folder", m_folder);
            srv->set_config(config);
            srv->configure();
            srv->start();

            CHECK_EQ(srv->get_folder(), (m_root_path / m_folder).string());

            srv->stop();
        }
    }
}
