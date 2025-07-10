/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <functional>
#include <type_traits>

namespace sight::core::com::util
{

template<typename F>
struct convert_function_type;

/// Convert class member method type to equivalent function type.
template<typename R, typename C, typename ... args>
struct convert_function_type<R (C::*)(args ...)>
{
    using type = R(args ...);
};

/// Convert class const member method type to equivalent function type.
template<typename R, typename C, typename ... args>
struct convert_function_type<R (C::*)(args ...) const>
{
    using type = R(args ...);
};

/// Extract function type from a boost function.
template<typename F>
struct convert_function_type<std::function<F> >
{
    using type = F;
};

/// Convert function pointer type to function type.
template<typename F>
struct convert_function_type
{
    using type = std::remove_pointer_t<F>;
};

} // namespace sight::core::com::util
