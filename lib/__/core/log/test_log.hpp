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

#include "core/log/detail/test_log_process.hpp"

#include <memory>
#include <string>
#include <string_view>

/**
 * Per-test logs of the generated test main.
 *
 * In an executable exporting the `sight_test_target` symbol, core opens a process log at its static initialization
 * and closes it when unloaded. The test main writes phases and opens one case log per test case. Elsewhere, every
 * function does nothing.
 */
namespace sight::core::log::test_log
{

using case_identity = detail::test_log::case_identity;

/// True in a test executable, even if its logs could not be initialized.
[[nodiscard]] SIGHT_CORE_API bool enabled() noexcept;

/// Empty while the test logs work, the first problem otherwise.
[[nodiscard]] SIGHT_CORE_API std::string failure();

/// Shared by every log of this process, and given to supervised children as their parent.
[[nodiscard]] SIGHT_CORE_API std::string execution_id();

/// For a query run: deletes the process log and discards records.
SIGHT_CORE_API void discard() noexcept;

/// Writes a phase record, e.g. "main entered".
SIGHT_CORE_API void phase(std::string_view _what) noexcept;

/// Log of the running test case.
class SIGHT_CORE_CLASS_API case_log final
{
public:

    SIGHT_CORE_API explicit case_log(const case_identity& _identity);
    SIGHT_CORE_API ~case_log();

    case_log(const case_log&)            = delete;
    case_log(case_log&&)                 = delete;
    case_log& operator=(const case_log&) = delete;
    case_log& operator=(case_log&&)      = delete;

    /// Writes the result record and closes the case log. Only the first finish counts.
    SIGHT_CORE_API void finish(std::string_view _result) noexcept;

    /// For a crash handler, on any thread: writes the records without lock, Boost.Log or allocation.
    SIGHT_CORE_API void finish_after_crash(std::string_view _diagnostic, std::string_view _result) noexcept;

private:

    std::unique_ptr<detail::test_log::case_log> m_case;
};

} // namespace sight::core::log::test_log
