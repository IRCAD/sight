/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "core/config.hpp"
#include "core/crypto/secure_string.hpp"

#include <filesystem>
#include <iostream>
#include <mutex>
#include <string>

namespace sight::core::log
{

// Default log file name inside the archive
constexpr static auto LOG_FILE           = "sight.log";
constexpr static auto ENCRYPTED_LOG_FILE = "sight.log.zip";

/**
 * @brief Implements the spy_logger.
 *
 * This class setup boost logging. First, man need to call `add_console_log()` or `add_file_log()` or
 * `start_encrypted_logger()`, to add a "sink" to boost logging. Then, it is possible to use `trace()`, ... ,`fatal()`
 * to write log message, depending of the severity.
 *
 * When using `start_encrypted_logger()`, the log file is encrypted using a separated detached child process, that will
 * produce a ZSTD compressed ZIP file with an encrypted SLM.log file inside.
 */
class spy_logger
{
public:

    enum level_type
    {
        SL_TRACE,
        SL_DEBUG,
        SL_INFO,
        SL_WARN,
        SL_ERROR,
        SL_FATAL
    };

    /// Adds a console sink to the logger from the given output stream.
    /// @param os The output stream to use.
    /// @param level The minimum level to log.
    static CORE_API void add_console_log(std::ostream& os = std::clog, level_type level = SL_TRACE);

    /// Adds a file sink to the logger.
    /// @param path The path to the log file.
    /// @param level The minimum level to log.
    static CORE_API void add_file_log(const std::filesystem::path& log_file = LOG_FILE, level_type level = SL_TRACE);

    /// Adds an encrypted file sink to the logger. It will start a separated detached child process that will perform
    /// the encryption. The real file name will have an extension suffix with an index counter which will be incremented
    /// each time the password change. For example, with an initial "sight.log.0.zip", if the password is changed,
    /// the file name will be sight.log.1.zip, and so one.
    /// @param log_archive The archive name "template".
    /// @param level The minimum level to log.
    /// @param password The password to use to encrypt the log.
    /// @param ask_password If true, the password will be asked to the user.
    CORE_API void start_encrypted_logger(
        const std::filesystem::path& log_archive    = ENCRYPTED_LOG_FILE,
        level_type level                            = SL_TRACE,
        const core::crypto::secure_string& password = "",
        bool ask_password                           = false
    );

    /// Adds an file sink to the logger. It will start a separated detached child process that will perform
    /// the logging.
    /// @param log_archive The archive name "template".
    /// @param level The minimum level to log.
    CORE_API void start_logger(
        const std::filesystem::path& log_archive = LOG_FILE,
        level_type level                         = SL_TRACE
    );

    /// Close the current logger.
    CORE_API void stop_logger();

    /// Change the password used for log file encryption. It will close the current encrypted file and open a new one
    /// named with an incremented index counter (like in `log.1.sight`, `log.2.sight`, ...), depending of the original
    /// log archive name.
    /// Has no effect if the log file is not encrypted.
    /// @param password the new password
    /// @param old_password the old password
    CORE_API void change_log_password(
        const core::crypto::secure_string& password,
        const core::crypto::secure_string& old_password = ""
    );

    /// Relocate the log to a new path. It will close the current log file and open a new one. If `copy_previous_log` is
    /// true, the previous log file will be copied to the new path.
    /// @param[in] new_path the new path of the log file.
    /// @param[in] password the password used to optionally encrypt the log file.
    /// @param[in] relocate_previous_logs if true, the previous log files will be merged into a new log in the new path.
    /// @param[in] old_password the old password, if relevant. This will allow decrypting the previous log files.
    CORE_API void relocate_log(
        const std::filesystem::path& new_path,
        const core::crypto::secure_string& password     = "",
        bool relocate_previous_logs                     = false,
        const core::crypto::secure_string& old_password = ""
    );

    /// Returns true if the current log file is encrypted. IE if there is a child sightlog process running.
    CORE_API bool is_log_encrypted() const;

    /// Returns true if the current log file is encrypted. IE if there is a child sightlog process running.
    CORE_API std::filesystem::path get_current_log_path() const;

    static CORE_API void trace(const std::string& mes, const char* file = nullptr, int line = -1);

    static CORE_API void debug(const std::string& mes, const char* file = nullptr, int line = -1);

    static CORE_API void info(const std::string& mes, const char* file = nullptr, int line = -1);

    static CORE_API void warn(const std::string& mes, const char* file = nullptr, int line = -1);

    static CORE_API void error(const std::string& mes, const char* file = nullptr, int line = -1);

    static CORE_API void fatal(const std::string& mes, const char* file = nullptr, int line = -1);

    /// Returns the singleton instance.
    static CORE_API spy_logger& get();

    /// Returns the path of the current sightlog executable.
    static CORE_API std::filesystem::path get_logger_path();

private:

    CORE_API spy_logger();
    CORE_API ~spy_logger();

    class spy_logger_impl;
    std::unique_ptr<spy_logger_impl> m_pimpl;
}; // spy_logger

} // namespace sight::core::log
