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

#include "core/log/detail/test_log_file.hpp"

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>

#include <cstddef>
#include <cstdint>
#include <format>
#include <initializer_list>
#include <string>
#include <string_view>
#include <utility>

#ifdef WIN32
#include <windows.h> // NOLINT(misc-include-cleaner): provides the Win32 file API
#else
#include <fcntl.h>
#include <unistd.h>

#include <cerrno>
#endif

namespace sight::core::log::detail::test_log
{

/// Longest readable part of a stem: keeps "<readable>~<hash>__attempt-99999.log" within 100 characters.
constexpr std::size_t READABLE_MAX = 68;

/// Highest attempt number; reaching it is an allocation failure.
constexpr unsigned ATTEMPT_MAX = 99'999;

//------------------------------------------------------------------------------

/// Replaces "::" by "__" and every byte outside ASCII [A-Za-z0-9_.-] by '_'.
static std::string normalize(std::string_view _text)
{
    std::string result;
    result.reserve(_text.size());

    for(std::size_t i = 0 ; i < _text.size() ; ++i)
    {
        const char c = _text[i];

        if(c == ':' && i + 1 < _text.size() && _text[i + 1] == ':')
        {
            result += "__";
            ++i;
        }
        else if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')
                || c == '_' || c == '.' || c == '-')
        {
            result += c;
        }
        else
        {
            result += '_';
        }
    }

    return result;
}

//------------------------------------------------------------------------------

/// High 48 bits of FNV-1a 64 over the length-prefixed fields, as 12 lowercase hex digits.
static std::string identity_hash(std::initializer_list<std::string_view> _fields)
{
    std::uint64_t hash = 0xcbf29ce484222325ULL;

    const auto feed = [&hash](std::string_view _bytes)
                      {
                          for(const char c : _bytes)
                          {
                              hash ^= static_cast<unsigned char>(c);
                              hash *= 0x100000001b3ULL;
                          }
                      };

    for(const auto field : _fields)
    {
        feed(std::to_string(field.size()));
        feed(":");
        feed(field);
    }

    return std::format("{:012x}", hash >> 16U);
}

//------------------------------------------------------------------------------

static std::string stem(std::string _readable, std::string_view _hash)
{
    if(_readable.size() > READABLE_MAX)
    {
        _readable.resize(READABLE_MAX);
    }

    return _readable + '~' + std::string(_hash);
}

//------------------------------------------------------------------------------

std::string case_stem(std::string_view _target, std::string_view _suite, std::string_view _name)
{
    const std::string_view suite = _suite.empty() ? "no-suite" : _suite;
    return stem(
        normalize(_target) + "__" + normalize(suite) + "__" + normalize(_name),
        identity_hash({"case", _target, _suite, _name})
    );
}

//------------------------------------------------------------------------------

std::string process_stem(std::string_view _target)
{
    return stem(normalize(_target) + "__process", identity_hash({"process", _target}));
}

//------------------------------------------------------------------------------

raw_handle::raw_handle(std::intptr_t _native) noexcept :
    m_native(_native)
{
}

//------------------------------------------------------------------------------

raw_handle::~raw_handle()
{
    if(m_native != -1)
    {
#ifdef WIN32
        ::CloseHandle(reinterpret_cast<HANDLE>(m_native));
#else
        ::close(static_cast<int>(m_native));
#endif
    }
}

//------------------------------------------------------------------------------

raw_handle::raw_handle(raw_handle&& _other) noexcept :
    m_native(std::exchange(_other.m_native, -1))
{
}

//------------------------------------------------------------------------------

raw_handle& raw_handle::operator=(raw_handle&& _other) noexcept
{
    raw_handle old(std::move(*this));
    m_native = std::exchange(_other.m_native, -1);
    return *this;
}

//------------------------------------------------------------------------------

