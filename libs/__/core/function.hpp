/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

//-----------------------------------------------------------------------------

#pragma once

namespace sight::core
{

namespace detail
{

//------------------------------------------------------------------------------

template<typename T>
struct deduce_type;

template<typename RETURN_TYPE, typename CLASS_TYPE, typename ... ARGS>
struct deduce_type<RETURN_TYPE (CLASS_TYPE::*)(ARGS ...) const>
{
    using type = std::function<RETURN_TYPE(ARGS ...)>;
};

} // namespace detail

template<typename CLOSURE>
using lambda_to_function_t = typename detail::deduce_type<decltype(&CLOSURE::operator())>::type;

//------------------------------------------------------------------------------

/// Converts a lambda to a std::function, used to convert template arguments
template<typename CLOSURE>
auto lambda_to_function(const CLOSURE& fn)
{
    return lambda_to_function_t<CLOSURE>(fn);
}

//------------------------------------------------------------------------------

} // namespace sight::core
