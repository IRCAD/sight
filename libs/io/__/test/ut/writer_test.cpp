/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

#include "writer_test.hpp"

#include <core/exception.hpp>
#include <core/os/temp_path.hpp>
#include <core/runtime/profile.hpp>
#include <core/runtime/runtime.hpp>
#include <core/tools/os.hpp>
#include <core/tools/uuid.hpp>

#include <ui/__/preferences.hpp>

#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/property_tree/ptree.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::ut::writer_test);

namespace sight::io::ut
{

//------------------------------------------------------------------------------

void writer_test::setUp()
{
    // Set the profile name (needed for the preferences to work)
    sight::core::runtime::init();
    const std::string& profile_name = sight::core::tools::uuid::generate();
    sight::core::runtime::get_current_profile()->set_name(profile_name);

    // Enables the preferences
    sight::ui::preferences::set_enabled(true);

    m_root_path = core::os::temp_dir::shared_directory();
}

//------------------------------------------------------------------------------

void writer_test::tearDown()
{
    m_root_path = "";

    ui::preferences preferences;

    // Clear the preferences
    ui::preferences::clear();

    // Disables the preferences
    sight::ui::preferences::set_enabled(false);
}

//------------------------------------------------------------------------------

void writer_test::test_type_ok()
{
    // Test using the wrong method for accessing a file
    {
        // Test adding service
        auto srv = std::make_shared<sight::io::ut::s_test_writer>(sight::io::service::file);

        sight::service::config_t config;
        config.add("file", (m_root_path / m_file).string());
        srv->set_config(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT_NO_THROW(srv->get_file());
        CPPUNIT_ASSERT_THROW(srv->get_folder(), FWCORE_EXCEPTION_CLASS);
    }

    // Test using the wrong method for accessing a folder
    {
        // Test adding service
        auto srv = std::make_shared<sight::io::ut::s_test_writer>(sight::io::service::folder);

        sight::service::config_t config;
        config.add("folder", m_root_path);
        srv->set_config(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT_THROW(srv->get_file(), FWCORE_EXCEPTION_CLASS);
        CPPUNIT_ASSERT_NO_THROW(srv->get_folder());
    }
}

//------------------------------------------------------------------------------

void writer_test::test_has_location_defined()
{
    // No location defined
    {
        auto srv = std::make_shared<sight::io::ut::s_test_writer>(sight::io::service::file);

        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(!srv->has_location_defined());
    }

    // No absolute location defined
    {
        auto srv = std::make_shared<sight::io::ut::s_test_writer>(sight::io::service::file);

        sight::service::config_t config;
        config.add("file", m_file);
        srv->set_config(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(!srv->has_location_defined());
    }

    // Absolute location defined
    {
        auto srv = std::make_shared<sight::io::ut::s_test_writer>(sight::io::service::file);
        sight::service::config_t config;
        config.add("file", (m_root_path / m_file).string());
        srv->set_config(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(srv->has_location_defined());
    }
}

//------------------------------------------------------------------------------

void writer_test::test_has_location_defined_with_base_folder()
{
    // Base location defined but no file in FILE mode
    {
        auto srv = std::make_shared<sight::io::ut::s_test_writer>(sight::io::service::file);

        sight::service::config_t config;
        config.add("baseFolder", m_root_path.string());
        srv->set_config(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(!srv->has_location_defined());
    }

    // Base location defined and a file in FILE mode
    {
        auto srv = std::make_shared<sight::io::ut::s_test_writer>(sight::io::service::file);

        sight::service::config_t config;
        config.add("baseFolder", m_root_path.string());
        config.add("file", m_file);
        srv->set_config(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(srv->has_location_defined());
    }

    // Base location defined and in FOLDER mode
    {
        auto srv = std::make_shared<sight::io::ut::s_test_writer>(sight::io::service::folder);

        sight::service::config_t config;
        config.add("baseFolder", m_root_path.string());
        srv->set_config(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(srv->has_location_defined());
    }

    // Base location defined and in FOLDER mode with an additional folder
    {
        auto srv = std::make_shared<sight::io::ut::s_test_writer>(sight::io::service::folder);

        sight::service::config_t config;
        config.add("baseFolder", m_root_path.string());
        config.add("folder", m_folder);
        srv->set_config(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(srv->has_location_defined());
    }
}

//------------------------------------------------------------------------------

void writer_test::test_output_paths()
{
    // Test folder access
    {
        auto srv = std::make_shared<sight::io::ut::s_test_writer>(sight::io::service::folder);

        sight::service::config_t config;
        config.add("folder", (m_root_path / m_folder).string());
        srv->set_config(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(srv->get_folder() == (m_root_path / m_folder).string());
    }

    // Test file access
    {
        auto srv = std::make_shared<sight::io::ut::s_test_writer>(sight::io::service::file);

        sight::service::config_t config;
        config.add("file", (m_root_path / m_file).string());
        srv->set_config(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(srv->get_file() == (m_root_path / m_file).string());
    }

    // Test base folder with file
    {
        auto srv = std::make_shared<sight::io::ut::s_test_writer>(sight::io::service::file);

        sight::service::config_t config;
        config.add("baseFolder", m_root_path.string());
        config.add("file", m_file);
        srv->set_config(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(srv->get_file() == (m_root_path / m_file).string());
    }

    // Test base folder with folder
    {
        auto srv = std::make_shared<sight::io::ut::s_test_writer>(sight::io::service::folder);

        sight::service::config_t config;
        config.add("baseFolder", m_root_path.string());
        config.add("folder", m_folder);
        srv->set_config(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(srv->get_folder() == (m_root_path / m_folder).string());
    }
}

} // namespace sight::io::ut
