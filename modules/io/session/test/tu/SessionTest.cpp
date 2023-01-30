/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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

#include <core/tools/System.hpp>

#include <data/String.hpp>

#include <io/base/service/ioTypes.hpp>
#include <io/base/service/IReader.hpp>
#include <io/base/service/IWriter.hpp>

#include <service/IService.hpp>
#include <service/op/Add.hpp>

#include <ui/base/dialog/DummyInputDialog.hpp>
#include <ui/base/dialog/DummyLocationDialog.hpp>
#include <ui/base/dialog/IMessageDialog.hpp>
#include <ui/base/registry/macros.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::session::ut::SessionTest);

SIGHT_REGISTER_GUI(
    sight::ui::base::dialog::DummyLocationDialog,
    sight::ui::base::dialog::ILocationDialog::REGISTRY_KEY
);
SIGHT_REGISTER_GUI(sight::ui::base::dialog::DummyInputDialog, sight::ui::base::dialog::IInputDialog::REGISTRY_KEY);

namespace sight::module::io::session::ut
{

class DummyMessageDialog : public sight::ui::base::dialog::IMessageDialog
{
public:

    explicit DummyMessageDialog(sight::ui::base::factory::Key /*key*/)
    {
    }

    //------------------------------------------------------------------------------

    void setTitle(const std::string& /*title*/) override
    {
    }

    //------------------------------------------------------------------------------

    void setMessage(const std::string& /*msg*/) override
    {
    }

    //------------------------------------------------------------------------------

    void setIcon(Icons /*icon*/) override
    {
    }

    //------------------------------------------------------------------------------

    void addButton(Buttons /*button*/) override
    {
    }

    //------------------------------------------------------------------------------

    void setDefaultButton(Buttons /*button*/) override
    {
    }

    //------------------------------------------------------------------------------

    void addCustomButton(const std::string& /*label*/, std::function<void()> /*clickedFn*/) override
    {
    }

    static std::queue<sight::ui::base::dialog::IMessageDialog::Buttons> actions;

    //------------------------------------------------------------------------------

    static void pushAction(sight::ui::base::dialog::IMessageDialog::Buttons action)
    {
        actions.push(action);
    }

    //------------------------------------------------------------------------------

    sight::ui::base::dialog::IMessageDialog::Buttons show() override
    {
        sight::ui::base::dialog::IMessageDialog::Buttons res = NOBUTTON;
        if(!actions.empty())
        {
            res = actions.front();
            actions.pop();
        }

        return res;
    }
};

std::queue<sight::ui::base::dialog::IMessageDialog::Buttons> DummyMessageDialog::actions;

SIGHT_REGISTER_GUI(DummyMessageDialog, sight::ui::base::dialog::IMessageDialog::REGISTRY_KEY);

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
    // Create a temporary directory
    const auto tmpfolder = core::tools::System::getTemporaryFolder("BasicTest");
    std::filesystem::remove_all(tmpfolder);
    std::filesystem::create_directories(tmpfolder);
    const auto testPath = tmpfolder / "powder.perlimpinpin";

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
        writer->setFile(testPath);

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
    CPPUNIT_ASSERT(std::filesystem::exists(testPath) && std::filesystem::is_regular_file(testPath));

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
        reader->setFile(testPath);

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
    // Create a temporary directory
    const auto tmpfolder = core::tools::System::getTemporaryFolder("FileDialogTest");
    std::filesystem::remove_all(tmpfolder);
    std::filesystem::create_directories(tmpfolder);
    const auto testPath = tmpfolder / "powder.perlimpinpin";

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

        sight::ui::base::dialog::DummyLocationDialog::setPaths({testPath});

        writer->update().wait();
        writer->stop().wait();

        // Cleanup
        service::unregisterService(writer);
    }

    // The file should have been created
    CPPUNIT_ASSERT(std::filesystem::exists(testPath) && std::filesystem::is_regular_file(testPath));

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

        sight::ui::base::dialog::DummyLocationDialog::setPaths({testPath});

        reader->update().wait();
        reader->stop().wait();

        // Cleanup
        service::unregisterService(reader);

        // Final test
        CPPUNIT_ASSERT_EQUAL(expected, outString->getValue());
    }
}

//------------------------------------------------------------------------------

void SessionTest::passwordTest()
{
    // Create a temporary directory
    const auto tmpfolder = core::tools::System::getTemporaryFolder("BasicTest");
    std::filesystem::remove_all(tmpfolder);
    std::filesystem::create_directories(tmpfolder);
    const auto testPath = tmpfolder / "powder.perlimpinpin";

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
        writer->setFile(testPath);

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
    }

    // The file should have been created
    CPPUNIT_ASSERT(std::filesystem::exists(testPath) && std::filesystem::is_regular_file(testPath));

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
        reader->setFile(testPath);

        // Configure the reader service
        auto config = getConfiguration(true);
        config.put("password.<xmlattr>.policy", "always");
        reader->setConfiguration(config);
        reader->configure();

        // Execute the writer service
        reader->start().wait();

        sight::ui::base::dialog::DummyInputDialog::pushInput("Oops");
        DummyMessageDialog::pushAction(sight::ui::base::dialog::IMessageDialog::RETRY);
        sight::ui::base::dialog::DummyInputDialog::pushInput("I forgot");
        DummyMessageDialog::pushAction(sight::ui::base::dialog::IMessageDialog::RETRY);
        sight::ui::base::dialog::DummyInputDialog::pushInput("Wait I remember");
        DummyMessageDialog::pushAction(sight::ui::base::dialog::IMessageDialog::RETRY);
        sight::ui::base::dialog::DummyInputDialog::pushInput("case-sensitive");

        reader->update().wait();
        reader->stop().wait();

        // Cleanup
        service::unregisterService(reader);

        // Final test
        CPPUNIT_ASSERT_EQUAL(expected, outString->getValue());
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::session::ut
