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

#include <core/log/detail/test_log_file.hpp>
#include <core/log/detail/test_log_process.hpp>
#include <core/log/detail/test_log_sink.hpp>
#include <core/log/spy_logger.hpp>
#include <core/os/temp_path.hpp>
#include <core/spy_log.hpp>

#include <doctest/doctest.h>

#include <boost/log/core/core.hpp>

#include <algorithm>
#include <charconv>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <tuple>
#include <vector>

namespace test_log = sight::core::log::detail::test_log;

//------------------------------------------------------------------------------

static std::string read_file(const std::filesystem::path& _path)
{
    std::ifstream stream(_path, std::ios::binary);
    return {std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
}

//------------------------------------------------------------------------------

static std::vector<std::string> read_lines(const std::filesystem::path& _path)
{
    std::ifstream stream(_path);
    std::vector<std::string> lines;
    for(std::string line ; std::getline(stream, line) ; )
    {
        lines.push_back(line);
    }

    return lines;
}

//------------------------------------------------------------------------------

static bool contains(const std::string& _text, const std::string& _what)
{
    return _text.find(_what) != std::string::npos;
}

//------------------------------------------------------------------------------

/// Reads the "[dd.mm.YYYY HH:MM:SS.ffffff]" timestamp that starts a record.
static std::chrono::microseconds timestamp(const std::string& _line)
{
    const auto field = [&_line](std::size_t _position, std::size_t _size)
                       {
                           const char* const begin = _line.data() + _position;
                           int value               = -1;
                           std::ignore = std::from_chars(begin, begin + _size, value);
                           return value;
                       };

    REQUIRE_GE(_line.size(), 28U);
    const auto date = std::chrono::sys_days(
        std::chrono::year_month_day(
            std::chrono::year(field(7, 4)),
            std::chrono::month(static_cast<unsigned>(field(4, 2))),
            std::chrono::day(static_cast<unsigned>(field(1, 2)))
        )
    );

    return date.time_since_epoch() + std::chrono::hours(field(12, 2)) + std::chrono::minutes(field(15, 2))
           + std::chrono::seconds(field(18, 2)) + std::chrono::microseconds(field(21, 6));
}

TEST_SUITE("sight::core::log::detail::test_log")
{
//------------------------------------------------------------------------------

    TEST_CASE("file_names")
    {
        // Reference values from an independent FNV-1a 64 implementation: names must not depend on the platform
        CHECK_EQ(
            test_log::case_stem("data_ut", "sight::data::series", "loads_series"),
            "data_ut__sight__data__series__loads_series~64cab41a015c"
        );
        CHECK_EQ(test_log::case_stem("data_ut", "", "loads_series"), "data_ut__no-suite__loads_series~6890d9737445");
        CHECK_EQ(test_log::process_stem("data_ut"), "data_ut__process~c4064646d835");

        // Names equal once normalized keep distinct stems
        CHECK_NE(test_log::case_stem("t", "a::b", "c"), test_log::case_stem("t", "a__b", "c"));
    }

//------------------------------------------------------------------------------

    TEST_CASE("same_format_as_spy_logger")
    {
        const sight::core::os::temp_dir dir;
        std::error_code error;
        auto file = test_log::reserve(dir.path(), "stem", error);
        REQUIRE(file.has_value());
        const auto test_path = file->path;
        const auto spy_path  = dir / "spy.log";

        {
            const test_log::sink sink(std::move(*file), test_log::sink::mode::all);
            const auto spy_sink = sight::core::log::spy_logger::add_global_file_log(spy_path);
            SIGHT_INFO("format probe");
            boost::log::core::get()->remove_sink(spy_sink);
        }

        const auto probe_line =
            [](const std::filesystem::path& _path)
            {
                const auto lines = read_lines(_path);
                const auto found = std::ranges::find_if(
                    lines,
                    [](const auto& _l)
            {
                return contains(_l, "format probe");
            });
                return found != lines.end() ? *found : std::string();
            };

        const auto test_line = probe_line(test_path);
        REQUIRE_FALSE(test_line.empty());
        CHECK_EQ(test_line, probe_line(spy_path));
    }

//------------------------------------------------------------------------------

    TEST_CASE("crashed_case")
    {
        const sight::core::os::temp_dir dir;
        std::filesystem::path process_path;
        std::filesystem::path case_path;

        {
            test_log::process process("probe_ut", {.primary = dir.path(), .fallback = std::nullopt});
            process_path = process.path();

            test_log::case_log test_case(process, {.suite = "suite", .name = "crash", .file = "file.cpp", .line = 1});
            case_path = test_case.path();
            SIGHT_INFO("record before the crash");

            test_case.finish_after_crash("Test case crashed: SIGSEGV", "result=failed reasons=crash");
            SIGHT_INFO("record after the crash");
            test_case.finish("result=ignored");
        }

        const auto case_lines = read_lines(case_path);
        REQUIRE_GE(case_lines.size(), 3U);
        CHECK(contains(case_lines[case_lines.size() - 3], "record before the crash"));
        CHECK(contains(case_lines[case_lines.size() - 2], "[error] [test] Test case crashed: SIGSEGV"));
        CHECK(contains(case_lines.back(), "[info] [test] case end result=failed reasons=crash"));

        // Written without localtime, in the same local time as Boost.Log
        const auto elapsed = timestamp(case_lines[case_lines.size() - 2])
                             - timestamp(case_lines[case_lines.size() - 3]);
        CHECK_GE(elapsed, std::chrono::microseconds(0));
        CHECK_LT(elapsed, std::chrono::seconds(60));

        // The process log takes the records again
        const auto process_text = read_file(process_path);
        CHECK(contains(process_text, "[test] case end result=failed reasons=crash"));
        CHECK(contains(process_text, "record after the crash"));
        CHECK_FALSE(contains(process_text, "result=ignored"));
    }
} // TEST_SUITE
