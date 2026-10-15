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

#include <boost/smart_ptr/shared_ptr.hpp>

#include <cstdint>
#include <filesystem>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <system_error>

/// Names and exclusive creation of the per-test log files.
namespace sight::core::log::detail::test_log
{

/// "<target>__<suite or no-suite>__<case>~<hash>".
SIGHT_CORE_API std::string case_stem(std::string_view _target, std::string_view _suite, std::string_view _name);

/// "<target>__process~<hash>".
SIGHT_CORE_API std::string process_stem(std::string_view _target);

/// Second handle on a reserved file, sharing its position with the stream: lets a crash handler append records
/// without Boost.Log, locks or allocations.
class SIGHT_CORE_CLASS_API raw_handle final
{
public:

    raw_handle() noexcept = default;
    SIGHT_CORE_API explicit raw_handle(std::intptr_t _native) noexcept;
    SIGHT_CORE_API ~raw_handle();

    SIGHT_CORE_API raw_handle(raw_handle&& _other) noexcept;
    SIGHT_CORE_API raw_handle& operator=(raw_handle&& _other) noexcept;
    raw_handle(const raw_handle&)            = delete;
    raw_handle& operator=(const raw_handle&) = delete;

    /// Writes all of _data, or returns false.
    [[nodiscard]] SIGHT_CORE_API bool write(std::string_view _data) const noexcept;

private:

    std::intptr_t m_native {-1};
};

/// A log file created by this process only, open for writing.
struct reserved_file
{
    std::filesystem::path path;
    unsigned attempt {0};
    boost::shared_ptr<std::ostream> stream;
    raw_handle raw;
};

/// Creates "<_dir>/<_stem>__attempt-N.log" with the first free N, never opening an existing file.
/// @return std::nullopt with _error set if the directory is unusable or every attempt number is taken.
SIGHT_CORE_API std::optional<reserved_file> reserve(
    const std::filesystem::path& _dir,
    std::string_view _stem,
    std::error_code& _error
);

} // namespace sight::core::log::detail::test_log
