/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include <iostream>
#include <sstream>
#include <string>

namespace sight::core
{

class SIGHT_CORE_CLASS_API string final
{
public:

    /// @brief Concatenate the given arguments into a string using the given separator.
    /// @note This method uses variadic template as argument to concatenate.
    /// @param[in] _separator : the separator to use. If the separator is an empty string, it will be ignored.
    /// @param[in] _first : the first element to concatenate.
    /// @param[in] _args : the other elements to concatenate.
    template<typename S, typename T, typename ... Args>
    static std::string join(const S& _separator, const T& _first, const Args& ... _args);

    template<typename T, typename ... Args>
    static std::string concat(const T& _first, const Args& ... _args);

private:

    template<typename S, typename T>
    static void join_impl(std::ostringstream& _stream, const S& _separator, const T& _t);

    template<typename S, typename T, typename ... Args>
    static void join_impl(
        std::ostringstream& _stream,
        const S& _separator,
        const T& _first,
        const Args& ... _args
    );
};

//------------------------------------------------------------------------------

template<typename S, typename T>
void string::join_impl(std::ostringstream& _stream, const S& _separator, const T& _current)
{
    // By default, we ignore empty elements.
    bool skip = false;

    // Nullptr is skipped.
    if constexpr(std::is_null_pointer_v<T>)
    {
        skip = true;
    }
    // Empty string is skipped.
    else if constexpr(std::is_base_of_v<std::string, T>|| std::is_base_of_v<std::string, T>)
    {
        skip = _current.empty();
    }
    // Empty char* like is skipped.
    else if constexpr(std::is_same_v<typename std::decay<T>::type, char*>
                      || std::is_same_v<typename std::decay<T>::type, const char*>)
    {
        if constexpr(std::is_pointer_v<T>)
        {
            skip = _current == nullptr || _current[0] == '\0';
        }
        else
        {
            skip = _current[0] == '\0';
        }
    }

    if(!skip)
    {
        bool skip_separator = false;

        // If the separator is not null, we add it if not the first elements.
        // Of course, this is still a noop if the separator is empty.
        if constexpr(std::is_null_pointer_v<S>)
        {
            skip_separator = true;
        }
        else if constexpr(std::is_base_of_v<std::string, S>|| std::is_base_of_v<std::string, S>)
        {
            skip_separator = _separator.empty();
        }
        else if constexpr(std::is_same_v<typename std::decay<S>::type, char*>
                          || std::is_same_v<typename std::decay<S>::type, const char*>)
        {
            if constexpr(std::is_pointer_v<S>)
            {
                skip_separator = _separator == nullptr || _separator[0] == '\0';
            }
            else
            {
                skip_separator = _separator[0] == '\0';
            }
        }

        // Add the separator if necessary (not for the first element).
        if(!skip_separator && _stream.tellp() != 0)
        {
            _stream << _separator;
        }

        // Concatenate the current element.
        _stream << _current;
    }
}

//------------------------------------------------------------------------------

template<typename S, typename T, typename ... Args>
void string::join_impl(
    std::ostringstream& _stream,
    const S& _separator,
    const T& _first,
    const Args& ... _args
)
{
    // Concatenate the first element.
    join_impl(_stream, _separator, _first);

    // Recursively concatenate the next elements.
    join_impl(_stream, _separator, _args ...);
}

//------------------------------------------------------------------------------

template<typename S, typename T, typename ... Args>
std::string string::join(const S& _separator, const T& _first, const Args& ... _args)
{
    // Create the stream buffer to concatenate the elements.
    std::ostringstream stream;
    join_impl(stream, _separator, _first, _args ...);

    // Return the concatenated string.
    return stream.str();
}

//------------------------------------------------------------------------------

template<typename T, typename ... Args>
std::string string::concat(const T& _first, const Args& ... _args)
{
    // Join with an empty/null separator.
    return join(nullptr, _first, _args ...);
}

} // namespace sight::core
