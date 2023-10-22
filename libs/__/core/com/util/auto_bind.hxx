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

// cspell:ignore NOLINTNEXTLINE

#pragma once

#include <boost/function_types/function_arity.hpp>

namespace sight::core::com::util
{

//------------------------------------------------------------------------------

template<typename F>
template<typename W, typename ... A>
std::function<typename auto_bind<F, 0>::function_type> auto_bind<F, 0>::wrap(W _f, A ... _a)
{
    // NOLINTNEXTLINE(modernize-avoid-bind)
    return std::bind(_f, _a ...);
}

//-----------------------------------------------------------------------------

template<typename F>
template<typename W, typename ... A>
std::function<typename auto_bind<F, 1>::function_type> auto_bind<F, 1>::wrap(W _f, A ... _a)
{
    // NOLINTNEXTLINE(modernize-avoid-bind)
    return std::bind(_f, _a ..., std::placeholders::_1);
}

//-----------------------------------------------------------------------------

template<typename F>
template<typename W, typename ... A>
std::function<typename auto_bind<F, 2>::function_type> auto_bind<F, 2>::wrap(W _f, A ... _a)
{
    // NOLINTNEXTLINE(modernize-avoid-bind)
    return std::bind(_f, _a ..., std::placeholders::_1, std::placeholders::_2);
}

//-----------------------------------------------------------------------------

template<typename F>
template<typename W, typename ... A>
std::function<typename auto_bind<F, 3>::function_type> auto_bind<F, 3>::wrap(W _f, A ... _a)
{
    // NOLINTNEXTLINE(modernize-avoid-bind)
    return std::bind(_f, _a ..., std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

//-----------------------------------------------------------------------------

template<typename F>
template<typename W, typename ... A>
std::function<typename auto_bind<F, 4>::function_type> auto_bind<F, 4>::wrap(W _f, A ... _a)
{
    // NOLINTNEXTLINE(modernize-avoid-bind)
    return std::bind(
        _f,
        _a ...,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4
    );
}

//-----------------------------------------------------------------------------

template<typename F>
template<typename W, typename ... A>
std::function<typename auto_bind<F, 5>::function_type> auto_bind<F, 5>::wrap(W _f, A ... _a)
{
    // NOLINTNEXTLINE(modernize-avoid-bind)
    return std::bind(
        _f,
        _a ...,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4,
        std::placeholders::_5
    );
}

//-----------------------------------------------------------------------------

template<typename F>
template<typename W, typename ... A>
std::function<typename auto_bind<F, 6>::function_type> auto_bind<F, 6>::wrap(W _f, A ... _a)
{
    // NOLINTNEXTLINE(modernize-avoid-bind)
    return std::bind(
        _f,
        _a ...,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4,
        std::placeholders::_5,
        std::placeholders::_6
    );
}

//-----------------------------------------------------------------------------

template<typename F>
template<typename W, typename ... A>
std::function<typename auto_bind<F, 7>::function_type> auto_bind<F, 7>::wrap(W _f, A ... _a)
{
    // NOLINTNEXTLINE(modernize-avoid-bind)
    return std::bind(
        _f,
        _a ...,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4,
        std::placeholders::_5,
        std::placeholders::_6,
        std::placeholders::_7
    );
}

//-----------------------------------------------------------------------------

template<typename F>
template<typename W, typename ... A>
std::function<typename auto_bind<F, 8>::function_type> auto_bind<F, 8>::wrap(W _f, A ... _a)
{
    // NOLINTNEXTLINE(modernize-avoid-bind)
    return std::bind(
        _f,
        _a ...,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4,
        std::placeholders::_5,
        std::placeholders::_6,
        std::placeholders::_7,
        std::placeholders::_8
    );
}

//-----------------------------------------------------------------------------

template<typename F>
template<typename W, typename ... A>
std::function<typename auto_bind<F, 9>::function_type> auto_bind<F, 9>::wrap(W _f, A ... _a)
{
    // NOLINTNEXTLINE(modernize-avoid-bind)
    return std::bind(
        _f,
        _a ...,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4,
        std::placeholders::_5,
        std::placeholders::_6,
        std::placeholders::_7,
        std::placeholders::_8,
        std::placeholders::_9
    );
}

//-----------------------------------------------------------------------------

template<typename F, typename ... A>
std::function<typename convert_function_type<F>::type> autobind(F _f, A ... _a)
{
    typedef typename convert_function_type<F>::type function_type;
    const int arity = boost::function_types::function_arity<function_type>::value;
    return auto_bind<F, arity>::wrap(_f, _a ...);
}

} // namespace sight::core::com::util
