/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

# include <boost/exception/exception.hpp>
# include <boost/throw_exception.hpp>
# include <boost/preprocessor/cat.hpp>

# include "core/Exception.hpp"
# include "core/spyLog.hpp"
# include "core/Demangler.hpp"

//cspell: disable
# define LOG_FWEXCEPTIONS 1

# define __FWCORE_EXCEPTION_CLASS sight::core::Exception

# define __FWCORE_EXCEPTION_PREPARE_MSG(var, msg) \
    std::stringstream var; \
    var << msg

# define __FWCORE_EXCEPTION_GETCLASSNAME(obj) \
    sight::core::Demangler(typeid(obj)).getClassname()

# define __FWCORE_EXCEPTION_INFO(excep) \
    "Exception: <" \
    << __FWCORE_EXCEPTION_GETCLASSNAME(excep) \
    << ">: " << excep.what()

// --------------------------------------------------------------------------------

# ifdef LOG_FWEXCEPTIONS
// Copy the exception parameter to avoid warning C4239 when it is a rvalue
#  define __FWCORE_EXCEPTION_LOG(excep) \
    auto BOOST_PP_CAT(__excep, __LINE__) = excep; \
    SIGHT_WARN(__FWCORE_EXCEPTION_INFO(BOOST_PP_CAT(__excep, __LINE__)))
# else
# define __FWCORE_EXCEPTION_LOG(excep)
# endif

// --------------------------------------------------------------------------------

# define __FWCORE_EXCEPT_RAISE_EXCEPTION(excep) \
    __FWCORE_EXCEPTION_LOG(excep); \
    BOOST_THROW_EXCEPTION(excep)

# define __FWCORE_EXCEPT_RAISE_EXCEPTION_MSG(excep_class, msg) \
    __FWCORE_EXCEPTION_PREPARE_MSG(_fwcore_exception_msgstream, msg); \
    __FWCORE_EXCEPT_RAISE_EXCEPTION( \
        excep_class(_fwcore_exception_msgstream.str()) \
    )

# define __FWCORE_EXCEPT_RAISE(msg) \
    __FWCORE_EXCEPT_RAISE_EXCEPTION_MSG(__FWCORE_EXCEPTION_CLASS, msg)

# define __FWCORE_EXCEPT_RAISE_EXCEPTION_IF(excep, cond) \
    __FWCORE_IF(cond, __FWCORE_EXCEPT_RAISE_EXCEPTION(excep); )

# define __FWCORE_EXCEPT_RAISE_IF(msg, cond) \
    __FWCORE_IF(cond, __FWCORE_EXCEPT_RAISE(msg); )

# define __FWCORE_EXCEPT_FORWARD_EXCEPTION(excep) \
    BOOST_THROW_EXCEPTION(excep)

# define __FWCORE_EXCEPT_FORWARD_EXCEPTION_IF(excep, cond) \
    __FWCORE_IF(cond, BOOST_THROW_EXCEPTION(excep); )

// -----------------------------------------------------------------------------

# ifdef FWEXCEPTIONS_AS_ASSERTS

#  undef __FWCORE_EXCEPT_RAISE_EXCEPTION
#  define __FWCORE_EXCEPT_RAISE_EXCEPTION(excep) \
    SIGHT_ASSERT(__FWCORE_EXCEPTION_INFO(excep), false)

#  undef __FWCORE_EXCEPT_FORWARD_EXCEPTION
#  define __FWCORE_EXCEPT_FORWARD_EXCEPTION(excep) \
    SIGHT_ASSERT( \
        "[Forwarded] " << __FWCORE_EXCEPTION_INFO(excep), \
        false \
    )

#  undef __FWCORE_EXCEPT_FORWARD_EXCEPTION_IF
#  define __FWCORE_EXCEPT_FORWARD_EXCEPTION_IF(excep, cond) \
    SIGHT_ASSERT( \
        "[Forwarded] " << __FWCORE_EXCEPTION_INFO(excep), \
        cond \
    )

# endif

// -----------------------------------------------------------------------------

# define SIGHT_THROW_EXCEPTION(excep) __FWCORE_EXPR_BLOCK( \
        __FWCORE_EXCEPT_RAISE_EXCEPTION(excep); \
)

# define SIGHT_THROW_EXCEPTION_MSG(excep_class, msg) __FWCORE_EXPR_BLOCK( \
        __FWCORE_EXCEPT_RAISE_EXCEPTION_MSG(excep_class, msg); \
)

# define SIGHT_THROW(msg) __FWCORE_EXPR_BLOCK( \
        __FWCORE_EXCEPT_RAISE(msg); \
)

# define SIGHT_THROW_EXCEPTION_IF(excep, cond) __FWCORE_EXPR_BLOCK( \
        __FWCORE_EXCEPT_RAISE_EXCEPTION_IF(excep, cond) \
)

# define SIGHT_THROW_IF(msg, cond) __FWCORE_EXPR_BLOCK( \
        __FWCORE_EXCEPT_RAISE_IF(msg, cond) \
)

# define FW_FORWARD_EXCEPTION(excep) __FWCORE_EXPR_BLOCK( \
        __FWCORE_EXCEPT_FORWARD_EXCEPTION(excep); \
)

# define FW_FORWARD_EXCEPTION_IF(excep, cond) __FWCORE_EXPR_BLOCK( \
        __FWCORE_EXCEPT_FORWARD_EXCEPTION_IF(excep, cond); \
)
//cspell: enable
