/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWCORE_EXCEPTIONMACROS_HPP__
#define __FWCORE_EXCEPTIONMACROS_HPP__

# include <boost/exception/exception.hpp>
# include <boost/throw_exception.hpp>

# include "fwCore/Exception.hpp"
# include "fwCore/spyLog.hpp"
# include "fwCore/Demangler.hpp"

# define LOG_FWEXCEPTIONS 1

# define __FWCORE_EXCEPTION_CLASS ::fwCore::Exception

# define __FWCORE_EXCEPTION_PREPARE_MSG(var, msg)       \
    std::stringstream var;                              \
    var << msg

# define __FWCORE_EXCEPTION_GETCLASSNAME(obj)           \
    ::fwCore::Demangler(typeid(obj)).getClassname()

# define __FWCORE_EXCEPTION_INFO(excep)         \
    "Exception: <"                              \
        << __FWCORE_EXCEPTION_GETCLASSNAME(excep)   \
        << ">: " << excep.what()

// --------------------------------------------------------------------------------

# ifdef LOG_FWEXCEPTIONS
#  define __FWCORE_EXCEPTION_LOG(excep)         \
    OSLM_WARN(__FWCORE_EXCEPTION_INFO(excep))
# else
# define __FWCORE_EXCEPTION_LOG(excep)
# endif

// --------------------------------------------------------------------------------

# define __FWCORE_EXCEPT_RAISE_EXCEPTION(excep) \
    __FWCORE_EXCEPTION_LOG(excep);              \
    BOOST_THROW_EXCEPTION(excep)

# define __FWCORE_EXCEPT_RAISE_EXCEPTION_MSG(excep_class, msg)          \
    __FWCORE_EXCEPTION_PREPARE_MSG(_fwcore_exception_msgstream, msg);   \
    __FWCORE_EXCEPT_RAISE_EXCEPTION(                                    \
        excep_class(_fwcore_exception_msgstream.str()))

# define __FWCORE_EXCEPT_RAISE(msg)                                     \
    __FWCORE_EXCEPT_RAISE_EXCEPTION_MSG(__FWCORE_EXCEPTION_CLASS, msg)

# define __FWCORE_EXCEPT_RAISE_EXCEPTION_IF(excep, cond)        \
    __FWCORE_IF(cond, __FWCORE_EXCEPT_RAISE_EXCEPTION(excep); )

# define __FWCORE_EXCEPT_RAISE_IF(msg, cond)            \
    __FWCORE_IF(cond, __FWCORE_EXCEPT_RAISE(msg); )

# define __FWCORE_EXCEPT_FORWARD_EXCEPTION(excep)       \
    BOOST_THROW_EXCEPTION(excep)

# define __FWCORE_EXCEPT_FORWARD_EXCEPTION_IF(excep, cond)    \
    __FWCORE_IF(cond, BOOST_THROW_EXCEPTION(excep); )

// -----------------------------------------------------------------------------

# ifdef FWEXCEPTIONS_AS_ASSERTS

#  undef __FWCORE_EXCEPT_RAISE_EXCEPTION
#  define __FWCORE_EXCEPT_RAISE_EXCEPTION(excep)        \
    OSLM_ASSERT(__FWCORE_EXCEPTION_INFO(excep), false)

#  undef __FWCORE_EXCEPT_FORWARD_EXCEPTION
#  define __FWCORE_EXCEPT_FORWARD_EXCEPTION(excep)           \
    OSLM_ASSERT(                                             \
        "[Forwarded] " << __FWCORE_EXCEPTION_INFO (excep),   \
        false)

#  undef __FWCORE_EXCEPT_FORWARD_EXCEPTION_IF
#  define __FWCORE_EXCEPT_FORWARD_EXCEPTION_IF(excep, cond)     \
    OSLM_ASSERT(                                                \
        "[Forwarded] " << __FWCORE_EXCEPTION_INFO (excep),      \
        cond)

# endif

// -----------------------------------------------------------------------------

# define FW_RAISE_EXCEPTION(excep)  __FWCORE_EXPR_BLOCK(        \
        __FWCORE_EXCEPT_RAISE_EXCEPTION(excep);                 \
        )

# define FW_RAISE_EXCEPTION_MSG(excep_class, msg) __FWCORE_EXPR_BLOCK(  \
        __FWCORE_EXCEPT_RAISE_EXCEPTION_MSG(excep_class, msg);          \
        )

# define FW_RAISE(msg) __FWCORE_EXPR_BLOCK(     \
        __FWCORE_EXCEPT_RAISE(msg);             \
        )

# define FW_RAISE_EXCEPTION_IF(excep, cond) __FWCORE_EXPR_BLOCK(        \
        __FWCORE_EXCEPT_RAISE_EXCEPTION_IF(excep, cond)                 \
        )

# define FW_RAISE_IF(msg, cond) __FWCORE_EXPR_BLOCK(    \
        __FWCORE_EXCEPT_RAISE_IF(msg, cond)             \
        )

# define FW_FORWARD_EXCEPTION(excep) __FWCORE_EXPR_BLOCK(       \
        __FWCORE_EXCEPT_FORWARD_EXCEPTION(excep);               \
        )

# define FW_FORWARD_EXCEPTION_IF(excep, cond) __FWCORE_EXPR_BLOCK(       \
        __FWCORE_EXCEPT_FORWARD_EXCEPTION_IF(excep, cond);               \
        )

#endif  // __FWCORE_EXCEPTIONMACROS_HPP__
