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
#include <service/IService.hxx>
#include <service/op/Add.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::session::ut::SessionTest);

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
        service::OSR::unregisterService(writer);
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
        service::OSR::unregisterService(reader);

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

} // namespace sight::module::io::session::ut
