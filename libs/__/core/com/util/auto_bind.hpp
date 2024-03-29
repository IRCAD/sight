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

#include "core/com/util/convert_function_type.hpp"

#include <functional>

/**
 * @brief fwCom
 */
namespace sight::core::com::util
{

/**
 * @brief Automatic binding class.
 * This class allow to bind automatically a function/method with the right number of arguments.
 *
 * @tparam F Desired binding signature.
 * @tparam PLACEHOLDERS_NB number of placeholders to use.
 */
template<typename F, int PLACEHOLDERS_NB>
struct auto_bind;

/// AutoBind specialization.
template<typename F>
struct auto_bind<F, 0>
{
    using function_type = typename convert_function_type<F>::type;

    template<typename W, typename ... A>
    static std::function<function_type> wrap(W _f, A ... _a);
};

/// AutoBind specialization.
template<typename F>
struct auto_bind<F, 1>
{
    using function_type = typename convert_function_type<F>::type;

    template<typename W, typename ... A>
    static std::function<function_type> wrap(W _f, A ... _a);
};

/// AutoBind specialization.
template<typename F>
struct auto_bind<F, 2>
{
    using function_type = typename convert_function_type<F>::type;

    template<typename W, typename ... A>
    static std::function<function_type> wrap(W _f, A ... _a);
};

/// AutoBind specialization.
template<typename F>
struct auto_bind<F, 3>
{
    using function_type = typename convert_function_type<F>::type;

    template<typename W, typename ... A>
    static std::function<function_type> wrap(W _f, A ... _a);
};

/// AutoBind specialization.
template<typename F>
struct auto_bind<F, 4>
{
    using function_type = typename convert_function_type<F>::type;

    template<typename W, typename ... A>
    static std::function<function_type> wrap(W _f, A ... _a);
};

/// AutoBind specialization.
template<typename F>
struct auto_bind<F, 5>
{
    using function_type = typename convert_function_type<F>::type;

    template<typename W, typename ... A>
    static std::function<function_type> wrap(W _f, A ... _a);
};

/// AutoBind specialization.
template<typename F>
struct auto_bind<F, 6>
{
    using function_type = typename convert_function_type<F>::type;

    template<typename W, typename ... A>
    static std::function<function_type> wrap(W _f, A ... _a);
};

/// AutoBind specialization.
template<typename F>
struct auto_bind<F, 7>
{
    using function_type = typename convert_function_type<F>::type;

    template<typename W, typename ... A>
    static std::function<function_type> wrap(W _f, A ... _a);
};

/// AutoBind specialization.
template<typename F>
struct auto_bind<F, 8>
{
    using function_type = typename convert_function_type<F>::type;

    template<typename W, typename ... A>
    static std::function<function_type> wrap(W _f, A ... _a);
};

/// AutoBind specialization.
template<typename F>
struct auto_bind<F, 9>
{
    using function_type = typename convert_function_type<F>::type;

    template<typename W, typename ... A>
    static std::function<function_type> wrap(W _f, A ... _a);
};

/**
 * @brief Automatic bind of given function
 *
 * @return a boost function of type F if F is a function type, of type *F if F is a function pointer,
 * or of the corresponding function type if F is a member method pointer (like R (C::*)(A1, A2, ..))
 * If F is a member method pointer, autobind takes the object on which we desire to call the method
 * as second argument.
 */
template<typename F, typename ... A>
std::function<typename convert_function_type<F>::type> autobind(F _f, A ... _a);

} // namespace sight::core::com::util
