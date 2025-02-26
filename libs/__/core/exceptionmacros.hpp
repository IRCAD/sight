/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

# include "core/exception.hpp"
# include "core/spy_log.hpp"
# include "core/demangler.hpp"

//cspell: disable
# define LOG_FWEXCEPTIONS 1

# define FWCORE_EXCEPTION_CLASS sight::core::exception

# define FWCORE_EXCEPTION_PREPARE_MSG(var, msg) \
        std::stringstream var; \
        var << msg

# define FWCORE_EXCEPTION_GETCLASSNAME(obj) \
        sight::core::demangler(typeid(obj)).get_classname()

# define FWCORE_EXCEPTION_INFO(excep) \
        "Exception: <" \
        << FWCORE_EXCEPTION_GETCLASSNAME(excep) \
        << ">: " << excep.what()

// --------------------------------------------------------------------------------

# ifdef LOG_FWEXCEPTIONS
// Copy the exception parameter to avoid warning C4239 when it is a rvalue
#  define FWCORE_EXCEPTION_LOG(excep) \
        auto BOOST_PP_CAT(__excep, __LINE__) = excep; \
        SIGHT_WARN(FWCORE_EXCEPTION_INFO(BOOST_PP_CAT(__excep, __LINE__)))
# else
# define FWCORE_EXCEPTION_LOG(excep)
# endif

// --------------------------------------------------------------------------------

# define FWCORE_EXCEPT_RAISE_EXCEPTION(excep) \
        FWCORE_EXCEPTION_LOG(excep); \
        BOOST_THROW_EXCEPTION(excep)

# define FWCORE_EXCEPT_RAISE_EXCEPTION_MSG(excep_class, msg) \
        FWCORE_EXCEPTION_PREPARE_MSG(_fwcore_exception_msgstream, msg); \
        FWCORE_EXCEPT_RAISE_EXCEPTION( \
            excep_class(_fwcore_exception_msgstream.str()) \
        )

# define FWCORE_EXCEPT_RAISE(msg) \
        FWCORE_EXCEPT_RAISE_EXCEPTION_MSG(FWCORE_EXCEPTION_CLASS, msg)

# define FWCORE_EXCEPT_RAISE_EXCEPTION_IF(excep, cond) \
        FWCORE_IF(cond, FWCORE_EXCEPT_RAISE_EXCEPTION(excep); )

# define FWCORE_EXCEPT_RAISE_IF(msg, cond) \
        FWCORE_IF(cond, FWCORE_EXCEPT_RAISE(msg); )

# define FWCORE_EXCEPT_FORWARD_EXCEPTION(excep) \
        BOOST_THROW_EXCEPTION(excep)

# define FWCORE_EXCEPT_FORWARD_EXCEPTION_IF(excep, cond) \
        FWCORE_IF(cond, BOOST_THROW_EXCEPTION(excep); )

// -----------------------------------------------------------------------------

# ifdef FWEXCEPTIONS_AS_ASSERTS

#  undef FWCORE_EXCEPT_RAISE_EXCEPTION
#  define FWCORE_EXCEPT_RAISE_EXCEPTION(excep) \
        SIGHT_ASSERT(FWCORE_EXCEPTION_INFO(excep), false)

#  undef FWCORE_EXCEPT_FORWARD_EXCEPTION
#  define FWCORE_EXCEPT_FORWARD_EXCEPTION(excep) \
        SIGHT_ASSERT( \
            "[Forwarded] " << FWCORE_EXCEPTION_INFO(excep), \
            false \
        )

#  undef FWCORE_EXCEPT_FORWARD_EXCEPTION_IF
#  define FWCORE_EXCEPT_FORWARD_EXCEPTION_IF(excep, cond) \
        SIGHT_ASSERT( \
            "[Forwarded] " << FWCORE_EXCEPTION_INFO(excep), \
            cond \
        )

# endif

// -----------------------------------------------------------------------------

# define SIGHT_THROW_EXCEPTION(excep) FWCORE_EXPR_BLOCK( \
            FWCORE_EXCEPT_RAISE_EXCEPTION(excep); \
)

# define SIGHT_THROW_EXCEPTION_MSG(excep_class, msg) FWCORE_EXPR_BLOCK( \
            FWCORE_EXCEPT_RAISE_EXCEPTION_MSG(excep_class, msg); \
)

# define SIGHT_THROW(msg) FWCORE_EXPR_BLOCK( \
            FWCORE_EXCEPT_RAISE(msg); \
)

# define SIGHT_THROW_EXCEPTION_IF(excep, cond) FWCORE_EXPR_BLOCK( \
            FWCORE_EXCEPT_RAISE_EXCEPTION_IF(excep, cond) \
)

# define SIGHT_THROW_IF(msg, cond) FWCORE_EXPR_BLOCK( \
            FWCORE_EXCEPT_RAISE_IF(msg, cond) \
)

# define FW_FORWARD_EXCEPTION(excep) FWCORE_EXPR_BLOCK( \
            FWCORE_EXCEPT_FORWARD_EXCEPTION(excep); \
)

# define FW_FORWARD_EXCEPTION_IF(excep, cond) FWCORE_EXPR_BLOCK( \
            FWCORE_EXCEPT_FORWARD_EXCEPTION_IF(excep, cond); \
)
//cspell: enable
