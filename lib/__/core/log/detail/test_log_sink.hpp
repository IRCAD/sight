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

#include "core/log/detail/test_log_file.hpp"

#include <boost/log/trivial.hpp>

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string_view>

namespace sight::core::log::detail::test_log
{

/// Channel of the identity, phase and result records, accepted by restricted test sinks.
constexpr std::string_view CHANNEL = "sight.test";

/// Writes a record on the test channel, in the standard Sight format.
SIGHT_CORE_API void write(
    std::string_view _message,
    boost::log::trivial::severity_level _level = boost::log::trivial::info
) noexcept;

/// Synchronous, non-rotating file sink of a test log, flushed after every record.
class SIGHT_CORE_CLASS_API sink final
{
public:

    enum class mode : std::uint8_t
    {
        all,       ///< every record
        test_only, ///< test channel records only
        none       ///< nothing
    };

    /// Registers the sink in the logging core. Without a stream, it only keeps the core from using its default console
    /// sink.
    SIGHT_CORE_API sink(reserved_file _file, mode _mode);

    /// Detaches the sink and closes the file.
    SIGHT_CORE_API ~sink();

    sink(const sink&)            = delete;
    sink(sink&&)                 = delete;
    sink& operator=(const sink&) = delete;
    sink& operator=(sink&&)      = delete;

    SIGHT_CORE_API void set_mode(mode _mode) noexcept;

    [[nodiscard]] SIGHT_CORE_API const std::filesystem::path& path() const noexcept;

    /// Flushes and returns the bytes written so far, at a record boundary.
    [[nodiscard]] SIGHT_CORE_API std::uint64_t size();

    /// False once a write or a flush failed, or once the file is closed.
    [[nodiscard]] SIGHT_CORE_API bool good();

    /// For a crash handler: appends a record in the standard format, without Boost.Log, lock or allocation. The sink
    /// should be in mode::none, so that concurrent records do not interleave with it.
    [[nodiscard]] SIGHT_CORE_API bool emergency_write(
        std::string_view _level,
        std::string_view _message
    ) const noexcept;

    /// Closes the file but stays registered: with mode::none, it keeps the logging core from falling back to its
    /// default console sink.
    SIGHT_CORE_API void close() noexcept;

    /// Unregisters the sink, but keeps the file open: records accepted before still reach it. The file closes on
    /// destruction.
    SIGHT_CORE_API void detach() noexcept;

private:

    struct impl;
    std::unique_ptr<impl> m_pimpl;
};

} // namespace sight::core::log::detail::test_log
