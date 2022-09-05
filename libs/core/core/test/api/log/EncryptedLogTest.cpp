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

// cspell:ignore NOLINT

#include "EncryptedLogTest.hpp"

#include <core/crypto/Base64.hpp>
#include <core/log/SpyLogger.hpp>
#include <core/spyLog.hpp>
#include <core/tools/System.hpp>
#include <core/tools/UUID.hpp>

#include <boost/dll.hpp>
#include <boost/log/core.hpp>
#include <boost/process.hpp>

#include <array>
#include <filesystem>
#include <regex>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::log::ut::EncryptedLogTest);

namespace sight::core::log::ut
{

constexpr static auto PASSWORD = "This_is_a_password";

//------------------------------------------------------------------------------

inline static std::smatch parseLogLine(const std::string& line)
{
    static const std::regex regex =
        []
        {
            constexpr auto date_group      = R"(\[([^\]]*)\])";
            constexpr auto time_group      = R"(\[([^\]]*)\])";
            constexpr auto process_group   = R"(\[([^\]]*)\])";
            constexpr auto thread_group    = R"(\[([^\]]*)\])";
            constexpr auto level_group     = R"(\[([^\]]*)\] )";
            constexpr auto file_line_group = R"(\[([^\]]*):([^\]]*)\] )";
            constexpr auto message_group   = "(.*)";

            std::stringstream ss;
            ss << date_group << time_group << process_group << thread_group << level_group << file_line_group
            << message_group;

            return std::regex(ss.str());
        }();

    std::smatch match;

    std::regex_search(line, match, regex);

    return match;
}

//------------------------------------------------------------------------------

inline static std::filesystem::path decrypt(
    const std::filesystem::path& log_archive,
    const core::crypto::secure_string& password
)
{
    if(!std::filesystem::exists(log_archive) || !std::filesystem::is_regular_file(log_archive))
    {
        throw std::runtime_error("Log archive '" + log_archive.string() + "' doesn't exist.");
    }

    // Find the logger binary
    static const auto logger_path =
        []
        {
            decltype(boost::this_process::path()) bin_paths;
            bin_paths.push_back(boost::dll::program_location().remove_filename());
            return boost::process::search_path("sightlog", bin_paths);
        }();

    boost::process::ipstream remote_err;

    auto child = boost::process::child(
        logger_path,
        boost::process::args = {
            "-b",
            "-i",
            core::crypto::to_base64(log_archive.string()),
            "-p",
            core::crypto::to_base64(password).c_str(), // NOLINT(readability-redundant-string-cstr)
            "-d",
            core::crypto::to_base64(log_archive.parent_path().string())
        },
        boost::process::std_out > boost::process::null,
        boost::process::std_err > remote_err,
        boost::process::std_in < boost::process::null
    );

    // Wait for the logger to finish
    child.join();

    // Find the decrypted log
    if(child.exit_code() != 0)
    {
        std::stringstream ss;
        ss << "Sightlog error [" << child.exit_code() << "]: " << remote_err.rdbuf();
        throw std::runtime_error(ss.str());
    }

    const auto& log_file = log_archive.parent_path() / LOG_FILE;
    if(std::filesystem::exists(log_file) && std::filesystem::is_regular_file(log_file))
    {
        return log_file;
    }

    throw std::runtime_error("Failed to decrypt the log archive");
}

//------------------------------------------------------------------------------

inline static std::filesystem::path setupEncryptedLog()
{
    // Create a temporary directory
    const auto& tmp_folder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmp_folder);
    const auto& log_archive = tmp_folder / ENCRYPTED_LOG_FILE;
    std::filesystem::remove(log_archive);

    auto& log = core::log::SpyLogger::get();
    CPPUNIT_ASSERT_NO_THROW(log.start_encrypted_logger(log_archive, sight::core::log::SpyLogger::SL_TRACE, PASSWORD));

    const auto& real_log_archive = log.get_current_log_path();

    CPPUNIT_ASSERT_MESSAGE(
        real_log_archive.string() + " doesn't exist.",
        std::filesystem::exists(real_log_archive) && std::filesystem::is_regular_file(real_log_archive)
    );

    return real_log_archive;
}

//------------------------------------------------------------------------------

inline static void stopLogger()
{
    auto& log = core::log::SpyLogger::get();

    // Log archive name is computed in start_encrypted_logger()
    const auto& log_archive = log.get_current_log_path();

    // This will remove the sink and close the sightlog process
    log.stop_logger();

    if(!log_archive.empty())
    {
        CPPUNIT_ASSERT_MESSAGE(
            log_archive.string() + " doesn't exist.",
            std::filesystem::exists(log_archive) && std::filesystem::is_regular_file(log_archive)
        );
    }

    // To be sure in case one added a custom sink for testing purpose...
    boost::log::core::get()->remove_all_sinks();
}

//------------------------------------------------------------------------------