bool raw_handle::write(std::string_view _data) const noexcept
{
    if(m_native == -1)
    {
        return false;
    }

    while(!_data.empty())
    {
#ifdef WIN32
        DWORD written = 0;
        if(::WriteFile(
               reinterpret_cast<HANDLE>(m_native),
               _data.data(),
               static_cast<DWORD>(_data.size()),
               &written,
               nullptr
           ) == FALSE || written == 0)
        {
            return false;
        }
#else
        const auto written = ::write(static_cast<int>(m_native), _data.data(), _data.size());
        if(written <= 0)
        {
            if(written < 0 && errno == EINTR)
            {
                continue;
            }

            return false;
        }
#endif

        _data.remove_prefix(static_cast<std::size_t>(written));
    }

    return true;
}

//------------------------------------------------------------------------------

/// Opens a new file for writing, failing if it already exists. Handles are not inherited by child processes.
static std::optional<reserved_file> create_new(
    std::filesystem::path _path,
    unsigned _attempt,
    std::error_code& _error
)
{
    namespace io = boost::iostreams;

#ifdef WIN32
    // Others may read the file (prologue copy) or delete it (query cleanup) while it is open
    HANDLE handle = ::CreateFileW(
        _path.c_str(),
        GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_DELETE,
        nullptr,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if(handle == INVALID_HANDLE_VALUE)
    {
        const auto last_error = ::GetLastError();
        _error = last_error == ERROR_FILE_EXISTS || last_error == ERROR_ALREADY_EXISTS
                 ? std::make_error_code(std::errc::file_exists)
                 : std::error_code(static_cast<int>(last_error), std::system_category());
        return std::nullopt;
    }

    // A duplicate shares the file position: crash writes append after the stream's
    HANDLE duplicate = nullptr;
    if(::DuplicateHandle(
           ::GetCurrentProcess(),
           handle,
           ::GetCurrentProcess(),
           &duplicate,
           0,
           FALSE,
           DUPLICATE_SAME_ACCESS
       ) == FALSE)
    {
        _error = std::error_code(static_cast<int>(::GetLastError()), std::system_category());
        ::CloseHandle(handle);
        return std::nullopt;
    }

    raw_handle raw(reinterpret_cast<std::intptr_t>(duplicate));
#else
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,hicpp-vararg): open() is the only way to get O_EXCL
    const int handle = ::open(_path.c_str(), O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, 0644);

    if(handle < 0)
    {
        _error = std::error_code(errno, std::generic_category());
        return std::nullopt;
    }

    // A duplicate shares the file position: crash writes append after the stream's
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,hicpp-vararg): POSIX API
    const int duplicate = ::fcntl(handle, F_DUPFD_CLOEXEC, 0);
    if(duplicate < 0)
    {
        _error = std::error_code(errno, std::generic_category());
        ::close(handle);
        return std::nullopt;
    }
    raw_handle raw(duplicate);
#endif

    try
    {
        return reserved_file {
            .path    = std::move(_path),
            .attempt = _attempt,
            .stream  = boost::make_shared<io::stream<io::file_descriptor_sink> >(handle, io::close_handle),
            .raw     = std::move(raw)
        };
    }
    catch(...)
    {
#ifdef WIN32
        ::CloseHandle(handle);
#else
        ::close(handle);
#endif
        throw;
    }
}

//------------------------------------------------------------------------------

std::optional<reserved_file> reserve(
    const std::filesystem::path& _dir,
    std::string_view _stem,
    std::error_code& _error
)
{
    _error.clear();

    std::filesystem::create_directories(_dir, _error);
    if(_error)
    {
        return std::nullopt;
    }

    for(unsigned attempt = 1 ; attempt <= ATTEMPT_MAX ; ++attempt)
    {
        if(auto file = create_new(_dir / std::format("{}__attempt-{}.log", _stem, attempt), attempt, _error); file)
        {
            _error.clear();
            return file;
        }

        // Only an attempt taken by a previous or concurrent run moves on to the next number
        if(_error != std::errc::file_exists)
        {
            return std::nullopt;
        }
    }

    _error = std::make_error_code(std::errc::file_exists);
    return std::nullopt;
}

} // namespace sight::core::log::detail::test_log
