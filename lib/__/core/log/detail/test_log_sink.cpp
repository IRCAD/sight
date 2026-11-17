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

#include "core/log/detail/test_log_sink.hpp"

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/log/attributes/current_process_id.hpp>
#include <boost/log/attributes/current_thread_id.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/attributes/value_extraction.hpp>
#include <boost/log/core/core.hpp>
#include <boost/log/expressions.hpp> // NOLINT(misc-include-cleaner): formatter operators
#include <boost/log/keywords/channel.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/support/date_time.hpp> // NOLINT(misc-include-cleaner): needed to format the dates
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>

#include <array>
#include <atomic>
#include <chrono>
#include <ctime>
#include <format>
#include <string>

#ifdef WIN32
#include <windows.h> // NOLINT(misc-include-cleaner): provides the process and thread API
#else
#include <pthread.h>
#include <unistd.h>
#endif

namespace sight::core::log::detail::test_log
{

using frontend_t = boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend>;

//------------------------------------------------------------------------------

void write(std::string_view _message, boost::log::trivial::severity_level _level) noexcept
{
    try
    {
        static boost::log::sources::severity_channel_logger_mt<boost::log::trivial::severity_level, std::string>
        s_logger(boost::log::keywords::channel = std::string(CHANNEL));

        BOOST_LOG_SEV(s_logger, _level) << "[test] " << _message;
    }
    catch(...)
    {
        // Logging must never break a test
    }
}

//------------------------------------------------------------------------------

/// Registers the attributes of the standard format, if spy_logger did not yet, and returns the origin of the uptime.
/// Called by the first sink, i.e. the process log at core initialization.
static std::chrono::steady_clock::time_point start_time()
{
    static const auto s_START =
        []
        {
            // Existing attributes are kept: spy_logger registers the same ones
            boost::log::add_common_attributes();
            boost::log::core::get()->add_global_attribute("Uptime", boost::log::attributes::timer());
            return std::chrono::steady_clock::now();
        }();

    return s_START;
}

//------------------------------------------------------------------------------

/// The standard Sight file format, as spy_logger writes it.
static boost::log::formatter file_formatter()
{
    namespace expressions = boost::log::expressions;
    using boost::posix_time::ptime;

    return expressions::stream << "["
           << expressions::format_date_time<ptime>("TimeStamp", "%d.%m.%Y %H:%M:%S.%f") << "]["
           << expressions::format_date_time<ptime::time_duration_type>("Uptime", "%H:%M:%S.%f") << "]["
           << expressions::attr<boost::log::attributes::current_process_id::value_type>("ProcessID") << "]["
           << expressions::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID") << "]["
           << expressions::attr<boost::log::trivial::severity_level>("Severity") << "] "
           << expressions::smessage;
}

//------------------------------------------------------------------------------

/// Offset of the local time from UTC, in seconds, for the crash path: localtime takes a libc lock that the crashed
/// thread may hold.
static std::atomic<std::int64_t> s_utc_offset {0};

//------------------------------------------------------------------------------

/// Updates s_utc_offset. Called by each new sink, i.e. at core initialization and at each case start.
static void refresh_utc_offset() noexcept
{
    using std::chrono::hours;
    using std::chrono::minutes;
    using std::chrono::seconds;
    using std::chrono::sys_days;
    using std::chrono::system_clock;

    const auto now  = std::chrono::floor<seconds>(system_clock::now());
    const auto time = system_clock::to_time_t(now);

    std::tm local {};
#ifdef WIN32
    if(localtime_s(&local, &time) != 0)
#else
    if(localtime_r(&time, &local) == nullptr)
#endif
    {
        return;
    }

    const auto date = std::chrono::year_month_day(
        std::chrono::year(local.tm_year + 1900),
        std::chrono::month(static_cast<unsigned>(local.tm_mon + 1)),
        std::chrono::day(static_cast<unsigned>(local.tm_mday))
    );
    const auto local_time = sys_days(date) + hours(local.tm_hour) + minutes(local.tm_min) + seconds(local.tm_sec);

    s_utc_offset.store((local_time - now).count(), std::memory_order_relaxed);
}

//------------------------------------------------------------------------------

/// Formats a record like file_formatter(), in _buffer, without lock or allocation. Only called after a sink exists.
static std::string_view format_record(
    std::array<char, 1024>& _buffer,
    std::string_view _level,
    std::string_view _message
) noexcept
{
    using std::chrono::duration_cast;
    using std::chrono::microseconds;
    using std::chrono::seconds;
    using std::chrono::steady_clock;

    const auto local = std::chrono::floor<microseconds>(std::chrono::system_clock::now())
                       + seconds(s_utc_offset.load(std::memory_order_relaxed));
    const auto date   = std::chrono::floor<std::chrono::days>(local);
    const auto ymd    = std::chrono::year_month_day(date);
    const auto clock  = std::chrono::hh_mm_ss(local - date);
    const auto uptime = duration_cast<microseconds>(steady_clock::now() - start_time()).count();

#ifdef WIN32
    const auto process_id = static_cast<std::uint32_t>(::GetCurrentProcessId());
    const auto thread_id  = static_cast<std::uint32_t>(::GetCurrentThreadId());
#else
    const auto process_id = static_cast<std::uint32_t>(::getpid());
    const auto thread_id  = static_cast<std::uintmax_t>(::pthread_self());
#endif

    const auto result = std::format_to_n(
        _buffer.data(),
        static_cast<std::ptrdiff_t>(_buffer.size() - 1),
        "[{:02}.{:02}.{:04} {:02}:{:02}:{:02}.{:06}][{:02}:{:02}:{:02}.{:06}][0x{:08x}][0x{:0{}x}][{}] [test] {}",
        static_cast<unsigned>(ymd.day()),
        static_cast<unsigned>(ymd.month()),
        static_cast<int>(ymd.year()),
        clock.hours().count(),
        clock.minutes().count(),
        clock.seconds().count(),
        clock.subseconds().count(),
        uptime / 3'600'000'000,
        uptime / 60'000'000 % 60,
        uptime / 1'000'000 % 60,
        uptime % 1'000'000,
        process_id,
        thread_id,
        sizeof(thread_id) * 2,
        _level,
        _message
    );

    // Truncated if needed, always one line
    const auto size = std::min<std::size_t>(static_cast<std::size_t>(result.size), _buffer.size() - 1);
    _buffer.at(size) = '\n';
    return {_buffer.data(), size + 1};
}

//------------------------------------------------------------------------------

struct sink::impl
{
    std::filesystem::path path;
    boost::shared_ptr<std::ostream> stream;
    raw_handle raw;
    boost::shared_ptr<frontend_t> frontend;
    bool attached {false};

