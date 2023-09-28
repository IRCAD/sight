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

// cspell:ignore NOLINT
// NOLINTBEGIN(clang-analyzer-optin.cplusplus.VirtualCall)

#include "encrypted_log_test.hpp"

#include <core/crypto/base64.hpp>
#include <core/log/spy_logger.hpp>
#include <core/os/temp_path.hpp>
#include <core/spy_log.hpp>
#include <core/tools/uuid.hpp>

#include <boost/dll.hpp>
#include <boost/log/core.hpp>
#include <boost/process.hpp>

#include <array>
#include <filesystem>
#include <regex>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::log::ut::encrypted_log_test);

namespace sight::core::log::ut
{

constexpr static auto PASSWORD = "This_is_a_password";

//------------------------------------------------------------------------------

inline static std::smatch parse_log_line(const std::string& line)
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
    const auto& logger_path = core::log::spy_logger::get_logger_path();

    boost::process::ipstream remote_err;

    auto child = boost::process::child(
        logger_path.string(),
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
        ss << "TEST: Sightlog error [" << child.exit_code() << "]: " << remote_err.rdbuf();
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

inline static std::filesystem::path setup_encrypted_log()
{
    // Create a temporary directory
    const auto& tmp_folder  = core::os::temp_dir::shared_directory();
    const auto& log_archive = tmp_folder / ENCRYPTED_LOG_FILE;
    std::filesystem::remove(log_archive);

    auto& log = core::log::spy_logger::get();
    CPPUNIT_ASSERT_NO_THROW(log.start_encrypted_logger(log_archive, sight::core::log::spy_logger::SL_TRACE, PASSWORD));

    const auto& real_log_archive = log.get_current_log_path();

    CPPUNIT_ASSERT_MESSAGE(
        real_log_archive.string() + " doesn't exist.",
        std::filesystem::exists(real_log_archive) && std::filesystem::is_regular_file(real_log_archive)
    );

    return real_log_archive;
}

//------------------------------------------------------------------------------

inline static std::filesystem::path setup_log()
{
    // Create a temporary directory
    const auto& tmp_folder  = core::os::temp_dir::shared_directory();
    const auto& log_archive = tmp_folder / LOG_FILE;
    std::filesystem::remove(log_archive);

    auto& log = core::log::spy_logger::get();
    CPPUNIT_ASSERT_NO_THROW(log.start_logger(log_archive, sight::core::log::spy_logger::SL_TRACE));

    const auto& real_log_archive = log.get_current_log_path();

    CPPUNIT_ASSERT_MESSAGE(
        real_log_archive.string() + " doesn't exist.",
        std::filesystem::exists(real_log_archive) && std::filesystem::is_regular_file(real_log_archive)
    );

    return real_log_archive;
}

//------------------------------------------------------------------------------

inline static void stop_logger()
{
    auto& log = core::log::spy_logger::get();

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
inline static void test_log_archive(
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

        const auto& smatch = parse_log_line(line);

        CPPUNIT_ASSERT_EQUAL(message, smatch[8].str());

        // Also test source path stripping from strip_source_path()
        const auto& reconstructed_path = (std::filesystem::path(SIGHT_SOURCE_DIR) / smatch[6].str()).lexically_normal();
        const auto& file_macro_path    = std::filesystem::path(__FILE__).lexically_normal();

        CPPUNIT_ASSERT_EQUAL(file_macro_path, reconstructed_path);
    }
}

//------------------------------------------------------------------------------

void encrypted_log_test::setUp()
{
}

//-----------------------------------------------------------------------------

void encrypted_log_test::tearDown()
{
}

//------------------------------------------------------------------------------

void encrypted_log_test::log_without_sink_test()
{
    auto& log = core::log::spy_logger::get();
    CPPUNIT_ASSERT_NO_THROW(log.trace(core::tools::UUID::generate(), SIGHT_SOURCE_FILE, __LINE__));
}

//------------------------------------------------------------------------------

void encrypted_log_test::nominal_test()
{
    // Start logger
    setup_encrypted_log();

    // Write a simple trace message
    auto& log = core::log::spy_logger::get();
    CPPUNIT_ASSERT_NO_THROW(log.trace(core::tools::UUID::generate(), SIGHT_SOURCE_FILE, __LINE__));

    // Final cleanup
    stop_logger();
}

//------------------------------------------------------------------------------

void encrypted_log_test::bad_path_test()
{
#ifdef WIN32
    constexpr auto path = "<<[{:}]>>";
#else
    constexpr auto path = "/proc/cpuinfo";
#endif

    auto& log = core::log::spy_logger::get();
    CPPUNIT_ASSERT_THROW(
        log.start_encrypted_logger(path, sight::core::log::spy_logger::SL_TRACE, PASSWORD),
        std::runtime_error
    );

    // Since we test a bad path, we need to reset the logger
    log.stop_logger();

    // Final cleanup
    stop_logger();
}

//------------------------------------------------------------------------------

void encrypted_log_test::basic_decryption_test()
{
    // Start logger
    setup_encrypted_log();

    // Write a simple trace message
    auto& log = core::log::spy_logger::get();

    const std::array<const std::string, 3> messages = {
        core::tools::UUID::generate(),
        core::tools::UUID::generate(),
        core::tools::UUID::generate()
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
    test_log_archive(log_archive, PASSWORD, messages);

    // Final cleanup
    stop_logger();
}

//------------------------------------------------------------------------------

void encrypted_log_test::password_change_decryption_test()
{
    // Start logger
    setup_encrypted_log();

    // Write a simple trace message
    auto& log = core::log::spy_logger::get();

    const std::array<const std::string, 3> first_messages = {
        core::tools::UUID::generate(),
        core::tools::UUID::generate(),
        core::tools::UUID::generate()
    };

    const std::array<const std::string, 3> last_messages = {
        core::tools::UUID::generate(),
        core::tools::UUID::generate(),
        core::tools::UUID::generate()
    };

    // Write some log messages
    for(const auto& message : first_messages)
    {
        CPPUNIT_ASSERT_NO_THROW(log.trace(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    // Change the current password, it will also close the current log archive
    constexpr static auto NEW_PASSWORD = "this_is_a_new_password";
    log.change_log_password(NEW_PASSWORD, PASSWORD);

    // Write again some log messages
    for(const auto& message : last_messages)
    {
        CPPUNIT_ASSERT_NO_THROW(log.trace(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    // Save the last real log archive path
    const auto& last_log_archive = log.get_current_log_path();

    // This will remove the sink and close the sightlog process
    log.stop_logger();

    // Try to decrypt the first log archive
    auto merged_log_archive = last_log_archive;
    merged_log_archive.replace_filename("sight.1.log.zip");
    test_log_archive(merged_log_archive, NEW_PASSWORD, first_messages);

    // Try to decrypt the last log archive
    test_log_archive(last_log_archive, NEW_PASSWORD, last_messages);

    // Final cleanup
    stop_logger();
}

//------------------------------------------------------------------------------

void encrypted_log_test::relocate_log_test()
{
    const std::array first_messages = {
        core::tools::UUID::generate(),
        core::tools::UUID::generate(),
        core::tools::UUID::generate()
    };

    const std::array next_messages = {
        core::tools::UUID::generate(),
        core::tools::UUID::generate(),
        core::tools::UUID::generate()
    };

    const std::array last_messages = {
        core::tools::UUID::generate(),
        core::tools::UUID::generate(),
        core::tools::UUID::generate()
    };

    const auto test =
        [&]
        {
            // Write some log messages
            auto& log = core::log::spy_logger::get();

            for(const auto& message : first_messages)
            {
                CPPUNIT_ASSERT_NO_THROW(log.trace(message, SIGHT_SOURCE_FILE, __LINE__));
            }

            // Relocate the log to a new directory (and relocate also current log archive)
            core::os::temp_dir first_temp_dir;
            log.relocate_log(first_temp_dir / "LOG-20230226-163932.zip", PASSWORD, true);

            const auto& merged_logs_path         = first_temp_dir / "LOG-20230226-163932.0.zip";
            const auto& first_relocated_log_path = first_temp_dir / "LOG-20230226-163932.1.zip";

            CPPUNIT_ASSERT(std::filesystem::exists(merged_logs_path));
            CPPUNIT_ASSERT(std::filesystem::exists(first_relocated_log_path));

            // Write again some log messages, in the new location
            for(const auto& message : next_messages)
            {
                CPPUNIT_ASSERT_NO_THROW(log.trace(message, SIGHT_SOURCE_FILE, __LINE__));
            }

            // Relocate the log a second time to a new directory (without relocating previous log archives)
            core::os::temp_dir second_temp_dir;
            log.relocate_log(second_temp_dir / "LOG-20230226-165223.zip", PASSWORD, false);

            const auto& second_relocated_log_path = second_temp_dir / "LOG-20230226-165223.0.zip";

            CPPUNIT_ASSERT(std::filesystem::exists(second_relocated_log_path));
            CPPUNIT_ASSERT(!std::filesystem::exists(second_temp_dir / "LOG-20230226-165223.1.zip"));

            // Write the last log messages, in the last location
            for(const auto& message : last_messages)
            {
                CPPUNIT_ASSERT_NO_THROW(log.trace(message, SIGHT_SOURCE_FILE, __LINE__));
            }

            // This will remove the sink and close the sightlog process
            log.stop_logger();

            // Try to decrypt the merged log archive
            test_log_archive(merged_logs_path, PASSWORD, first_messages);

            // Try to decrypt the second log archive
            test_log_archive(first_relocated_log_path, PASSWORD, next_messages);

            // Try to decrypt the last log archive
            test_log_archive(second_relocated_log_path, PASSWORD, last_messages);
        };

    // Test when starting clear log
    {
        setup_log();

        test();

        // Final cleanup
        stop_logger();
    }

    // Test when starting with encrypted log
    {
        // Start logger
        setup_encrypted_log();

        test();

        // Final cleanup
        stop_logger();
    }
}

} // namespace sight::core::log::ut

// NOLINTEND(clang-analyzer-optin.cplusplus.VirtualCall)
