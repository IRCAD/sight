/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#pragma once

#include <sight/core/config.hpp>

#include "core/crypto/secure_string.hpp"

#include <boost/log/trivial.hpp>

#include <filesystem>
#include <iostream>
#include <optional>

namespace sight::core::log::detail
{

class spy_logger_impl;

}
namespace sight::core::log
{

// Default log file name inside the archive
constexpr static auto LOG_FILE           = "sight.log";
constexpr static auto ENCRYPTED_LOG_FILE = "sight.log.aes";

/**
 * @brief Implements the spy_logger log system. The spy_logger class is a wrapper around the boost logging system.
 *
 * The global logging is setup from `profile_launcher` and `cppunit_main` for unit tests.
 * `profile_launcher` can use the static `add_global_console_log()` which sets up a simple global console sink or the
 * more advanced `start()` on the global spy_logger instance `sight::core::log::g_logger` which allows log file
 * compression (ZSTD) and encryption (AES256) with a non empty password. Setting up a sink, makes the log macros
 * (SIGHT_DEBUG, SIGHT_ERROR, ...) to output log messages on console or file.
 * `cppunit_main` only uses the static `add_global_file_log()` to setup a simple global `FWTest.log` file sink, which
 * is useful for debugging.
 *
 * It is also possible to setup a local private spy_logger with the static `make()` function. This is useful to bypass
 * the global logging system and to have a private log file. Of course, log macros cannot be used, and direct calls to
 * the `trace()`, `debug()`, ... functions are needed.
 */
class SIGHT_CORE_CLASS_API spy_logger
{
public:

    /// The log level.
    enum class SIGHT_CORE_CLASS_API level_t : std::uint8_t
    {
        trace   = boost::log::trivial::trace,
        debug   = boost::log::trivial::debug,
        info    = boost::log::trivial::info,
        warning = boost::log::trivial::warning,
        error   = boost::log::trivial::error,
        fatal   = boost::log::trivial::fatal
    };

    /// Thrown when the password is invalid.
    class SIGHT_CORE_CLASS_API bad_password : public std::runtime_error
    {
    public:

        SIGHT_CORE_API bad_password(const char* const what = "Invalid password.") :
            std::runtime_error(what)
        {
        }
    };

    /// Thrown when the file has been truncated.
    class SIGHT_CORE_CLASS_API premature_end : public std::runtime_error
    {
    public:

        SIGHT_CORE_API premature_end(const char* const what = "Premature end.") :
            std::runtime_error(what)
        {
        }
    };

    using optional_password_t = std::optional<std::reference_wrapper<const core::crypto::secure_string> >;

    /// Adds a console sink to the logger from the given output stream.
    /// @param _os The output stream to use.
    /// @param _level The minimum level to log.
    static SIGHT_CORE_API boost::shared_ptr<boost::log::sinks::sink> add_global_console_log(
        std::ostream& _os = std::clog,
        level_t _level    = level_t::trace
    );

    /// Adds a file sink to the logger.
    /// @param _log_file The path to the log file.
    /// @param _level The minimum level to log.
    static SIGHT_CORE_API boost::shared_ptr<boost::log::sinks::sink> add_global_file_log(
        const std::filesystem::path& _log_file = LOG_FILE,
        level_t _level                         = level_t::trace
    );

    /// Adds file sink to the logger.
    /// The real file name will have an extension suffix with an index counter which will be incremented
    /// each time the password change. For example, with an initial "sight.log.0.aes", if the password is changed,
    /// the file name will be sight.log.1.aes, and so on.
    /// @param _log_path The log file name "template".
    /// @param _level The minimum level to log.
    /// @param _password The password to use to encrypt the log. If std::nullopt is used (default), the log will be in
    ///                  clear text. Empty password will make the log unencrypted, but still compressed.
    /// @param _asynchronous If true, the log will be written asynchronously, which is arguably less safe, but a lot
    ///                      faster, since the log operation is no more blocking.
    SIGHT_CORE_API void start(
        const std::filesystem::path& _log_path,
        level_t _level                       = level_t::trace,
        const optional_password_t& _password = std::nullopt,
        bool _asynchronous                   = false
    );

