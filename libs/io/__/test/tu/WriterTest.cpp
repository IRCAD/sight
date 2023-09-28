/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "WriterTest.hpp"

#include <core/exception.hpp>
#include <core/os/temp_path.hpp>
#include <core/runtime/profile.hpp>
#include <core/runtime/runtime.hpp>
#include <core/tools/os.hpp>
#include <core/tools/uuid.hpp>

#include <ui/__/Preferences.hpp>

#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/property_tree/ptree.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::ut::WriterTest);

namespace sight::io::ut
{

//------------------------------------------------------------------------------

void WriterTest::setUp()
{
    // Set the profile name (needed for the preferences to work)
    sight::core::runtime::init();
    const std::string& profileName = sight::core::tools::UUID::generate();
    sight::core::runtime::get_current_profile()->set_name(profileName);

    // Enables the preferences
    sight::ui::Preferences::set_enabled(true);

    m_rootPath = core::os::temp_dir::shared_directory();
}

//------------------------------------------------------------------------------

void WriterTest::tearDown()
{
    m_rootPath = "";

    ui::Preferences preferences;

    // Clear the preferences
    ui::Preferences::clear();
    // Disables the preferences
    sight::ui::Preferences::set_enabled(false);
}

//------------------------------------------------------------------------------

void WriterTest::test_typeOK()
{
    // Test using the wrong method for accessing a file
    {
        // Test adding service
        auto srv = std::make_shared<sight::io::ut::STestWriter>(sight::io::service::FILE);

        sight::service::config_t config;
        config.add("file", (m_rootPath / m_file).string());
        srv->setConfiguration(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT_NO_THROW(srv->get_file());
        CPPUNIT_ASSERT_THROW(srv->get_folder(), __FWCORE_EXCEPTION_CLASS);
    }

    // Test using the wrong method for accessing a folder
    {
        // Test adding service
        auto srv = std::make_shared<sight::io::ut::STestWriter>(sight::io::service::FOLDER);

        sight::service::config_t config;
        config.add("folder", m_rootPath);
        srv->setConfiguration(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT_THROW(srv->get_file(), __FWCORE_EXCEPTION_CLASS);
        CPPUNIT_ASSERT_NO_THROW(srv->get_folder());
    }
}

//------------------------------------------------------------------------------

void WriterTest::test_hasLocationDefined()
{
    // No location defined
    {
        auto srv = std::make_shared<sight::io::ut::STestWriter>(sight::io::service::FILE);

        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(!srv->hasLocationDefined());
    }

    // No absolute location defined
    {
        auto srv = std::make_shared<sight::io::ut::STestWriter>(sight::io::service::FILE);

        sight::service::config_t config;
        config.add("file", m_file);
        srv->setConfiguration(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(!srv->hasLocationDefined());
    }

    // Absolute location defined
    {
        auto srv = std::make_shared<sight::io::ut::STestWriter>(sight::io::service::FILE);
        sight::service::config_t config;
        config.add("file", (m_rootPath / m_file).string());
        srv->setConfiguration(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(srv->hasLocationDefined());
    }
}

//------------------------------------------------------------------------------

void WriterTest::test_hasLocationDefinedWithBaseFolder()
{
    // Base location defined but no file in FILE mode
    {
        auto srv = std::make_shared<sight::io::ut::STestWriter>(sight::io::service::FILE);

        sight::service::config_t config;
        config.add("baseFolder", m_rootPath.string());
        srv->setConfiguration(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(!srv->hasLocationDefined());
    }

    // Base location defined and a file in FILE mode
    {
        auto srv = std::make_shared<sight::io::ut::STestWriter>(sight::io::service::FILE);

        sight::service::config_t config;
        config.add("baseFolder", m_rootPath.string());
        config.add("file", m_file);
        srv->setConfiguration(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(srv->hasLocationDefined());
    }

    // Base location defined and in FOLDER mode
    {
        auto srv = std::make_shared<sight::io::ut::STestWriter>(sight::io::service::FOLDER);

        sight::service::config_t config;
        config.add("baseFolder", m_rootPath.string());
        srv->setConfiguration(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(srv->hasLocationDefined());
    }

    // Base location defined and in FOLDER mode with an additional folder
    {
        auto srv = std::make_shared<sight::io::ut::STestWriter>(sight::io::service::FOLDER);

        sight::service::config_t config;
        config.add("baseFolder", m_rootPath.string());
        config.add("folder", m_folder);
        srv->setConfiguration(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(srv->hasLocationDefined());
    }
}

//------------------------------------------------------------------------------

void WriterTest::test_outputPaths()
{
    // Test folder access
    {
        auto srv = std::make_shared<sight::io::ut::STestWriter>(sight::io::service::FOLDER);

        sight::service::config_t config;
        config.add("folder", (m_rootPath / m_folder).string());
        srv->setConfiguration(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(srv->get_folder() == (m_rootPath / m_folder).string());
    }

    // Test file access
    {
        auto srv = std::make_shared<sight::io::ut::STestWriter>(sight::io::service::FILE);

        sight::service::config_t config;
        config.add("file", (m_rootPath / m_file).string());
        srv->setConfiguration(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(srv->get_file() == (m_rootPath / m_file).string());
    }

    // Test base folder with file
    {
        auto srv = std::make_shared<sight::io::ut::STestWriter>(sight::io::service::FILE);

        sight::service::config_t config;
        config.add("baseFolder", m_rootPath.string());
        config.add("file", m_file);
        srv->setConfiguration(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(srv->get_file() == (m_rootPath / m_file).string());
    }

    // Test base folder with folder
    {
        auto srv = std::make_shared<sight::io::ut::STestWriter>(sight::io::service::FOLDER);

        sight::service::config_t config;
        config.add("baseFolder", m_rootPath.string());
        config.add("folder", m_folder);
        srv->setConfiguration(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(srv->get_folder() == (m_rootPath / m_folder).string());
    }
}

} // namespace sight::io::ut
