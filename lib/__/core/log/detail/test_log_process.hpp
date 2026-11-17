/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace sight::core::log::detail::test_log
{

/// A Doctest test case, as given to the listener.
struct case_identity
{
    std::string_view suite;
    std::string_view name;
    std::string_view file;
    unsigned line {0};
};

/// Directories of the test logs, tried in this order.
struct location
{
    std::filesystem::path primary;
    std::optional<std::filesystem::path> fallback;
};

/// "<executable directory>/test-logs", then SIGHT_TEST_LOG_FALLBACK_DIR, or "<temp>/sight-test-logs" if it is not
/// set; the value "none" disables the fallback.
SIGHT_CORE_API location default_location();

/// Process log of a test executable: records outside test cases, and the prologue of every case log.
class SIGHT_CORE_CLASS_API process final
{
public:

    /// Opens the process log. If no location works, keeps a discarding sink and records the failure.
    SIGHT_CORE_API process(std::string _target, location _location, std::string _parent = {});

    /// Writes the last record and closes the log.
    SIGHT_CORE_API ~process();

    process(const process&)            = delete;
    process(process&&)                 = delete;
    process& operator=(const process&) = delete;
    process& operator=(process&&)      = delete;

    /// Empty while every log could be created and written, the first problem otherwise.
    [[nodiscard]] SIGHT_CORE_API std::string failure() const;

    [[nodiscard]] SIGHT_CORE_API const std::string& execution_id() const noexcept;

    /// The process log file, empty if it failed or was discarded.
    [[nodiscard]] SIGHT_CORE_API std::filesystem::path path() const;

    /// For a query run: closes and deletes the process log, and keeps discarding records.
    SIGHT_CORE_API void discard() noexcept;

private:

    friend class case_log;
    struct impl;
    std::unique_ptr<impl> m_pimpl;
};

/// Log of one test case execution. The process log only receives test records meanwhile.
class SIGHT_CORE_CLASS_API case_log final
{
public:

    /// Opens the case log with a copy of the process prologue.
    SIGHT_CORE_API case_log(process& _process, const case_identity& _identity);

    /// Finishes with an interrupted result if finish() was not called.
    SIGHT_CORE_API ~case_log();

    case_log(const case_log&)            = delete;
    case_log(case_log&&)                 = delete;
    case_log& operator=(const case_log&) = delete;
    case_log& operator=(case_log&&)      = delete;

    /// Writes the result record, closes the case log and resumes the process log. Only the first finish counts.
    SIGHT_CORE_API void finish(std::string_view _result) noexcept;

    /// For a crash handler, on any thread: appends the diagnostic and result records directly to the files, and
    /// resumes the process log, without lock, Boost.Log or allocation. Only the first finish counts.
    SIGHT_CORE_API void finish_after_crash(std::string_view _diagnostic, std::string_view _result) noexcept;

    /// The case log file, empty if it could not be created.
    [[nodiscard]] SIGHT_CORE_API std::filesystem::path path() const;

private:

    struct impl;
    std::unique_ptr<impl> m_pimpl;
};

} // namespace sight::core::log::detail::test_log
