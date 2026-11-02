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

#include "core/log/test_log.hpp"

#include "core/log/detail/test_log_sink.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdlib>
#include <exception>

#ifdef WIN32
#include <windows.h> // NOLINT(misc-include-cleaner): provides the module API
#else
#include <dlfcn.h>
#endif

namespace sight::core::log::detail::test_log
{

// Constant-initialized and trivially destructible: valid during the whole static initialization and destruction
constinit static std::atomic<process*> s_process {nullptr};
constinit static std::atomic<bool> s_marker_found {false};
constinit static std::array<char, 512> s_init_failure {};

// Keeps the logging core from printing records on the standard output if the process log could not be created
constinit static std::atomic<sink*> s_null_sink {nullptr};

//------------------------------------------------------------------------------

/// The target name exported by the generated test main, or nullptr in any other executable.
static const char* find_test_target() noexcept
{
#ifdef WIN32
    // Only the executable itself is searched, and its constant data is readable before its own initialization
    return reinterpret_cast<const char*>(::GetProcAddress(::GetModuleHandleW(nullptr), "sight_test_target"));
#else
    return static_cast<const char*>(::dlsym(RTLD_DEFAULT, "sight_test_target"));
#endif
}

//------------------------------------------------------------------------------

namespace
{

/// Opens the process log if the executable exports `sight_test_target`, and closes it when core is unloaded.
class process_owner final
{
public:

    process_owner() noexcept;
    ~process_owner();

    process_owner(const process_owner&)            = delete;
    process_owner(process_owner&&)                 = delete;
    process_owner& operator=(const process_owner&) = delete;
    process_owner& operator=(process_owner&&)      = delete;
};

} // namespace

//------------------------------------------------------------------------------

process_owner::process_owner() noexcept
{
    const char* const target = find_test_target();
    if(target == nullptr)
    {
        return;
    }

    s_marker_found = true;

    try
    {
        const char* const parent = std::getenv("SIGHT_TEST_PARENT");
        s_process.store(new process(target, default_location(), parent != nullptr ? parent : ""));
    }
    catch(const std::exception& e)
    {
        // Recorded without allocation: main() refuses to run the tests
        const std::string_view what = e.what();
        std::ranges::copy(what.substr(0, s_init_failure.size() - 1), s_init_failure.begin());

        try
        {
            s_null_sink.store(new sink(reserved_file {}, sink::mode::none));
        }
        catch(...)
        {
            // Queries may then print records
        }
    }
    catch(...)
    {
        constexpr std::string_view what = "unknown error";
        std::ranges::copy(what, s_init_failure.begin());
    }
}

//------------------------------------------------------------------------------

process_owner::~process_owner()
{
    delete s_process.exchange(nullptr);
    delete s_null_sink.exchange(nullptr);
}

// Lives from core's static initialization to its unload
static const process_owner OWNER;

} // namespace sight::core::log::detail::test_log

namespace sight::core::log::test_log
{

using detail::test_log::s_process;

//------------------------------------------------------------------------------

bool enabled() noexcept
{
    return detail::test_log::s_marker_found;
}

//------------------------------------------------------------------------------

std::string failure()
{
    if(auto* const process = s_process.load(); process != nullptr)
    {
        return process->failure();
    }

    if(enabled())
    {
        const auto* const init_failure = detail::test_log::s_init_failure.data();
        return std::string("cannot initialize the test logs: ") + (*init_failure != 0 ? init_failure : "unknown error");
    }

    return {};
}

//------------------------------------------------------------------------------

std::string execution_id()
{
    auto* const process = s_process.load();
    return process != nullptr ? process->execution_id() : std::string();
}

//------------------------------------------------------------------------------

void discard() noexcept
{
    if(auto* const process = s_process.load(); process != nullptr)
    {
        process->discard();
    }
}

//------------------------------------------------------------------------------

void phase(std::string_view _what) noexcept
{
    if(enabled())
    {
        detail::test_log::write(_what);
    }
}

//------------------------------------------------------------------------------

case_log::case_log(const case_identity& _identity)
{
    if(auto* const process = s_process.load(); process != nullptr)
    {
        m_case = std::make_unique<detail::test_log::case_log>(*process, _identity);
    }
}

//------------------------------------------------------------------------------

case_log::~case_log() = default;

//------------------------------------------------------------------------------

void case_log::finish(std::string_view _result) noexcept
{
    if(m_case)
    {
        m_case->finish(_result);
    }
}

//------------------------------------------------------------------------------

void case_log::finish_after_crash(std::string_view _diagnostic, std::string_view _result) noexcept
{
    if(m_case)
    {
        m_case->finish_after_crash(_diagnostic, _result);
    }
}

} // namespace sight::core::log::test_log
