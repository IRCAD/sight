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

#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/os/TempPath.hpp>

#include <data/String.hpp>

#include <io/base/service/ioTypes.hpp>
#include <io/base/service/IReader.hpp>
#include <io/base/service/IWriter.hpp>

#include <service/IService.hpp>
#include <service/op/Add.hpp>

#include <ui/base/dialog/DummyInputDialog.hpp>
#include <ui/base/dialog/DummyLocationDialog.hpp>
#include <ui/base/dialog/DummyMessageDialog.hpp>
#include <ui/base/registry/macros.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::session::ut::SessionTest);

SIGHT_REGISTER_GUI(
    sight::ui::base::dialog::DummyLocationDialog,
    sight::ui::base::dialog::ILocationDialog::REGISTRY_KEY
);
SIGHT_REGISTER_GUI(sight::ui::base::dialog::DummyInputDialog, sight::ui::base::dialog::IInputDialog::REGISTRY_KEY);
SIGHT_REGISTER_GUI(sight::ui::base::dialog::DummyMessageDialog, sight::ui::base::dialog::IMessageDialog::REGISTRY_KEY);

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
inline static service::IService::ConfigType getConfiguration(const bool read = true, const bool raw = false)
{
    service::IService::ConfigType config;

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
    core::os::TempDir tmpDir;
    const auto& tmpFile = tmpDir / "powder.perlimpinpin";

    const std::string expected("Abracadabra");
    {
        // Create a writer service
        auto writer = sight::io::base::service::IWriter::dynamicCast(
            service::add("sight::module::io::session::SWriter")
        );
        CPPUNIT_ASSERT(writer);

        // Set data input
        auto inString = data::String::New(expected);
        writer->setInput(inString, sight::io::base::service::s_DATA_KEY);

        // Set file output
        writer->setFile(tmpFile);

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
        auto reader = sight::io::base::service::IReader::dynamicCast(
            service::add("sight::module::io::session::SReader")
        );
        CPPUNIT_ASSERT(reader);

        // Set data output
        auto outString = data::String::New();
        reader->setInOut(outString, sight::io::base::service::s_DATA_KEY);

        // Set file input
        reader->setFile(tmpFile);

        // Create slot connections
        bool sessionLoaded = false;
        std::filesystem::path sessionLoadedPath;
        auto sessionLoadedSlot = sight::core::com::newSlot(
            [&](std::filesystem::path path)
            {
                sessionLoaded     = true;
                sessionLoadedPath = path;
            });
        sessionLoadedSlot->setWorker(sight::core::thread::getDefaultWorker());
        auto conn1 = reader->signal("sessionLoaded")->connect(sessionLoadedSlot);

        bool sessionLoadingFailed = false;
        std::filesystem::path sessionLoadingFailedPath;
        auto sessionLoadingFailedSlot = sight::core::com::newSlot(
            [&sessionLoadingFailed, &sessionLoadingFailedPath](std::filesystem::path path)
            {
                sessionLoadingFailed     = true;
                sessionLoadingFailedPath = path;
            });
        sessionLoadingFailedSlot->setWorker(sight::core::thread::getDefaultWorker());
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
    service::IService::sptr service;
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
    CPPUNIT_ASSERT_THROW(service->configure(), sight::core::Exception);
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
    core::os::TempDir tmpDir;
    const auto& tmpFile = tmpDir / "powder.perlimpinpin";

    {
        // Create a reader service
        auto reader = sight::io::base::service::IReader::dynamicCast(
            service::add("sight::module::io::session::SReader")
        );
        CPPUNIT_ASSERT(reader);

        // Set data output
        auto outString = data::String::New();
        reader->setInOut(outString, sight::io::base::service::s_DATA_KEY);

        // Set file input
        reader->setFile(tmpFile);

        // Create slot connections
        bool sessionLoaded = false;
        std::filesystem::path sessionLoadedPath;
        auto sessionLoadedSlot = sight::core::com::newSlot(
            [&](std::filesystem::path path)
            {
                sessionLoaded     = true;
                sessionLoadedPath = path;
            });
        sessionLoadedSlot->setWorker(sight::core::thread::getDefaultWorker());
        auto conn1 = reader->signal("sessionLoaded")->connect(sessionLoadedSlot);

        bool sessionLoadingFailed = false;
        std::filesystem::path sessionLoadingFailedPath;
        auto sessionLoadingFailedSlot = sight::core::com::newSlot(
            [&sessionLoadingFailed, &sessionLoadingFailedPath](std::filesystem::path path)
            {
                sessionLoadingFailed     = true;
                sessionLoadingFailedPath = path;
            });
        sessionLoadingFailedSlot->setWorker(sight::core::thread::getDefaultWorker());
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
    core::os::TempDir tmpDir;
    const auto& tmpFile = tmpDir / "powder.perlimpinpin";

    const std::string expected("Abracadabra");
    {
        // Create a writer service
        auto writer = sight::io::base::service::IWriter::dynamicCast(
            service::add("sight::module::io::session::SWriter")
        );
        CPPUNIT_ASSERT(writer);

        // Set data input
        auto inString = data::String::New(expected);
        writer->setInput(inString, sight::io::base::service::s_DATA_KEY);

        // Configure the writer service
        auto config = getConfiguration(false);
        config.put("dialog.<xmlattr>.policy", "always");
        writer->setConfiguration(config);
        writer->configure();

        // Execute the writer service
        writer->start().wait();

        sight::ui::base::dialog::DummyLocationDialog::setPaths({tmpFile});

        writer->update().wait();
        writer->stop().wait();

        // Cleanup
        service::unregisterService(writer);

        CPPUNIT_ASSERT(sight::ui::base::dialog::DummyLocationDialog::clear());
    }

    // The file should have been created
    CPPUNIT_ASSERT(
        std::filesystem::exists(tmpFile)
        && std::filesystem::is_regular_file(tmpFile)
        && std::filesystem::file_size(tmpFile) > 0
    );

    {
        // Create a reader service
        auto reader = sight::io::base::service::IReader::dynamicCast(
            service::add("sight::module::io::session::SReader")
        );
        CPPUNIT_ASSERT(reader);

        // Set data output
        auto outString = data::String::New();
        reader->setInOut(outString, sight::io::base::service::s_DATA_KEY);

        // Configure the reader service
        auto config = getConfiguration(true);
        config.put("dialog.<xmlattr>.policy", "always");
        reader->setConfiguration(config);
        reader->configure();

        // Execute the writer service
        reader->start().wait();

        sight::ui::base::dialog::DummyLocationDialog::setPaths({tmpFile});

        reader->update().wait();
        reader->stop().wait();

        // Cleanup
        service::unregisterService(reader);

        // Final test
        CPPUNIT_ASSERT_EQUAL(expected, outString->getValue());

        CPPUNIT_ASSERT(sight::ui::base::dialog::DummyLocationDialog::clear());
    }
}

//------------------------------------------------------------------------------

void SessionTest::passwordTest()
{
    // Create a temporary file
    core::os::TempDir tmpDir;
    const auto& tmpFile = tmpDir / "powder.perlimpinpin";

    const std::string expected("Abracadabra");
    {
        // Create a writer service
        auto writer = sight::io::base::service::IWriter::dynamicCast(
            service::add("sight::module::io::session::SWriter")
        );
        CPPUNIT_ASSERT(writer);

        // Set data input
        auto inString = data::String::New(expected);
        writer->setInput(inString, sight::io::base::service::s_DATA_KEY);

        // Set file output
        writer->setFile(tmpFile);

        // Configure the writer service
        auto config = getConfiguration(false);
        config.put("password.<xmlattr>.policy", "always");
        writer->setConfiguration(config);
        writer->configure();

        // Execute the writer service
        writer->start().wait();

        sight::ui::base::dialog::DummyInputDialog::pushInput("case-sensitive");

        writer->update().wait();
        writer->stop().wait();

        // Cleanup
        service::unregisterService(writer);

        CPPUNIT_ASSERT(sight::ui::base::dialog::DummyInputDialog::clear());
    }

    // The file should have been created
    CPPUNIT_ASSERT(
        std::filesystem::exists(tmpFile)
        && std::filesystem::is_regular_file(tmpFile)
        && std::filesystem::file_size(tmpFile) > 0
    );

    {
        // Create a reader service
        auto reader = sight::io::base::service::IReader::dynamicCast(
            service::add("sight::module::io::session::SReader")
        );
        CPPUNIT_ASSERT(reader);

        // Set data output
        auto outString = data::String::New();
        reader->setInOut(outString, sight::io::base::service::s_DATA_KEY);

        // Set file input
        reader->setFile(tmpFile);

        // Configure the reader service
        auto config = getConfiguration(true);
        config.put("password.<xmlattr>.policy", "always");
        reader->setConfiguration(config);
        reader->configure();

        // Execute the writer service
        reader->start().wait();

        sight::ui::base::dialog::DummyInputDialog::pushInput("Oops");
        ui::base::dialog::DummyMessageDialog::pushAction(sight::ui::base::dialog::IMessageDialog::RETRY);
        sight::ui::base::dialog::DummyInputDialog::pushInput("I forgot");
        ui::base::dialog::DummyMessageDialog::pushAction(sight::ui::base::dialog::IMessageDialog::RETRY);
        sight::ui::base::dialog::DummyInputDialog::pushInput("Wait I remember");
        ui::base::dialog::DummyMessageDialog::pushAction(sight::ui::base::dialog::IMessageDialog::RETRY);
        sight::ui::base::dialog::DummyInputDialog::pushInput("case-sensitive");

        reader->update().wait();
        reader->stop().wait();

        // Cleanup
        service::unregisterService(reader);

        // Final test
        CPPUNIT_ASSERT_EQUAL(expected, outString->getValue());

        CPPUNIT_ASSERT(sight::ui::base::dialog::DummyInputDialog::clear());
        CPPUNIT_ASSERT(ui::base::dialog::DummyMessageDialog::clear());
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::session::ut
