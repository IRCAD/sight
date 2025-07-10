/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include "encrypted_log_test.hpp"

#include <core/crypto/sha256.hpp>
#include <core/log/spy_logger.hpp>
#include <core/os/temp_path.hpp>
#include <core/spy_log.hpp>
#include <core/tools/uuid.hpp>

#include <boost/dll.hpp>
#include <boost/log/core.hpp>
#include <boost/process.hpp>

#include <openssl/err.h>
#include <openssl/evp.h>

#include <zstd.h>

#include <array>
#include <filesystem>
#include <regex>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::log::ut::encrypted_log_test);

namespace sight::core::log::ut
{

constexpr static auto PASSWORD = "This_is_a_password";

//------------------------------------------------------------------------------

inline static std::smatch parse_log_line(const std::string& _line)
{
    static const std::regex s_REGEX =
        []
        {
            constexpr auto date      = R"(\[([^\]]*)\])";
            constexpr auto time      = R"(\[([^\]]*)\])";
            constexpr auto process   = R"(\[([^\]]*)\])";
            constexpr auto thread    = R"(\[([^\]]*)\])";
            constexpr auto level     = R"(\[([^\]]*)\] )";
            constexpr auto file_line = R"(\[([^\]]*):([^\]]*)\] )";
            constexpr auto message   = "(.*)";

            std::stringstream ss;
            ss << date << time << process << thread << level << file_line << message;

            return std::regex(ss.str());
        }();

    std::smatch match;

    std::regex_search(_line, match, s_REGEX);

    return match;
}

//------------------------------------------------------------------------------

inline static void check_log(
    const std::filesystem::path& _log_path,
    const auto& _messages,
    const std::optional<std::reference_wrapper<const core::crypto::secure_string> >& _password = std::nullopt,
    bool _bad_password                                                                         = false,
    bool _truncated                                                                            = false
)
{
    std::filesystem::path raw_log_path;
    core::os::temp_dir temp_dir;

    if(_password)
    {
        // Create the raw log
        raw_log_path = temp_dir / "raw.log";
        std::ofstream raw_stream_output(raw_log_path);

        CPPUNIT_ASSERT_MESSAGE(
            raw_log_path.string() + " doesn't exist or cannot be opened.",
            raw_stream_output.good()
        );

        // Open the encrypted/compressed log
        std::ifstream log_stream_input(_log_path, std::ios::binary);

        CPPUNIT_ASSERT_MESSAGE(
            _log_path.string() + " doesn't exist or cannot be opened.",
            log_stream_input.good()
        );

        // Extract the log
        if(_bad_password)
        {
            CPPUNIT_ASSERT_THROW(
                spy_logger::extract(log_stream_input, raw_stream_output, _password),
                spy_logger::bad_password
            );

            // No need to check the content
            return;
        }

        if(!_truncated)
        {
            CPPUNIT_ASSERT_NO_THROW(spy_logger::extract(log_stream_input, raw_stream_output, _password));
        }
        else
        {
            CPPUNIT_ASSERT_THROW(
                spy_logger::extract(log_stream_input, raw_stream_output, _password),
                spy_logger::premature_end
            );
        }
    }
    else
    {
        // Raw log
        raw_log_path = _log_path;
    }

    // open the log file
    std::ifstream raw_stream_input(raw_log_path.string());

    CPPUNIT_ASSERT_MESSAGE(
        raw_log_path.string() + " doesn't exist or cannot be opened.",
        std::filesystem::exists(raw_log_path) && raw_stream_input.good()
    );

    for(const auto& message : _messages)
    {
        std::string line;
        std::getline(raw_stream_input, line);

        const auto& smatch = parse_log_line(line);

        CPPUNIT_ASSERT_EQUAL(message, smatch[8].str());

        // Also test source path stripping from strip_source_path()
        const auto& stored_path   = (std::filesystem::path(SIGHT_SOURCE_DIR) / smatch[6].str()).lexically_normal();
        const auto& expected_path = std::filesystem::path(__FILE__).lexically_normal();

        CPPUNIT_ASSERT_EQUAL(expected_path, stored_path);
    }
}

//------------------------------------------------------------------------------

inline static std::unique_ptr<core::os::temp_dir> setup_log(
    const std::optional<std::reference_wrapper<const core::crypto::secure_string> >& _password = std::nullopt,
    bool _asynchronous                                                                         = false
)
{
    // Create a temporary directory
    auto temp_dir       = std::make_unique<core::os::temp_dir>();
    const auto log_path = *temp_dir / (_password ? ENCRYPTED_LOG_FILE : LOG_FILE);

    CPPUNIT_ASSERT_NO_THROW(
        core::log::g_logger.start(
            log_path,
            sight::core::log::spy_logger::level_t::trace,
            _password,
            _asynchronous
        )
    );

    const auto real_log_path = core::log::g_logger.get_current_log_path();

    CPPUNIT_ASSERT_MESSAGE(
        real_log_path.string() + " doesn't exist.",
        std::filesystem::exists(real_log_path) && std::filesystem::is_regular_file(real_log_path)
    );

    return temp_dir;
}

//------------------------------------------------------------------------------

inline static void stop_logger(bool _check = true)
{
    // Log archive name is computed in start_encrypted_logger()
    const auto& log_path = core::log::g_logger.get_current_log_path();

    // This will remove the sink
    CPPUNIT_ASSERT_NO_THROW(core::log::g_logger.stop());

    if(_check && !log_path.empty())
    {
        CPPUNIT_ASSERT_MESSAGE(
            log_path.string() + " doesn't exist.",
            std::filesystem::exists(log_path) && std::filesystem::is_regular_file(log_path)
        );
    }
}

//------------------------------------------------------------------------------

void encrypted_log_test::setUp()
{
}

//-----------------------------------------------------------------------------

void encrypted_log_test::tearDown()
{
    // To be sure there is no logger still running
    stop_logger(false);
}

//------------------------------------------------------------------------------

void encrypted_log_test::log_without_sink_test()
{
    CPPUNIT_ASSERT_NO_THROW(
        core::log::g_logger.trace(
            core::tools::uuid::generate(),
            SIGHT_SOURCE_FILE,
            __LINE__
        )
    );
}

//------------------------------------------------------------------------------

void encrypted_log_test::nominal_raw_log_test()
{
    const std::array messages = {
        core::tools::uuid::generate(),
        core::tools::uuid::generate(),
        core::tools::uuid::generate()
    };

    // Start logger
    decltype(setup_log()) temp_dir;
    CPPUNIT_ASSERT_NO_THROW(temp_dir = setup_log());

    // Write a couple of simple trace messages
    const auto& log_path = core::log::g_logger.get_current_log_path();

    for(const auto& message : messages)
    {
        CPPUNIT_ASSERT_NO_THROW(core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    // Final cleanup
    CPPUNIT_ASSERT_NO_THROW(stop_logger());

    // Check the log content
    CPPUNIT_ASSERT_NO_THROW(check_log(log_path, messages));
}

//------------------------------------------------------------------------------

void encrypted_log_test::nominal_compressed_log_test()
{
    const std::array messages = {
        core::tools::uuid::generate(),
        core::tools::uuid::generate(),
        core::tools::uuid::generate()
    };

    // Start logger - empty password means compressed log
    core::crypto::secure_string empty_password;
    decltype(setup_log()) temp_dir;
    CPPUNIT_ASSERT_NO_THROW(temp_dir = setup_log(empty_password));

    // Write a couple of simple trace messages
    const auto& log_path = core::log::g_logger.get_current_log_path();

    for(const auto& message : messages)
    {
        CPPUNIT_ASSERT_NO_THROW(core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    // Final cleanup
    CPPUNIT_ASSERT_NO_THROW(stop_logger());

    // Check the log content
    CPPUNIT_ASSERT_NO_THROW(check_log(log_path, messages, empty_password));
}

//------------------------------------------------------------------------------

void encrypted_log_test::nominal_encrypted_log_test()
{
    const std::array messages = {
        core::tools::uuid::generate(),
        core::tools::uuid::generate(),
        core::tools::uuid::generate()
    };

    // Start logger
    const core::crypto::secure_string password(PASSWORD);
    decltype(setup_log()) temp_dir;
    CPPUNIT_ASSERT_NO_THROW(temp_dir = setup_log(password));

    // Write a couple of simple trace messages
    const auto& log_path = core::log::g_logger.get_current_log_path();

    for(const auto& message : messages)
    {
        CPPUNIT_ASSERT_NO_THROW(core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    // Final cleanup
    CPPUNIT_ASSERT_NO_THROW(stop_logger());

    // Check the log content
    CPPUNIT_ASSERT_NO_THROW(check_log(log_path, messages, password));
}

//------------------------------------------------------------------------------

void encrypted_log_test::multiple_logger_test()
{
    // Create a temporary directory
    auto temp_dir = core::os::temp_dir();

    const auto log_1_path         = temp_dir / "log_1.log";
    auto logger_1                 = spy_logger::make(log_1_path);
    const auto current_log_1_path = logger_1->get_current_log_path();

    const std::array messages_1 = {
        core::tools::uuid::generate(),
        core::tools::uuid::generate(),
        core::tools::uuid::generate()
    };

    for(const auto& message : messages_1)
    {
        CPPUNIT_ASSERT_NO_THROW(logger_1->trace(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    const auto log_2_path         = temp_dir / "log_2.log";
    auto logger_2                 = spy_logger::make(log_2_path, spy_logger::level_t::error);
    const auto current_log_2_path = logger_2->get_current_log_path();

    // Those messages should be ignored since spy_logger::level_t::error > spy_logger::level_t::trace
    for(const auto& message : messages_1)
    {
        CPPUNIT_ASSERT_NO_THROW(logger_2->trace(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    // ...But not those
    const std::array messages_2 = {
        core::tools::uuid::generate(),
        core::tools::uuid::generate(),
        core::tools::uuid::generate()
    };

    for(const auto& message : messages_2)
    {
        CPPUNIT_ASSERT_NO_THROW(logger_2->error(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    // Test asynchronous writing
    const auto log_3_path         = temp_dir / "log_3.log";
    auto logger_3                 = spy_logger::make(log_3_path, spy_logger::level_t::debug, std::nullopt, true);
    const auto current_log_3_path = logger_3->get_current_log_path();

    const std::array messages_3 = {
        core::tools::uuid::generate(),
        core::tools::uuid::generate(),
        core::tools::uuid::generate()
    };

    for(const auto& message : messages_3)
    {
        CPPUNIT_ASSERT_NO_THROW(logger_3->error(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    // Test asynchronous writing with encryption
    const auto log_4_path = temp_dir / "log_4.log.aes";
    const core::crypto::secure_string password(PASSWORD);
    auto logger_4                 = spy_logger::make(log_4_path, spy_logger::level_t::debug, password, true);
    const auto current_log_4_path = logger_4->get_current_log_path();

    const std::array messages_4 = {
        core::tools::uuid::generate(),
        core::tools::uuid::generate(),
        core::tools::uuid::generate()
    };

    for(const auto& message : messages_4)
    {
        CPPUNIT_ASSERT_NO_THROW(logger_4->error(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    logger_1.reset();
    check_log(current_log_1_path, messages_1);

    logger_2.reset();
    check_log(current_log_2_path, messages_2);

    logger_3.reset();
    check_log(current_log_3_path, messages_3);

    logger_4.reset();
    check_log(current_log_4_path, messages_4, password);
}

//------------------------------------------------------------------------------

void encrypted_log_test::bad_path_test()
{
#ifdef WIN32
    constexpr auto path = "<<[{:}]>>";
#else
    constexpr auto path = "/proc/cpuinfo";
#endif

    const core::crypto::secure_string password(PASSWORD);
    CPPUNIT_ASSERT_THROW(
        core::log::g_logger.start(path, sight::core::log::spy_logger::level_t::trace, password),
        std::runtime_error
    );

    // Since we test a bad path, we need to reset the logger
    CPPUNIT_ASSERT_NO_THROW(core::log::g_logger.stop());

    // Final cleanup
    CPPUNIT_ASSERT_NO_THROW(stop_logger());
}

//------------------------------------------------------------------------------

void encrypted_log_test::bad_password_test()
{
    const std::array messages = {
        core::tools::uuid::generate(),
        core::tools::uuid::generate(),
        core::tools::uuid::generate()
    };

    // Start logger
    const core::crypto::secure_string password(PASSWORD);
    decltype(setup_log()) temp_dir;
    CPPUNIT_ASSERT_NO_THROW(temp_dir = setup_log(password));

    // Write a couple of simple trace messages
    const auto& log_path = core::log::g_logger.get_current_log_path();

    for(const auto& message : messages)
    {
        CPPUNIT_ASSERT_NO_THROW(core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    // Final cleanup
    CPPUNIT_ASSERT_NO_THROW(stop_logger());

    // Check the log content
    CPPUNIT_ASSERT_NO_THROW(check_log(log_path, messages, password));

    // Check the log content with a bad password
    core::crypto::secure_string bad_password("This_is_a_bad_password");
    CPPUNIT_ASSERT_NO_THROW(check_log(log_path, messages, bad_password, true));
}

//------------------------------------------------------------------------------

void encrypted_log_test::corner_case_test()
{
    core::os::temp_dir temp_dir;

    CPPUNIT_ASSERT_NO_THROW(
        core::log::g_logger.start(
            temp_dir,
            sight::core::log::spy_logger::level_t::trace
        )
    );

    const core::crypto::secure_string password(PASSWORD);
    core::log::g_logger.relocate(temp_dir / (core::tools::uuid::generate() + ".log.aes"), password);
    const auto& log_path = core::log::g_logger.get_current_log_path();

    const std::array messages_1 = {
        core::tools::uuid::generate(),
        core::tools::uuid::generate(),
        core::tools::uuid::generate()
    };

    // Write a couple of simple trace messages
    for(const auto& message : messages_1)
    {
        CPPUNIT_ASSERT_NO_THROW(core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    // Final cleanup
    CPPUNIT_ASSERT_NO_THROW(stop_logger());

    // Check the log content.
    CPPUNIT_ASSERT_NO_THROW(check_log(log_path, messages_1, password));
}

//------------------------------------------------------------------------------

void encrypted_log_test::raw_to_encrypted_test()
{
    const std::array messages_1 = {
        core::tools::uuid::generate(),
        core::tools::uuid::generate(),
        core::tools::uuid::generate()
    };

    // Start logger - without password means raw log
    decltype(setup_log()) temp_dir;
    CPPUNIT_ASSERT_NO_THROW(temp_dir = setup_log());

    // Write a couple of simple trace messages
    for(const auto& message : messages_1)
    {
        CPPUNIT_ASSERT_NO_THROW(core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    // Change the password
    const core::crypto::secure_string password(PASSWORD);
    const auto& relocated_log_path = core::log::g_logger.change_password(password);
    const auto& new_log_path       = core::log::g_logger.get_current_log_path();

    // Write another couple of simple trace messages
    const std::array messages_2 = {
        core::tools::uuid::generate(),
        core::tools::uuid::generate(),
        core::tools::uuid::generate()
    };

    for(const auto& message : messages_2)
    {
        CPPUNIT_ASSERT_NO_THROW(core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    // Final cleanup
    CPPUNIT_ASSERT_NO_THROW(stop_logger());

    // Check the log content. Both files should be decrypted with the new password
    CPPUNIT_ASSERT_NO_THROW(check_log(relocated_log_path, messages_1, password));
    CPPUNIT_ASSERT_NO_THROW(check_log(new_log_path, messages_2, password));
}

//------------------------------------------------------------------------------

void encrypted_log_test::compressed_to_encrypted_test()
{
    const std::array messages_1 = {
        core::tools::uuid::generate(),
        core::tools::uuid::generate(),
        core::tools::uuid::generate()
    };

    // Start logger - with empty password means compressed log
    const core::crypto::secure_string empty_password;
    decltype(setup_log()) temp_dir;
    CPPUNIT_ASSERT_NO_THROW(temp_dir = setup_log(empty_password));

    // Write a couple of simple trace messages
    for(const auto& message : messages_1)
    {
        CPPUNIT_ASSERT_NO_THROW(core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    // Change the password
    const core::crypto::secure_string password(PASSWORD);
    const auto& relocated_log_path = core::log::g_logger.change_password(password, empty_password);
    const auto& new_log_path       = core::log::g_logger.get_current_log_path();

    // Write another couple of simple trace messages
    const std::array messages_2 = {
        core::tools::uuid::generate(),
        core::tools::uuid::generate(),
        core::tools::uuid::generate()
    };

    for(const auto& message : messages_2)
    {
        CPPUNIT_ASSERT_NO_THROW(core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    // Final cleanup
    CPPUNIT_ASSERT_NO_THROW(stop_logger());

    // Check the log content. Both files should be decrypted with the new password
    CPPUNIT_ASSERT_NO_THROW(check_log(relocated_log_path, messages_1, password));
    CPPUNIT_ASSERT_NO_THROW(check_log(new_log_path, messages_2, password));
}

//------------------------------------------------------------------------------

void encrypted_log_test::password_change_test()
{
    const std::array messages_1 = {
        core::tools::uuid::generate(),
        core::tools::uuid::generate(),
        core::tools::uuid::generate()
    };

    // Start logger
    const core::crypto::secure_string password(PASSWORD);
    decltype(setup_log()) temp_dir;
    CPPUNIT_ASSERT_NO_THROW(temp_dir = setup_log(password));

    // Write a couple of simple trace messages
    for(const auto& message : messages_1)
    {
        CPPUNIT_ASSERT_NO_THROW(core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    // Change the password
    const core::crypto::secure_string new_password("This_is_a_new_password");
    const auto& relocated_log_path = core::log::g_logger.change_password(new_password, password);
    const auto& new_log_path       = core::log::g_logger.get_current_log_path();

    // Write another couple of simple trace messages
    const std::array messages_2 = {
        core::tools::uuid::generate(),
        core::tools::uuid::generate(),
        core::tools::uuid::generate()
    };

    for(const auto& message : messages_2)
    {
        CPPUNIT_ASSERT_NO_THROW(core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    // Final cleanup
    CPPUNIT_ASSERT_NO_THROW(stop_logger());

    // Check the log content. Both files should be decrypted with the new password
    CPPUNIT_ASSERT_NO_THROW(check_log(relocated_log_path, messages_1, new_password));
    CPPUNIT_ASSERT_NO_THROW(check_log(new_log_path, messages_2, new_password));
}

//------------------------------------------------------------------------------

void encrypted_log_test::relocate_test()
{
    const std::array messages_1 = {
        core::tools::uuid::generate(),
        core::tools::uuid::generate(),
        core::tools::uuid::generate()
    };

    // Start logger
    const core::crypto::secure_string password(PASSWORD);
    decltype(setup_log()) temp_dir;
    CPPUNIT_ASSERT_NO_THROW(temp_dir = setup_log(password));
    const auto old_log_path = core::log::g_logger.get_current_log_path();

    // Write a couple of simple trace messages
    for(const auto& message : messages_1)
    {
        CPPUNIT_ASSERT_NO_THROW(core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    // Relocate the log
    auto new_temp_dir   = std::make_unique<core::os::temp_dir>();
    const auto new_path = *new_temp_dir / ENCRYPTED_LOG_FILE;
    const core::crypto::secure_string new_password("This_is_a_new_password");

    const auto& relocated_log_path = core::log::g_logger.relocate(new_path, new_password, password);
    const auto& new_log_path       = core::log::g_logger.get_current_log_path();

    // Verify that the old log has been removed
    CPPUNIT_ASSERT(old_log_path != relocated_log_path);
    CPPUNIT_ASSERT_EQUAL(false, std::filesystem::exists(old_log_path));

    // Write another couple of simple trace messages
    const std::array messages_2 = {
        core::tools::uuid::generate(),
        core::tools::uuid::generate(),
        core::tools::uuid::generate()
    };

    for(const auto& message : messages_2)
    {
        CPPUNIT_ASSERT_NO_THROW(core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    // Final cleanup
    CPPUNIT_ASSERT_NO_THROW(stop_logger());

    // Check the log content. Both files should be decrypted with the new password
    CPPUNIT_ASSERT_NO_THROW(check_log(relocated_log_path, messages_1, new_password));
    CPPUNIT_ASSERT_NO_THROW(check_log(new_log_path, messages_2, new_password));
}

//------------------------------------------------------------------------------

void encrypted_log_test::crash_test()
{
    std::vector<std::string> messages;
    messages.reserve(1000);

    for(std::size_t i = 0 ; i < 1000 ; ++i)
    {
        messages.emplace_back(core::tools::uuid::generate());
    }

    // Start logger - worst case scenario: encrypted and asynchronous log
    const core::crypto::secure_string password(PASSWORD);
    decltype(setup_log()) temp_dir;
    CPPUNIT_ASSERT_NO_THROW(temp_dir = setup_log(password, true));

    // Store the log path
    const auto& log_path = core::log::g_logger.get_current_log_path();

    // Write 100 of simple trace messages
    for(const auto& message : messages)
    {
        CPPUNIT_ASSERT_NO_THROW(core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
    }

    // Stop and truncate the log file to simulate a crash
    CPPUNIT_ASSERT_NO_THROW(stop_logger());

    const auto file_size = std::filesystem::file_size(log_path);
    std::filesystem::resize_file(log_path, file_size - (10 * file_size / 100));

    // The log file is truncated, some messages will be lost, depending of the zstd block size, the size of the data...
    // IF zstd < 1.5, ZSTD_e_flush doesn't finish the block, meaning we may loose more as the block is always big.
    // That's why we stay conservative here.
    messages.resize(100);

    // Check the log content
    CPPUNIT_ASSERT_NO_THROW(check_log(log_path, messages, password, false, true));
}

} // namespace sight::core::log::ut