template<typename T>
inline static void testLogArchive(
    const std::filesystem::path& log_archive,
    const char* const password,
    const T& messages
)
{
    // Try to decrypt the first log archive
    std::filesystem::path decrypted_log_path;
    CPPUNIT_ASSERT_NO_THROW(decrypted_log_path = decrypt(log_archive, password));

    CPPUNIT_ASSERT_MESSAGE(
        decrypted_log_path.string() + " doesn't exist.",
        std::filesystem::exists(decrypted_log_path) && std::filesystem::is_regular_file(decrypted_log_path)
    );

    // Read the first decrypted log
    std::ifstream decrypted_stream(decrypted_log_path.string());

    for(const auto& message : messages)
    {
        std::string line;
        std::getline(decrypted_stream, line);

        const auto& smatch = parseLogLine(line);

        CPPUNIT_ASSERT_EQUAL(message, smatch[8].str());

        // Also test source path stripping from strip_source_path()
        const auto& reconstructed_path = (std::filesystem::path(SIGHT_SOURCE_DIR) / smatch[6].str()).lexically_normal();
        const auto& file_macro_path    = std::filesystem::path(__FILE__).lexically_normal();

        CPPUNIT_ASSERT_EQUAL(file_macro_path, reconstructed_path);
    }
}

//------------------------------------------------------------------------------

void EncryptedLogTest::setUp()
{
}

//-----------------------------------------------------------------------------

void EncryptedLogTest::tearDown()
{
}

//------------------------------------------------------------------------------

void EncryptedLogTest::log_without_sink_test()
{
    auto& log = core::log::SpyLogger::get();
    CPPUNIT_ASSERT_NO_THROW(log.trace(core::tools::UUID::generateUUID(), SIGHT_SOURCE_FILE, __LINE__));
}

//------------------------------------------------------------------------------

void EncryptedLogTest::nominal_test()
{
    // Start logger
    setupEncryptedLog();

    // Write a simple trace message
    auto& log = core::log::SpyLogger::get();
    CPPUNIT_ASSERT_NO_THROW(log.trace(core::tools::UUID::generateUUID(), SIGHT_SOURCE_FILE, __LINE__));

    // Final cleanup
    stopLogger();
}

//------------------------------------------------------------------------------

void EncryptedLogTest::bad_path_test()
{
#ifdef WIN32
    constexpr auto path = "<<[{:}]>>";
#else
    constexpr auto path = "/proc/cpuinfo";
#endif

    auto& log = core::log::SpyLogger::get();
    CPPUNIT_ASSERT_THROW(
        log.start_encrypted_logger(path, sight::core::log::SpyLogger::SL_TRACE, PASSWORD),
        std::runtime_error
    );

    // Since we test a bad path, we need to reset the logger
    log.stop_logger();

    // Final cleanup
    stopLogger();
}

//------------------------------------------------------------------------------

void EncryptedLogTest::basic_decryption_test()
{
    // Start logger
    setupEncryptedLog();

    // Write a simple trace message
    auto& log = core::log::SpyLogger::get();

    const std::array<const std::string, 3> messages = {
        core::tools::UUID::generateUUID(),
        core::tools::UUID::generateUUID(),
        core::tools::UUID::generateUUID()
    };

    // Write some log messages
    for(const auto& message : messages)
    {
        CPPUNIT_ASSERT_NO_THROW(log.trace(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    // Save the real log archive path
    const auto& log_archive = log.get_current_log_path();

    // This will remove the sink and close the sightlog process
    log.stop_logger();

    // Try to decrypt the log archive
    testLogArchive(log_archive, PASSWORD, messages);

    // Final cleanup
    stopLogger();
}

//------------------------------------------------------------------------------

void EncryptedLogTest::password_change_decryption_test()
{
    // Start logger
    setupEncryptedLog();

    // Write a simple trace message
    auto& log = core::log::SpyLogger::get();

    const std::array<const std::string, 3> first_messages = {
        core::tools::UUID::generateUUID(),
        core::tools::UUID::generateUUID(),
        core::tools::UUID::generateUUID()
    };

    const std::array<const std::string, 3> last_messages = {
        core::tools::UUID::generateUUID(),
        core::tools::UUID::generateUUID(),
        core::tools::UUID::generateUUID()
    };

    // Write some log messages
    for(const auto& message : first_messages)
    {
        CPPUNIT_ASSERT_NO_THROW(log.trace(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    // Save the first real log archive path
    const auto& first_log_archive = log.get_current_log_path();
    std::cerr << "First log archive: " << first_log_archive.string() << std::endl;

    // Change the current password, it will also close the current log archive
    constexpr static auto NEW_PASSWORD = "this_is_a_new_password";
    log.change_log_password(NEW_PASSWORD);

    // Write again some log messages
    for(const auto& message : last_messages)
    {
        CPPUNIT_ASSERT_NO_THROW(log.trace(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    // Save the last real log archive path
    const auto& last_log_archive = log.get_current_log_path();
    std::cerr << "Last log archive: " << last_log_archive.string() << std::endl;

    // This will remove the sink and close the sightlog process
    log.stop_logger();

    // Try to decrypt the first log archive
    testLogArchive(first_log_archive, PASSWORD, first_messages);

    // Try to decrypt the last log archive
    testLogArchive(last_log_archive, NEW_PASSWORD, last_messages);

    // Final cleanup
    stopLogger();
}

} // namespace sight::core::log::ut
