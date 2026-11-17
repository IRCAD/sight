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

#include "core/log/detail/test_log_process.hpp"

#include "core/log/detail/test_log_file.hpp"
#include "core/log/detail/test_log_sink.hpp"

#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/log/trivial.hpp>

#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <fstream>
#include <iostream>
#include <mutex>
#include <tuple>
#include <vector>

#ifdef WIN32
#include <process.h>
#include <windows.h> // NOLINT(misc-include-cleaner): provides TerminateProcess
#else
#include <unistd.h>
#endif

namespace sight::core::log::detail::test_log
{

static constexpr std::size_t COPY_BUFFER_SIZE = 65536;

//------------------------------------------------------------------------------

location default_location()
{
    location result;

    boost::dll::fs::error_code dll_error;
    result.primary = std::filesystem::path(boost::dll::program_location(dll_error).parent_path().native())
                     / "test-logs";

    std::error_code error;

    if(const char* const fallback = std::getenv("SIGHT_TEST_LOG_FALLBACK_DIR"); fallback != nullptr)
    {
        if(std::string_view(fallback) != "none")
        {
            result.fallback = std::filesystem::path(fallback);
        }
    }
    else if(const auto temp = std::filesystem::temp_directory_path(error); !error)
    {
        result.fallback = temp / "sight-test-logs";
    }

    return result;
}

//------------------------------------------------------------------------------

/// Ends the process at once with a failure status, e.g. from a static destructor.
[[noreturn]] static void exit_with_failure() noexcept
{
#ifdef WIN32
    // Safe under the loader lock, unlike exit()
    ::TerminateProcess(::GetCurrentProcess(), EXIT_FAILURE);
#endif
    std::_Exit(EXIT_FAILURE);
}

//------------------------------------------------------------------------------

static int process_id()
{
#ifdef WIN32
    return _getpid();
#else
    return static_cast<int>(::getpid());
#endif
}

//------------------------------------------------------------------------------

struct process::impl
{
    std::string target;
    location dirs;
    std::string parent;
    std::string execution_id;

    std::mutex mutex;
    std::unique_ptr<sink> process_sink;

    // Closed case logs, kept open until the end: records accepted before their closure may still reach them
    std::vector<std::unique_ptr<sink> > retired;
    bool discarded {false};
    std::optional<std::uint64_t> prologue_size;
    std::string failure;

    //------------------------------------------------------------------------------

    void report(const std::string& _what) noexcept
    {
        try
        {
            std::cerr << "[sight] test log: " << _what << std::endl;
            if(failure.empty())
            {
                failure = _what;
            }
        }
        catch(...)
        {
            // Nothing more can be done
        }
    }

    //------------------------------------------------------------------------------

    std::optional<reserved_file> reserve(std::string_view _stem)
    {
        std::error_code error;
        if(auto file = test_log::reserve(dirs.primary, _stem, error); file)
        {
            return file;
        }

        auto why = std::format("cannot create a log in '{}': {}", dirs.primary.string(), error.message());

        if(dirs.fallback)
        {
            if(auto file = test_log::reserve(*dirs.fallback, _stem, error); file)
            {
                std::cerr << "[sight] test log: " << why << ", using '" << file->path.string() << "'" << std::endl;
                return file;
            }

            why += std::format(", nor in '{}': {}", dirs.fallback->string(), error.message());
        }

        report(why);
        return std::nullopt;
    }
};

//------------------------------------------------------------------------------

process::process(std::string _target, location _location, std::string _parent) :
    m_pimpl(std::make_unique<impl>())
{
    m_pimpl->target       = std::move(_target);
    m_pimpl->dirs         = std::move(_location);
    m_pimpl->parent       = std::move(_parent);
    m_pimpl->execution_id = std::format(
        "{}-{}",
        process_id(),
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count()
    );

    if(auto file = m_pimpl->reserve(process_stem(m_pimpl->target)); file)
    {
        const auto path    = file->path;
        const auto attempt = file->attempt;
        m_pimpl->process_sink = std::make_unique<sink>(std::move(*file), sink::mode::all);

        write(
            std::format(
                "process begin target={} exec={} parent={} pid={} attempt={} path={}",
                m_pimpl->target,
                m_pimpl->execution_id,
                m_pimpl->parent.empty() ? "none" : m_pimpl->parent,
                process_id(),
                attempt,
                path.string()
            )
        );
    }
    else
    {
        // Keeps the logging core from printing records on the standard output
        m_pimpl->process_sink = std::make_unique<sink>(reserved_file {}, sink::mode::none);
    }
}

//------------------------------------------------------------------------------

process::~process()
{
    bool write_failed = false;

    if(!m_pimpl->discarded && !m_pimpl->process_sink->path().empty())
    {
        write("process end");
        write_failed = !m_pimpl->process_sink->good();
    }

    for(const auto& retired : m_pimpl->retired)
    {
        write_failed = write_failed || !retired->good();
    }

    m_pimpl->retired.clear();
    m_pimpl->process_sink.reset();

    // Too late for main() to report it: the exit status is the only way left
    if(write_failed)
    {
        std::cerr << "[sight] test log: write failure at the end of the process" << std::endl;
        exit_with_failure();
    }
}

//------------------------------------------------------------------------------

std::string process::failure() const
{
    std::unique_lock lock(m_pimpl->mutex);
    if(m_pimpl->failure.empty() && !m_pimpl->discarded && !m_pimpl->process_sink->path().empty()
       && !m_pimpl->process_sink->good())
    {
        m_pimpl->report(std::format("cannot write '{}'", m_pimpl->process_sink->path().string()));
    }

    return m_pimpl->failure;
}

//------------------------------------------------------------------------------

const std::string& process::execution_id() const noexcept
{
    return m_pimpl->execution_id;
}

//------------------------------------------------------------------------------

std::filesystem::path process::path() const
{
    std::unique_lock lock(m_pimpl->mutex);
    return m_pimpl->discarded ? std::filesystem::path() : m_pimpl->process_sink->path();
}

//------------------------------------------------------------------------------

void process::discard() noexcept
{
    std::unique_lock lock(m_pimpl->mutex);
    if(m_pimpl->discarded)
    {
        return;
    }

    m_pimpl->discarded = true;
    m_pimpl->process_sink->set_mode(sink::mode::none);
    m_pimpl->process_sink->close();

    // Closed before removal: the attempt number is free again at once
    if(const auto& path = m_pimpl->process_sink->path(); !path.empty())
    {
        std::error_code error;
        std::filesystem::remove(path, error);
    }
}

//------------------------------------------------------------------------------

/// Copies the first _size bytes of _from at the beginning of _to.
static bool copy_prologue(const std::filesystem::path& _from, std::uint64_t _size, std::ostream& _to)
{
    std::ifstream input(_from, std::ios::binary);
    std::array<char, COPY_BUFFER_SIZE> buffer {};

    while(_size > 0 && input)
    {
        input.read(buffer.data(), static_cast<std::streamsize>(std::min<std::uint64_t>(_size, buffer.size())));
        _to.write(buffer.data(), input.gcount());
        _size -= static_cast<std::uint64_t>(input.gcount());
    }

    _to.flush();
    return _size == 0 && _to.good();
}

//------------------------------------------------------------------------------

struct case_log::impl
{
    explicit impl(process& _process) :
        owner(_process)
    {
    }

