/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include "SessionTest.hpp"

#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/os/temp_path.hpp>

#include <data/String.hpp>

#include <io/__/service/ioTypes.hpp>
#include <io/__/service/reader.hpp>
#include <io/__/service/writer.hpp>

#include <service/base.hpp>
#include <service/op/Add.hpp>

#include <ui/__/dialog/input_dummy.hpp>
#include <ui/__/dialog/location_dummy.hpp>
#include <ui/__/dialog/message_dummy.hpp>
#include <ui/__/macros.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::session::ut::SessionTest);

SIGHT_REGISTER_GUI(sight::ui::dialog::location_dummy, sight::ui::dialog::location_base::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::dialog::input_dummy, sight::ui::dialog::input_base::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::dialog::message_dummy, sight::ui::dialog::message_base::REGISTRY_KEY);

namespace sight::module::io::session::ut
{

//------------------------------------------------------------------------------

void SessionTest::setUp()
{
}

//------------------------------------------------------------------------------

void SessionTest::tearDown()
{
}

//------------------------------------------------------------------------------

// Returns a basic configuration for SReader and SWriter
inline static service::config_t getConfiguration(const bool read = true, const bool raw = false)
{
    service::config_t config;

    config.put("dialog.<xmlattr>.extension", ".perlimpinpin");
    config.put("dialog.<xmlattr>.description", "Perlimpinpin powder container");
    config.put("dialog.<xmlattr>.policy", "never");

    config.put("password.<xmlattr>.policy", "never");
    config.put("password.<xmlattr>.encryption", "password");

    if(raw)
    {
        config.put("archive.<xmlattr>.format", "filesystem");
    }
    else if(read)
    {
        config.put("archive.<xmlattr>.format", "archive");
    }
    else
    {
        config.put("archive.<xmlattr>.format", "optimized");
    }

    return config;
}

//------------------------------------------------------------------------------

inline static void basicTest(const bool raw = false)
{
    // Create a temporary file
    core::os::temp_dir tmpDir;
    const auto& tmpFile = tmpDir / "powder.perlimpinpin";

    const std::string expected("Abracadabra");
    {
        // Create a writer service
        auto writer = std::dynamic_pointer_cast<sight::io::service::writer>(
            service::add("sight::module::io::session::SWriter")
        );
        CPPUNIT_ASSERT(writer);

        // Set data input
        auto inString = std::make_shared<data::String>(expected);
        writer->setInput(inString, sight::io::service::s_DATA_KEY);

        // Set file output
        writer->set_file(tmpFile);

        // Configure the writer service
        writer->setConfiguration(getConfiguration(false, raw));
        writer->configure();

        // Execute the writer service
        writer->start().wait();
        writer->update().wait();
        writer->stop().wait();

        // Cleanup
        service::unregisterService(writer);
    }

    // The file should have been created
    CPPUNIT_ASSERT(
        std::filesystem::exists(tmpFile)
        && std::filesystem::is_regular_file(tmpFile)
        && std::filesystem::file_size(tmpFile) > 0
    );

    {
        // Create a reader service
        auto reader = std::dynamic_pointer_cast<sight::io::service::reader>(
            service::add("sight::module::io::session::SReader")
        );
        CPPUNIT_ASSERT(reader);

        // Set data output
        auto outString = std::make_shared<data::String>();
        reader->setInOut(outString, sight::io::service::s_DATA_KEY);

        // Set file input
        reader->set_file(tmpFile);

        // Create slot connections
        bool sessionLoaded = false;
        std::filesystem::path sessionLoadedPath;
        auto sessionLoadedSlot = sight::core::com::new_slot(
            [&](std::filesystem::path path)
            {
                sessionLoaded     = true;
                sessionLoadedPath = path;
            });
        sessionLoadedSlot->set_worker(sight::core::thread::get_default_worker());
        auto conn1 = reader->signal("sessionLoaded")->connect(sessionLoadedSlot);

        bool sessionLoadingFailed = false;
        std::filesystem::path sessionLoadingFailedPath;
        auto sessionLoadingFailedSlot = sight::core::com::new_slot(
            [&sessionLoadingFailed, &sessionLoadingFailedPath](std::filesystem::path path)
            {
                sessionLoadingFailed     = true;
                sessionLoadingFailedPath = path;
            });
        sessionLoadingFailedSlot->set_worker(sight::core::thread::get_default_worker());
        auto conn2 = reader->signal("sessionLoadingFailed")->connect(sessionLoadingFailedSlot);

        // Configure the reader service
        reader->setConfiguration(getConfiguration(true, raw));
        reader->configure();

        // Execute the writer service
        reader->start().wait();
        reader->update().wait();
        reader->stop().wait();

        // Cleanup
        service::unregisterService(reader);

        // Final test
        CPPUNIT_ASSERT(sessionLoaded);
        CPPUNIT_ASSERT_EQUAL(sessionLoadedPath, tmpFile);
        CPPUNIT_ASSERT(!sessionLoadingFailed);
        CPPUNIT_ASSERT_EQUAL(sessionLoadingFailedPath, std::filesystem::path(""));

        CPPUNIT_ASSERT_EQUAL(expected, outString->getValue());
    }
}

//------------------------------------------------------------------------------

void SessionTest::basicArchiveTest()
{
    basicTest(false);
}

//------------------------------------------------------------------------------

void SessionTest::basicRawTest()
{
    basicTest(true);
}

//------------------------------------------------------------------------------

static void badPolicyTest(bool reader, const std::string& key, const std::string& value)
{
    auto config = getConfiguration(reader);
    config.put(key, value);
    service::base::sptr service;
    if(reader)
    {
        service = service::add("sight::module::io::session::SReader");
    }
    else
    {
        service = service::add("sight::module::io::session::SWriter");
    }

    CPPUNIT_ASSERT(service);
    service->setConfiguration(config);
    CPPUNIT_ASSERT_THROW(service->configure(), sight::core::exception);
}

//------------------------------------------------------------------------------

void SessionTest::readerBadDialogPolicyTest()
{
    badPolicyTest(true, "dialog.<xmlattr>.policy", "whenever_i_want");
}

//------------------------------------------------------------------------------

void SessionTest::readerBadPasswordPolicyTest()
{
    badPolicyTest(true, "password.<xmlattr>.policy", "when_i_remember_it");
}

//------------------------------------------------------------------------------

void SessionTest::readerBadPasswordEncryptionTest()
{
    badPolicyTest(true, "password.<xmlattr>.encryption", "sweet");
}

//------------------------------------------------------------------------------

void SessionTest::readerBadFile()
{
    // Create a temporary file
    core::os::temp_dir tmpDir;
    const auto& tmpFile = tmpDir / "powder.perlimpinpin";

    {
        // Create a reader service
        auto reader = std::dynamic_pointer_cast<sight::io::service::reader>(
            service::add("sight::module::io::session::SReader")
        );
        CPPUNIT_ASSERT(reader);

        // Set data output
        auto outString = std::make_shared<data::String>();
        reader->setInOut(outString, sight::io::service::s_DATA_KEY);

        // Set file input
        reader->set_file(tmpFile);

        // Create slot connections
        bool sessionLoaded = false;
        std::filesystem::path sessionLoadedPath;
        auto sessionLoadedSlot = sight::core::com::new_slot(
            [&](std::filesystem::path path)
            {
                sessionLoaded     = true;
                sessionLoadedPath = path;
            });
        sessionLoadedSlot->set_worker(sight::core::thread::get_default_worker());
        auto conn1 = reader->signal("sessionLoaded")->connect(sessionLoadedSlot);

        bool sessionLoadingFailed = false;
        std::filesystem::path sessionLoadingFailedPath;
        auto sessionLoadingFailedSlot = sight::core::com::new_slot(
            [&sessionLoadingFailed, &sessionLoadingFailedPath](std::filesystem::path path)
            {
                sessionLoadingFailed     = true;
                sessionLoadingFailedPath = path;
            });
        sessionLoadingFailedSlot->set_worker(sight::core::thread::get_default_worker());
        auto conn2 = reader->signal("sessionLoadingFailed")->connect(sessionLoadingFailedSlot);

        // Configure the reader service
        reader->setConfiguration(getConfiguration(true, false));
        reader->configure();

        // Execute the writer service
        reader->start().wait();
        reader->update().wait();
        reader->stop().wait();

        // Cleanup
        service::unregisterService(reader);

        // Final test
        CPPUNIT_ASSERT(!sessionLoaded);
        CPPUNIT_ASSERT_EQUAL(sessionLoadedPath, std::filesystem::path(""));
        CPPUNIT_ASSERT(sessionLoadingFailed);
        CPPUNIT_ASSERT_EQUAL(sessionLoadingFailedPath, tmpFile);
    }
}

//------------------------------------------------------------------------------

void SessionTest::writerBadDialogPolicyTest()
{
    badPolicyTest(false, "dialog.<xmlattr>.policy", "whenever_i_want");
}

//------------------------------------------------------------------------------

void SessionTest::writerBadPasswordPolicyTest()
{
    badPolicyTest(false, "password.<xmlattr>.policy", "when_i_remember_it");
}

//------------------------------------------------------------------------------

void SessionTest::writerBadPasswordEncryptionTest()
{
    badPolicyTest(false, "password.<xmlattr>.encryption", "sweet");
}

//------------------------------------------------------------------------------

void SessionTest::fileDialogTest()
{
    // Create a temporary file
    core::os::temp_dir tmpDir;
    const auto& tmpFile = tmpDir / "powder.perlimpinpin";

    const std::string expected("Abracadabra");
    {
        // Create a writer service
        auto writer = std::dynamic_pointer_cast<sight::io::service::writer>(
            service::add("sight::module::io::session::SWriter")
        );
        CPPUNIT_ASSERT(writer);

        // Set data input
        auto inString = std::make_shared<data::String>(expected);
        writer->setInput(inString, sight::io::service::s_DATA_KEY);

        // Configure the writer service
        auto config = getConfiguration(false);
        config.put("dialog.<xmlattr>.policy", "always");
        writer->setConfiguration(config);
        writer->configure();

        // Execute the writer service
        writer->start().wait();

        sight::ui::dialog::location_dummy::setPaths({tmpFile});

        writer->update().wait();
        writer->stop().wait();

        // Cleanup
        service::unregisterService(writer);

        CPPUNIT_ASSERT(sight::ui::dialog::location_dummy::clear());
    }

    // The file should have been created
    CPPUNIT_ASSERT(
        std::filesystem::exists(tmpFile)
        && std::filesystem::is_regular_file(tmpFile)
        && std::filesystem::file_size(tmpFile) > 0
    );

    {
        // Create a reader service
        auto reader = std::dynamic_pointer_cast<sight::io::service::reader>(
            service::add("sight::module::io::session::SReader")
        );
        CPPUNIT_ASSERT(reader);

        // Set data output
        auto outString = std::make_shared<data::String>();
        reader->setInOut(outString, sight::io::service::s_DATA_KEY);

        // Configure the reader service
        auto config = getConfiguration(true);
        config.put("dialog.<xmlattr>.policy", "always");
        reader->setConfiguration(config);
        reader->configure();

        // Execute the writer service
        reader->start().wait();

        sight::ui::dialog::location_dummy::setPaths({tmpFile});

        reader->update().wait();
        reader->stop().wait();

        // Cleanup
        service::unregisterService(reader);

        // Final test
        CPPUNIT_ASSERT_EQUAL(expected, outString->getValue());

        CPPUNIT_ASSERT(sight::ui::dialog::location_dummy::clear());
    }
}

//------------------------------------------------------------------------------

void SessionTest::passwordTest()
{
    // Create a temporary file
    core::os::temp_dir tmpDir;
    const auto& tmpFile = tmpDir / "powder.perlimpinpin";

    const std::string expected("Abracadabra");
    {
        // Create a writer service
        auto writer = std::dynamic_pointer_cast<sight::io::service::writer>(
            service::add("sight::module::io::session::SWriter")
        );
        CPPUNIT_ASSERT(writer);

        // Set data input
        auto inString = std::make_shared<data::String>(expected);
        writer->setInput(inString, sight::io::service::s_DATA_KEY);

        // Set file output
        writer->set_file(tmpFile);

        // Configure the writer service
        auto config = getConfiguration(false);
        config.put("password.<xmlattr>.policy", "always");
        writer->setConfiguration(config);
        writer->configure();

        // Execute the writer service
        writer->start().wait();

        sight::ui::dialog::input_dummy::pushInput("case-sensitive");

        writer->update().wait();
        writer->stop().wait();

        // Cleanup
        service::unregisterService(writer);

        CPPUNIT_ASSERT(sight::ui::dialog::input_dummy::clear());
    }

    // The file should have been created
    CPPUNIT_ASSERT(
        std::filesystem::exists(tmpFile)
        && std::filesystem::is_regular_file(tmpFile)
        && std::filesystem::file_size(tmpFile) > 0
    );

    {
        // Create a reader service
        auto reader = std::dynamic_pointer_cast<sight::io::service::reader>(
            service::add("sight::module::io::session::SReader")
        );
        CPPUNIT_ASSERT(reader);

        // Set data output
        auto outString = std::make_shared<data::String>();
        reader->setInOut(outString, sight::io::service::s_DATA_KEY);

        // Set file input
        reader->set_file(tmpFile);

        // Configure the reader service
        auto config = getConfiguration(true);
        config.put("password.<xmlattr>.policy", "always");
        reader->setConfiguration(config);
        reader->configure();

        // Execute the writer service
        reader->start().wait();

        sight::ui::dialog::input_dummy::pushInput("Oops");
        ui::dialog::message_dummy::pushAction(sight::ui::dialog::message_dummy::RETRY);
        sight::ui::dialog::input_dummy::pushInput("I forgot");
        ui::dialog::message_dummy::pushAction(sight::ui::dialog::message_dummy::RETRY);
        sight::ui::dialog::input_dummy::pushInput("Wait I remember");
        ui::dialog::message_dummy::pushAction(sight::ui::dialog::message_dummy::RETRY);
        sight::ui::dialog::input_dummy::pushInput("case-sensitive");

        reader->update().wait();
        reader->stop().wait();

        // Cleanup
        service::unregisterService(reader);

        // Final test
        CPPUNIT_ASSERT_EQUAL(expected, outString->getValue());

        CPPUNIT_ASSERT(sight::ui::dialog::input_dummy::clear());
        CPPUNIT_ASSERT(ui::dialog::message_dummy::clear());
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::session::ut