    // Shared with the filter, which may outlive this object for records in flight
    std::shared_ptr<std::atomic<mode> > mode_flag;
};

//------------------------------------------------------------------------------

sink::sink(reserved_file _file, mode _mode) :
    m_pimpl(std::make_unique<impl>())
{
    m_pimpl->path      = std::move(_file.path);
    m_pimpl->stream    = std::move(_file.stream);
    m_pimpl->raw       = std::move(_file.raw);
    m_pimpl->mode_flag = std::make_shared<std::atomic<mode> >(_mode);
    std::ignore        = start_time();
    refresh_utc_offset();

    auto backend = boost::make_shared<boost::log::sinks::text_ostream_backend>();
    if(m_pimpl->stream)
    {
        backend->add_stream(m_pimpl->stream);
    }

    backend->auto_flush(true);

    m_pimpl->frontend = boost::make_shared<frontend_t>(backend);
    m_pimpl->frontend->set_formatter(file_formatter());
    m_pimpl->frontend->set_filter(
        [mode_flag = m_pimpl->mode_flag](const boost::log::attribute_value_set& _attributes)
        {
            switch(mode_flag->load(std::memory_order_acquire))
            {
                case mode::all:
                    return true;

                case mode::test_only:
                    return boost::log::extract_or_default<std::string>("Channel", _attributes, std::string())
                           == CHANNEL;

                default:
                    return false;
            }
        });

    boost::log::core::get()->add_sink(m_pimpl->frontend);
    m_pimpl->attached = true;
}

//------------------------------------------------------------------------------

sink::~sink()
{
    detach();
    close();
}

//------------------------------------------------------------------------------

void sink::set_mode(mode _mode) noexcept
{
    m_pimpl->mode_flag->store(_mode, std::memory_order_release);
}

//------------------------------------------------------------------------------

const std::filesystem::path& sink::path() const noexcept
{
    return m_pimpl->path;
}

//------------------------------------------------------------------------------

std::uint64_t sink::size()
{
    if(!m_pimpl->frontend)
    {
        return 0;
    }

    // The frontend writes each record whole under the backend lock
    const auto backend = m_pimpl->frontend->locked_backend();
    if(!m_pimpl->stream)
    {
        return 0;
    }

    m_pimpl->stream->flush();
    const auto position = m_pimpl->stream->tellp();
    return position < 0 ? 0 : static_cast<std::uint64_t>(position);
}

//------------------------------------------------------------------------------

bool sink::good()
{
    if(!m_pimpl->frontend)
    {
        return false;
    }

    const auto backend = m_pimpl->frontend->locked_backend();
    return m_pimpl->stream && m_pimpl->stream->good();
}

//------------------------------------------------------------------------------

void sink::close() noexcept
{
    if(!m_pimpl->frontend)
    {
        return;
    }

    try
    {
        auto backend = m_pimpl->frontend->locked_backend();
        if(m_pimpl->stream)
        {
            m_pimpl->stream->flush();
            backend->remove_stream(m_pimpl->stream);
            m_pimpl->stream.reset();
        }
    }
    catch(...)
    {
        m_pimpl->stream.reset();
    }
}

//------------------------------------------------------------------------------

bool sink::emergency_write(std::string_view _level, std::string_view _message) const noexcept
{
    std::array<char, 1024> buffer {};
    return m_pimpl->raw.write(format_record(buffer, _level, _message));
}

//------------------------------------------------------------------------------

void sink::detach() noexcept
{
    if(m_pimpl->attached)
    {
        m_pimpl->attached = false;
        m_pimpl->mode_flag->store(mode::none, std::memory_order_release);
        boost::log::core::get()->remove_sink(m_pimpl->frontend);
    }
}

} // namespace sight::core::log::detail::test_log
