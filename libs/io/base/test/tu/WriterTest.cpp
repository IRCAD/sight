/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include <core/Exception.hpp>
#include <core/runtime/operations.hpp>
#include <core/runtime/Profile.hpp>
#include <core/tools/Os.hpp>
#include <core/tools/System.hpp>
#include <core/tools/UUID.hpp>

#include <ui/base/Preferences.hpp>

#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/property_tree/ptree.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::base::ut::WriterTest);

namespace sight::io::base
{

namespace ut
{

//------------------------------------------------------------------------------

void WriterTest::setUp()
{
    // Set the profile name (needed for the preferences to work)
    sight::core::runtime::init();
    const std::string& profileName = sight::core::tools::UUID::generateUUID();
    sight::core::runtime::getCurrentProfile()->setName(profileName);

    // Enables the preferences
    sight::ui::base::Preferences::set_enabled(true);

    m_rootPath = sight::core::tools::System::getTemporaryFolder();
}

//------------------------------------------------------------------------------

void WriterTest::tearDown()
{
    m_rootPath = "";

    ui::base::Preferences preferences;

    // Clear the preferences
    preferences.clear();
    // Disables the preferences
    sight::ui::base::Preferences::set_enabled(false);
}

//------------------------------------------------------------------------------

void WriterTest::test_typeOK()
{
    // Test using the wrong method for accessing a file
    {
        // Test adding service
        auto srv = std::make_shared<sight::io::base::ut::STestWriter>();
        srv->setPathType(sight::io::base::service::FILE);

        sight::service::IService::ConfigType config;
        config.add("file", (m_rootPath / m_file).string());
        srv->setConfiguration(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT_NO_THROW(srv->getFile());
        CPPUNIT_ASSERT_THROW(srv->getFolder(), __FWCORE_EXCEPTION_CLASS);
    }

    // Test using the wrong method for accessing a folder
    {
        // Test adding service
        auto srv = std::make_shared<sight::io::base::ut::STestWriter>();
        srv->setPathType(sight::io::base::service::FOLDER);

        sight::service::IService::ConfigType config;
        config.add("folder", m_rootPath);
        srv->setConfiguration(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT_THROW(srv->getFile(), __FWCORE_EXCEPTION_CLASS);
        CPPUNIT_ASSERT_NO_THROW(srv->getFolder());
    }
}

//------------------------------------------------------------------------------

void WriterTest::test_hasLocationDefined()
{
    // No location defined
    {
        auto srv = std::make_shared<sight::io::base::ut::STestWriter>();
        srv->setPathType(sight::io::base::service::FILE);

        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(!srv->hasLocationDefined());
    }

    // No absolute location defined
    {
        auto srv = std::make_shared<sight::io::base::ut::STestWriter>();
        srv->setPathType(sight::io::base::service::FILE);

        sight::service::IService::ConfigType config;
        config.add("file", m_file);
        srv->setConfiguration(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(!srv->hasLocationDefined());
    }

    // Absolute location defined
    {
        auto srv = std::make_shared<sight::io::base::ut::STestWriter>();
        srv->setPathType(sight::io::base::service::FILE);
        sight::service::IService::ConfigType config;
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
        auto srv = std::make_shared<sight::io::base::ut::STestWriter>();
        srv->setPathType(sight::io::base::service::FILE);

        sight::service::IService::ConfigType config;
        config.add("baseFolder", m_rootPath.string());
        srv->setConfiguration(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(!srv->hasLocationDefined());
    }

    // Base location defined and a file in FILE mode
    {
        auto srv = std::make_shared<sight::io::base::ut::STestWriter>();
        srv->setPathType(sight::io::base::service::FILE);

        sight::service::IService::ConfigType config;
        config.add("baseFolder", m_rootPath.string());
        config.add("file", m_file);
        srv->setConfiguration(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(srv->hasLocationDefined());
    }

    // Base location defined and in FOLDER mode
    {
        auto srv = std::make_shared<sight::io::base::ut::STestWriter>();
        srv->setPathType(sight::io::base::service::FOLDER);

        sight::service::IService::ConfigType config;
        config.add("baseFolder", m_rootPath.string());
        srv->setConfiguration(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(srv->hasLocationDefined());
    }

    // Base location defined and in FOLDER mode with an additional folder
    {
        auto srv = std::make_shared<sight::io::base::ut::STestWriter>();
        srv->setPathType(sight::io::base::service::FOLDER);

        sight::service::IService::ConfigType config;
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
        auto srv = std::make_shared<sight::io::base::ut::STestWriter>();
        srv->setPathType(sight::io::base::service::FOLDER);

        sight::service::IService::ConfigType config;
        config.add("folder", (m_rootPath / m_folder).string());
        srv->setConfiguration(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(srv->getFolder() == (m_rootPath / m_folder).string());
    }

    // Test file access
    {
        auto srv = std::make_shared<sight::io::base::ut::STestWriter>();
        srv->setPathType(sight::io::base::service::FILE);

        sight::service::IService::ConfigType config;
        config.add("file", (m_rootPath / m_file).string());
        srv->setConfiguration(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(srv->getFile() == (m_rootPath / m_file).string());
    }

    // Test base folder with file
    {
        auto srv = std::make_shared<sight::io::base::ut::STestWriter>();
        srv->setPathType(sight::io::base::service::FILE);

        sight::service::IService::ConfigType config;
        config.add("baseFolder", m_rootPath.string());
        config.add("file", m_file);
        srv->setConfiguration(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(srv->getFile() == (m_rootPath / m_file).string());
    }

    // Test base folder with folder
    {
        auto srv = std::make_shared<sight::io::base::ut::STestWriter>();
        srv->setPathType(sight::io::base::service::FOLDER);

        sight::service::IService::ConfigType config;
        config.add("baseFolder", m_rootPath.string());
        config.add("folder", m_folder);
        srv->setConfiguration(config);
        srv->configure();
        srv->start();

        CPPUNIT_ASSERT(srv->getFolder() == (m_rootPath / m_folder).string());
    }
}

} //namespace ut

} //namespace sight::io::base
