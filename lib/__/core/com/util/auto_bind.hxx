/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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
    return [... a = _a, _f](auto&& ...){return std::invoke(_f, a ...);};
}

//-----------------------------------------------------------------------------

template<typename F>
template<typename W, typename ... A>
std::function<typename auto_bind<F, 1>::function_type> auto_bind<F, 1>::wrap(W _f, A ... _a)
{
    return [... a = _a, _f](auto _p1, auto&& ...){return std::invoke(_f, a ..., _p1);};
}

//-----------------------------------------------------------------------------

template<typename F>
template<typename W, typename ... A>
std::function<typename auto_bind<F, 2>::function_type> auto_bind<F, 2>::wrap(W _f, A ... _a)
{
    return [... a = _a, _f](auto _p1, auto _p2, auto&& ...){return std::invoke(_f, a ..., _p1, _p2);};
}

//-----------------------------------------------------------------------------

template<typename F>
template<typename W, typename ... A>
std::function<typename auto_bind<F, 3>::function_type> auto_bind<F, 3>::wrap(W _f, A ... _a)
{
    return [... a = _a, _f](auto _p1, auto _p2, auto _p3, auto&& ...){return std::invoke(_f, a ..., _p1, _p2, _p3);};
}

//-----------------------------------------------------------------------------

template<typename F>
template<typename W, typename ... A>
std::function<typename auto_bind<F, 4>::function_type> auto_bind<F, 4>::wrap(W _f, A ... _a)
{
    return [... a = _a, _f](auto _p1, auto _p2, auto _p3, auto _p4, auto&& ...)
           {return std::invoke(_f, a ..., _p1, _p2, _p3, _p4);};
}

//-----------------------------------------------------------------------------

template<typename F>
template<typename W, typename ... A>
std::function<typename auto_bind<F, 5>::function_type> auto_bind<F, 5>::wrap(W _f, A ... _a)
{
    return [... a = _a, _f](auto _p1, auto _p2, auto _p3, auto _p4, auto _p5, auto&& ...)
           {return std::invoke(_f, a ..., _p1, _p2, _p3, _p4, _p5);};
}

//-----------------------------------------------------------------------------

template<typename F>
template<typename W, typename ... A>
std::function<typename auto_bind<F, 6>::function_type> auto_bind<F, 6>::wrap(W _f, A ... _a)
{
    return [... a = _a, _f](auto _p1, auto _p2, auto _p3, auto _p4, auto _p5, auto _p6, auto&& ...)
           {return std::invoke(_f, a ..., _p1, _p2, _p3, _p4, _p5, _p6);};
}

//-----------------------------------------------------------------------------

template<typename F>
template<typename W, typename ... A>
std::function<typename auto_bind<F, 7>::function_type> auto_bind<F, 7>::wrap(W _f, A ... _a)
{
    return [... a = _a, _f](auto _p1, auto _p2, auto _p3, auto _p4, auto _p5, auto _p6, auto _p7, auto&& ...)
           {return std::invoke(_f, a ..., _p1, _p2, _p3, _p4, _p5, _p6, _p7);};
}

//-----------------------------------------------------------------------------

template<typename F>
template<typename W, typename ... A>
std::function<typename auto_bind<F, 8>::function_type> auto_bind<F, 8>::wrap(W _f, A ... _a)
{
    return [... a = _a, _f](auto _p1, auto _p2, auto _p3, auto _p4, auto _p5, auto _p6, auto _p7, auto _p8, auto&& ...)
           {return std::invoke(_f, a ..., _p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8);};
}

//-----------------------------------------------------------------------------

template<typename F>
template<typename W, typename ... A>
std::function<typename auto_bind<F, 9>::function_type> auto_bind<F, 9>::wrap(W _f, A ... _a)
{
    return [... a = _a, _f](auto _p1, auto _p2, auto _p3, auto _p4, auto _p5, auto _p6, auto _p7, auto _p8, auto _p9,
                            auto&& ...)
           {return std::invoke(_f, a ..., _p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8, _p9);};
}

//-----------------------------------------------------------------------------

template<typename F, typename ... A>
std::function<typename convert_function_type<F>::type> autobind(F _f, A ... _a)
{
    using function_type = convert_function_type<F>::type;
    const int arity = boost::function_types::function_arity<function_type>::value;
    return auto_bind<F, arity>::wrap(_f, _a ...);
}

} // namespace sight::core::com::util