    process& owner;
    std::unique_ptr<sink> case_sink;
    std::filesystem::path path;

    // Set by the first finish, normal or after a crash
    std::atomic<bool> finished {false};
};

//------------------------------------------------------------------------------

case_log::case_log(process& _process, const case_identity& _identity) :
    m_pimpl(std::make_unique<impl>(_process))
{
    auto& owner = *_process.m_pimpl;
    std::unique_lock lock(owner.mutex);

    auto& process_sink = *owner.process_sink;
    if(!owner.prologue_size)
    {
        owner.prologue_size = process_sink.size();
    }

    auto file = owner.reserve(case_stem(owner.target, _identity.suite, _identity.name));
    if(!file)
    {
        m_pimpl->finished = true;
        return;
    }

    const bool copied = process_sink.path().empty()
                        || copy_prologue(process_sink.path(), *owner.prologue_size, *file->stream);

    const auto path    = file->path;
    const auto attempt = file->attempt;
    m_pimpl->path = path;

    // The identity record goes to both logs, then the case log takes every record
    m_pimpl->case_sink = std::make_unique<sink>(std::move(*file), sink::mode::test_only);
    write(
        std::format(
            "case begin target={} suite={} case={} source={}:{} exec={} attempt={} path={}",
            owner.target,
            _identity.suite,
            _identity.name,
            _identity.file,
            _identity.line,
            owner.execution_id,
            attempt,
            path.string()
        )
    );

    if(!copied)
    {
        write("incomplete copy of the process prologue", boost::log::trivial::warning);
        owner.report(std::format("incomplete prologue copy in '{}'", path.string()));
    }

    m_pimpl->case_sink->set_mode(sink::mode::all);
    process_sink.set_mode(sink::mode::test_only);
}

//------------------------------------------------------------------------------

case_log::~case_log()
{
    finish("result=interrupted");
}

//------------------------------------------------------------------------------

void case_log::finish(std::string_view _result) noexcept
{
    auto& owner = *m_pimpl->owner.m_pimpl;
    const std::unique_lock lock(owner.mutex);

    if(m_pimpl->finished.exchange(true))
    {
        return;
    }

    if(!owner.discarded)
    {
        owner.process_sink->set_mode(sink::mode::all);
    }

    m_pimpl->case_sink->set_mode(sink::mode::test_only);
    write(std::format("case end {}", _result));

    if(!m_pimpl->case_sink->good())
    {
        owner.report(std::format("cannot write '{}'", m_pimpl->case_sink->path().string()));
    }

    m_pimpl->case_sink->detach();

    try
    {
        owner.retired.push_back(std::move(m_pimpl->case_sink));
    }
    catch(...)
    {
        // Closes it now: only records still in flight are lost
        m_pimpl->case_sink.reset();
    }
}

//------------------------------------------------------------------------------

void case_log::finish_after_crash(std::string_view _diagnostic, std::string_view _result) noexcept
{
    // Called from a crash handler, possibly on a worker thread: no lock, no Boost.Log, no allocation
    if(m_pimpl->finished.exchange(true))
    {
        return;
    }

    auto& owner     = *m_pimpl->owner.m_pimpl;
    auto& case_sink = *m_pimpl->case_sink;
    auto& proc_sink = *owner.process_sink;
    std::array<char, 256> result {};
    const auto end = std::format_to_n(
        result.data(),
        static_cast<std::ptrdiff_t>(result.size()),
        "case end {}",
        _result
    );
    const auto marker = std::string_view(result.data(), std::min(static_cast<std::size_t>(end.size), result.size()));

    case_sink.set_mode(sink::mode::none);
    std::ignore = case_sink.emergency_write("error", _diagnostic);
    std::ignore = case_sink.emergency_write("info", marker);

    std::ignore = proc_sink.emergency_write("info", marker);
    proc_sink.set_mode(sink::mode::all);
}

//------------------------------------------------------------------------------

std::filesystem::path case_log::path() const
{
    return m_pimpl->path;
}

} // namespace sight::core::log::detail::test_log
