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

#include <core/crypto/sha256.hpp>
#include <core/log/spy_logger.hpp>
#include <core/os/temp_path.hpp>
#include <core/spy_log.hpp>
#include <core/tools/uuid.hpp>

#include <doctest/doctest.h>

#include <openssl/err.h>
#include <openssl/evp.h>

#include <zstd.h>

#include <array>
#include <filesystem>
#include <regex>

TEST_SUITE("sight::core::log::encrypted_log")
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
        const std::optional<std::reference_wrapper<const sight::core::crypto::secure_string> >& _password = std::nullopt,
        bool _bad_password                                                                                = false,
        bool _truncated                                                                                   = false
)
    {
        std::filesystem::path raw_log_path;
        sight::core::os::temp_dir temp_dir;

        if(_password)
        {
            // Create the raw log
            raw_log_path = temp_dir / "raw.log";
            std::ofstream raw_stream_output(raw_log_path);

            CHECK_MESSAGE(raw_stream_output.good(), raw_log_path, " doesn't exist or cannot be opened.");

            // Open the encrypted/compressed log
            std::ifstream log_stream_input(_log_path, std::ios::binary);

            CHECK_MESSAGE(log_stream_input.good(), _log_path, " doesn't exist or cannot be opened.");

            // Extract the log
            if(_bad_password)
            {
                CHECK_THROWS_AS(
                    sight::core::log::spy_logger::extract(log_stream_input, raw_stream_output, _password),
                    sight::core::log::spy_logger::bad_password
                );

                // No need to check the content
                return;
            }

            if(!_truncated)
            {
                CHECK_NOTHROW(sight::core::log::spy_logger::extract(log_stream_input, raw_stream_output, _password));
            }
            else
            {
                CHECK_THROWS_AS(
                    sight::core::log::spy_logger::extract(log_stream_input, raw_stream_output, _password),
                    sight::core::log::spy_logger::premature_end
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

        REQUIRE_MESSAGE(std::filesystem::exists(raw_log_path), raw_log_path, " doesn't exist.");
        CHECK_MESSAGE(raw_stream_input.good(), raw_log_path, " cannot be opened.");

        for(const auto& message : _messages)
        {
            std::string line;
            std::getline(raw_stream_input, line);

            const auto& smatch = parse_log_line(line);

            CHECK_EQ(message, smatch[8].str());

            // Also test source path stripping from strip_source_path()
            const auto& stored_path   = (std::filesystem::path(SIGHT_SOURCE_DIR) / smatch[6].str()).lexically_normal();
            const auto& expected_path = std::filesystem::path(__FILE__).lexically_normal();

            CHECK_EQ(expected_path, stored_path);
        }
    }

//------------------------------------------------------------------------------

    inline static std::unique_ptr<sight::core::os::temp_dir> setup_log(
        const std::optional<std::reference_wrapper<const sight::core::crypto::secure_string> >& _password = std::nullopt,
        bool _asynchronous                                                                                = false
)
    {
        // Create a temporary directory
        auto temp_dir       = std::make_unique<sight::core::os::temp_dir>();
        const auto log_path = *temp_dir
                              / (_password ? sight::core::log::ENCRYPTED_LOG_FILE : sight::core::log::LOG_FILE);

        CHECK_NOTHROW(
            sight::core::log::g_logger.start(
                log_path,
                sight::core::log::spy_logger::level_t::trace,
                _password,
                _asynchronous
            )
        );

        const auto real_log_path = sight::core::log::g_logger.get_current_log_path();

        REQUIRE_MESSAGE(std::filesystem::exists(real_log_path), real_log_path, " doesn't exist.");
        CHECK(std::filesystem::is_regular_file(real_log_path));

        return temp_dir;
    }

//------------------------------------------------------------------------------

    inline static void stop_logger(bool _check = true)
    {
        // Log archive name is computed in start_encrypted_logger()
        const auto& log_path = sight::core::log::g_logger.get_current_log_path();

        // This will remove the sink
        CHECK_NOTHROW(sight::core::log::g_logger.stop());

        if(_check && !log_path.empty())
        {
            REQUIRE_MESSAGE(std::filesystem::exists(log_path), log_path, " doesn't exist.");
            CHECK(std::filesystem::is_regular_file(log_path));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("log_without_sink")
    {
        // To be sure there is no logger still running
        stop_logger(false);

        CHECK_NOTHROW(
            sight::core::log::g_logger.trace(
                sight::core::tools::uuid::generate(),
                SIGHT_SOURCE_FILE,
                __LINE__
            )
        );
    }

//------------------------------------------------------------------------------

    TEST_CASE("nominal_raw_log")
    {
        const std::array messages = {
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate()
        };

        // Start logger
        decltype(setup_log()) temp_dir;
        CHECK_NOTHROW(temp_dir = setup_log());

        // Write a couple of simple trace messages
        const auto& log_path = sight::core::log::g_logger.get_current_log_path();

        for(const auto& message : messages)
        {
            CHECK_NOTHROW(sight::core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
        }

        // Final cleanup
        CHECK_NOTHROW(stop_logger());

        // Check the log content
        CHECK_NOTHROW(check_log(log_path, messages));
    }

//------------------------------------------------------------------------------

    TEST_CASE("nominal_compressed_log")
    {
        const std::array messages = {
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate()
        };

        // Start logger - empty password means compressed log
        sight::core::crypto::secure_string empty_password;
        decltype(setup_log()) temp_dir;
        CHECK_NOTHROW(temp_dir = setup_log(empty_password));

        // Write a couple of simple trace messages
        const auto& log_path = sight::core::log::g_logger.get_current_log_path();

        for(const auto& message : messages)
        {
            CHECK_NOTHROW(sight::core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
        }

        // Final cleanup
        CHECK_NOTHROW(stop_logger());

        // Check the log content
        CHECK_NOTHROW(check_log(log_path, messages, empty_password));
    }

//------------------------------------------------------------------------------

    TEST_CASE("nominal_encrypted_log")
    {
        const std::array messages = {
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate()
        };

        // Start logger
        const sight::core::crypto::secure_string password(PASSWORD);
        decltype(setup_log()) temp_dir;
        CHECK_NOTHROW(temp_dir = setup_log(password));

        // Write a couple of simple trace messages
        const auto& log_path = sight::core::log::g_logger.get_current_log_path();

        for(const auto& message : messages)
        {
            CHECK_NOTHROW(sight::core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
        }

        // Final cleanup
        CHECK_NOTHROW(stop_logger());

        // Check the log content
        CHECK_NOTHROW(check_log(log_path, messages, password));
    }

//------------------------------------------------------------------------------

    TEST_CASE("multiple_logger")
    {
        // Create a temporary directory
        auto temp_dir = sight::core::os::temp_dir();

        const auto log_1_path         = temp_dir / "log_1.log";
        auto logger_1                 = sight::core::log::spy_logger::make(log_1_path);
        const auto current_log_1_path = logger_1->get_current_log_path();

        const std::array messages_1 = {
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate()
        };

        for(const auto& message : messages_1)
        {
            CHECK_NOTHROW(logger_1->trace(message, SIGHT_SOURCE_FILE, __LINE__));
        }

        const auto log_2_path = temp_dir / "log_2.log";
        auto logger_2         = sight::core::log::spy_logger::make(
            log_2_path,
            sight::core::log::spy_logger::level_t::error
        );
        const auto current_log_2_path = logger_2->get_current_log_path();

        // Those messages should be ignored since sight::core::log::spy_logger::level_t::error >
        // sight::core::log::spy_logger::level_t::trace
        for(const auto& message : messages_1)
        {
            CHECK_NOTHROW(logger_2->trace(message, SIGHT_SOURCE_FILE, __LINE__));
        }

        // ...But not those
        const std::array messages_2 = {
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate()
        };

        for(const auto& message : messages_2)
        {
            CHECK_NOTHROW(logger_2->error(message, SIGHT_SOURCE_FILE, __LINE__));
        }

        // Test asynchronous writing
        const auto log_3_path = temp_dir / "log_3.log";
        auto logger_3         = sight::core::log::spy_logger::make(
            log_3_path,
            sight::core::log::spy_logger::level_t::debug,
            std::nullopt,
            true
        );
        const auto current_log_3_path = logger_3->get_current_log_path();

        const std::array messages_3 = {
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate()
        };

        for(const auto& message : messages_3)
        {
            CHECK_NOTHROW(logger_3->error(message, SIGHT_SOURCE_FILE, __LINE__));
        }

        // Test asynchronous writing with encryption
        const auto log_4_path = temp_dir / "log_4.log.aes";
        const sight::core::crypto::secure_string password(PASSWORD);
        auto logger_4 = sight::core::log::spy_logger::make(
            log_4_path,
            sight::core::log::spy_logger::level_t::debug,
            password,
            true
        );
        const auto current_log_4_path = logger_4->get_current_log_path();

        const std::array messages_4 = {
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate()
        };

        for(const auto& message : messages_4)
        {
            CHECK_NOTHROW(logger_4->error(message, SIGHT_SOURCE_FILE, __LINE__));
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

    TEST_CASE("bad_path")
    {
#ifdef WIN32
        constexpr auto path = "<<[{:}]>>";
#else
        constexpr auto path = "/proc/cpuinfo";
#endif

        const sight::core::crypto::secure_string password(PASSWORD);
        CHECK_THROWS_AS(
            sight::core::log::g_logger.start(path, sight::core::log::spy_logger::level_t::trace, password),
            std::runtime_error
        );

        // Since we test a bad path, we need to reset the logger
        CHECK_NOTHROW(sight::core::log::g_logger.stop());

        // Final cleanup
        CHECK_NOTHROW(stop_logger());
    }

//------------------------------------------------------------------------------

    TEST_CASE("bad_password")
    {
        const std::array messages = {
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate()
        };

        // Start logger
        const sight::core::crypto::secure_string password(PASSWORD);
        decltype(setup_log()) temp_dir;
        CHECK_NOTHROW(temp_dir = setup_log(password));

        // Write a couple of simple trace messages
        const auto& log_path = sight::core::log::g_logger.get_current_log_path();

        for(const auto& message : messages)
        {
            CHECK_NOTHROW(sight::core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
        }

        // Final cleanup
        CHECK_NOTHROW(stop_logger());

        // Check the log content
        CHECK_NOTHROW(check_log(log_path, messages, password));

        // Check the log content with a bad password
        sight::core::crypto::secure_string bad_password("This_is_a_bad_password");
        CHECK_NOTHROW(check_log(log_path, messages, bad_password, true));
    }

//------------------------------------------------------------------------------

    TEST_CASE("corner_case")
    {
        sight::core::os::temp_dir temp_dir;

        CHECK_NOTHROW(
            sight::core::log::g_logger.start(
                temp_dir,
                sight::core::log::spy_logger::level_t::trace
            )
        );

        const sight::core::crypto::secure_string password(PASSWORD);
        sight::core::log::g_logger.relocate(temp_dir / (sight::core::tools::uuid::generate() + ".log.aes"), password);
        const auto& log_path = sight::core::log::g_logger.get_current_log_path();

        const std::array messages_1 = {
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate()
        };

        // Write a couple of simple trace messages
        for(const auto& message : messages_1)
        {
            CHECK_NOTHROW(sight::core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
        }

        // Final cleanup
        CHECK_NOTHROW(stop_logger());

        // Check the log content.
        CHECK_NOTHROW(check_log(log_path, messages_1, password));
    }

//------------------------------------------------------------------------------

    TEST_CASE("raw_to_encrypted")
    {
        const std::array messages_1 = {
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate()
        };

        // Start logger - without password means raw log
        decltype(setup_log()) temp_dir;
        CHECK_NOTHROW(temp_dir = setup_log());

        // Write a couple of simple trace messages
        for(const auto& message : messages_1)
        {
            CHECK_NOTHROW(sight::core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
        }

        // Change the password
        const sight::core::crypto::secure_string password(PASSWORD);
        const auto& relocated_log_path = sight::core::log::g_logger.change_password(password);
        const auto& new_log_path       = sight::core::log::g_logger.get_current_log_path();

        // Write another couple of simple trace messages
        const std::array messages_2 = {
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate()
        };

        for(const auto& message : messages_2)
        {
            CHECK_NOTHROW(sight::core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
        }

        // Final cleanup
        CHECK_NOTHROW(stop_logger());

        // Check the log content. Both files should be decrypted with the new password
        CHECK_NOTHROW(check_log(relocated_log_path, messages_1, password));
        CHECK_NOTHROW(check_log(new_log_path, messages_2, password));
    }

//------------------------------------------------------------------------------

    TEST_CASE("compressed_to_encrypted")
    {
        const std::array messages_1 = {
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate()
        };

        // Start logger - with empty password means compressed log
        const sight::core::crypto::secure_string empty_password;
        decltype(setup_log()) temp_dir;
        CHECK_NOTHROW(temp_dir = setup_log(empty_password));

        // Write a couple of simple trace messages
        for(const auto& message : messages_1)
        {
            CHECK_NOTHROW(sight::core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
        }

        // Change the password
        const sight::core::crypto::secure_string password(PASSWORD);
        const auto& relocated_log_path = sight::core::log::g_logger.change_password(password, empty_password);
        const auto& new_log_path       = sight::core::log::g_logger.get_current_log_path();

        // Write another couple of simple trace messages
        const std::array messages_2 = {
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate()
        };

        for(const auto& message : messages_2)
        {
            CHECK_NOTHROW(sight::core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
        }

        // Final cleanup
        CHECK_NOTHROW(stop_logger());

        // Check the log content. Both files should be decrypted with the new password
        CHECK_NOTHROW(check_log(relocated_log_path, messages_1, password));
        CHECK_NOTHROW(check_log(new_log_path, messages_2, password));
    }

//------------------------------------------------------------------------------

    TEST_CASE("password_change")
    {
        const std::array messages_1 = {
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate()
        };

        // Start logger
        const sight::core::crypto::secure_string password(PASSWORD);
        decltype(setup_log()) temp_dir;
        CHECK_NOTHROW(temp_dir = setup_log(password));

        // Write a couple of simple trace messages
        for(const auto& message : messages_1)
        {
            CHECK_NOTHROW(sight::core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
        }

        // Change the password
        const sight::core::crypto::secure_string new_password("This_is_a_new_password");
        const auto& relocated_log_path = sight::core::log::g_logger.change_password(new_password, password);
        const auto& new_log_path       = sight::core::log::g_logger.get_current_log_path();

        // Write another couple of simple trace messages
        const std::array messages_2 = {
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate()
        };

        for(const auto& message : messages_2)
        {
            CHECK_NOTHROW(sight::core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
        }

        // Final cleanup
        CHECK_NOTHROW(stop_logger());

        // Check the log content. Both files should be decrypted with the new password
        CHECK_NOTHROW(check_log(relocated_log_path, messages_1, new_password));
        CHECK_NOTHROW(check_log(new_log_path, messages_2, new_password));
    }

//------------------------------------------------------------------------------

    TEST_CASE("relocate")
    {
        const std::array messages_1 = {
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate()
        };

        // Start logger
        const sight::core::crypto::secure_string password(PASSWORD);
        decltype(setup_log()) temp_dir;
        CHECK_NOTHROW(temp_dir = setup_log(password));
        const auto old_log_path = sight::core::log::g_logger.get_current_log_path();

        // Write a couple of simple trace messages
        for(const auto& message : messages_1)
        {
            CHECK_NOTHROW(sight::core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
        }

        // Relocate the log
        auto new_temp_dir   = std::make_unique<sight::core::os::temp_dir>();
        const auto new_path = *new_temp_dir / sight::core::log::ENCRYPTED_LOG_FILE;
        const sight::core::crypto::secure_string new_password("This_is_a_new_password");

        const auto& relocated_log_path = sight::core::log::g_logger.relocate(new_path, new_password, password);
        const auto& new_log_path       = sight::core::log::g_logger.get_current_log_path();

        // Verify that the old log has been removed
        CHECK(old_log_path != relocated_log_path);
        CHECK_EQ(false, std::filesystem::exists(old_log_path));

        // Write another couple of simple trace messages
        const std::array messages_2 = {
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate(),
            sight::core::tools::uuid::generate()
        };

        for(const auto& message : messages_2)
        {
            CHECK_NOTHROW(sight::core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
        }

        // Final cleanup
        CHECK_NOTHROW(stop_logger());

        // Check the log content. Both files should be decrypted with the new password
        CHECK_NOTHROW(check_log(relocated_log_path, messages_1, new_password));
        CHECK_NOTHROW(check_log(new_log_path, messages_2, new_password));
    }

//------------------------------------------------------------------------------

    TEST_CASE("crash")
    {
        std::vector<std::string> messages;
        messages.reserve(1000);

        for(std::size_t i = 0 ; i < 1000 ; ++i)
        {
            messages.emplace_back(sight::core::tools::uuid::generate());
        }

        // Start logger - worst case scenario: encrypted and asynchronous log
        const sight::core::crypto::secure_string password(PASSWORD);
        decltype(setup_log()) temp_dir;
        CHECK_NOTHROW(temp_dir = setup_log(password, true));

        // Store the log path
        const auto& log_path = sight::core::log::g_logger.get_current_log_path();

        // Write 100 of simple trace messages
        for(const auto& message : messages)
        {
            CHECK_NOTHROW(sight::core::log::g_logger.trace(message, SIGHT_SOURCE_FILE, __LINE__));
        }

        // Stop and truncate the log file to simulate a crash
        CHECK_NOTHROW(stop_logger());

        const auto file_size = std::filesystem::file_size(log_path);
        std::filesystem::resize_file(log_path, file_size - (10 * file_size / 100));

        // The log file is truncated, some messages will be lost, depending of the zstd block size, the size of the
        // data...
        // IF zstd < 1.5, ZSTD_e_flush doesn't finish the block, meaning we may loose more as the block is always big.
        // That's why we stay conservative here.
        messages.resize(100);

        // Check the log content
        CHECK_NOTHROW(check_log(log_path, messages, password, false, true));
    }
} // TEST_SUITE