    /// Close the current logger.
    SIGHT_CORE_API void stop();

    /// Change the password used for log file encryption. It will close the current encrypted file and open a new one
    /// named with an incremented index counter (like in `log.1.sight`, `log.2.sight`, ...), depending of the original
    /// log archive name.
    /// Has no effect if the log file is not encrypted.
    /// @param _password the new password
    /// @param _old_password the old password
    /// @return the relocated log file path
    SIGHT_CORE_API std::filesystem::path change_password(
        const core::crypto::secure_string& _password,
        const optional_password_t& _old_password = std::nullopt
    );

    /// Relocate the log to a new path. It will close the current log file and open a new one. If `copy_previous_log` is
    /// true, the previous log file will be copied to the new path.
    /// @param _new_path the new path of the log file.
    /// @param _password the password used to optionally encrypt the log file.
    /// @param _old_password the old password, if relevant. This will allow decrypting the previous log files.
    /// @return the relocated log file path
    SIGHT_CORE_API std::filesystem::path relocate(
        const std::filesystem::path& _new_path,
        const optional_password_t& _password     = std::nullopt,
        const optional_password_t& _old_password = std::nullopt
    );

    /// Returns true if the current log file is encrypted.
    SIGHT_CORE_API bool is_log_encrypted() const;

    /// Returns the current log file path.
    SIGHT_CORE_API std::filesystem::path get_current_log_path() const;

    /// Logs a message with the given severity level.
    /// @param _mes The message to log.
    /// @param _file The file name where the log is called.
    /// @param _line The line number where the log is called.
    /// @{
    SIGHT_CORE_API void trace(const std::string_view& _mes, const char* _file = nullptr, int _line = -1);
    SIGHT_CORE_API void debug(const std::string_view& _mes, const char* _file = nullptr, int _line = -1);
    SIGHT_CORE_API void info(const std::string_view& _mes, const char* _file  = nullptr, int _line = -1);
    SIGHT_CORE_API void warn(const std::string_view& _mes, const char* _file  = nullptr, int _line = -1);
    SIGHT_CORE_API void error(const std::string_view& _mes, const char* _file = nullptr, int _line = -1);
    SIGHT_CORE_API void fatal(const std::string_view& _mes, const char* _file = nullptr, int _line = -1);
    /// @}

    /// Holds the global instance. It may also be accessed from anywhere in the code with `sight::core::log::g_logger`.
    static spy_logger s_instance;

    /// Create a private logger shared pointer and start it with the given parameters.
    /// Destruction will stop the logger.
    /// @see start() for more details.
    [[nodiscard]] static SIGHT_CORE_API std::shared_ptr<spy_logger> make(
        const std::filesystem::path& _log_path,
        level_t _level                       = level_t::trace,
        const optional_password_t& _password = std::nullopt,
        bool _asynchronous                   = false
    );

    /// Extract an input stream content to the output stream.
    /// @param _input The input stream to read.
    /// @param _output The output stream to write.
    /// @param _password The password to use to decrypt.
    ///                  - std::nullopt means not encrypted.
    ///                  - empty password means compressed but not encrypted.
    static SIGHT_CORE_API std::ostream& extract(
        std::istream& _input,
        std::ostream& _output,
        const optional_password_t& _password = std::nullopt
    );

private:

    SIGHT_CORE_API spy_logger();
    SIGHT_CORE_API virtual ~spy_logger();

    std::unique_ptr<detail::spy_logger_impl> m_pimpl;
}; // spy_logger

// This is a shortcut to the global logger
extern SIGHT_CORE_API spy_logger& g_logger;

} // namespace sight::core::log
