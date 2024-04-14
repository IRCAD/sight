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

/**
 * @file spy_log.hpp
 * @brief This file defines SpyLog macros.
 * These macros are used to log messages to a file or to the console during
 * application execution.
 *
 * Six log levels are defined :
 * -# Fatal
 * -# Error
 * -# Warning
 * -# Info
 * -# Debug
 * -# Trace
 *
 * Log level is set by defining SPYLOG_LEVEL to N where 0 < N <= 6. If log
 * level is set to N, every log level lesser than N will be enabled, the other
 * macro will be define but won't have any effect.
 *
 * Each log level macro can accept strings or stringstreams:
 *   - Example : SIGHT_INFO( "Count : " << i );
 *
 * FATAL macros have a particular behavior : the application is aborted after
 * the message was logged.
 *
 *
 */

#pragma once

#ifdef _DEBUG
    #ifdef WIN32
        #define DEBUG_BREAK() __debugbreak()
    #else
        #include <csignal>
        #define DEBUG_BREAK() do{std::ignore = std::raise(SIGTRAP); std::abort();}while(0)
    #endif

    #define SPYLOG_ABORT() DEBUG_BREAK()
#else
    #define SPYLOG_ABORT() std::abort()
#endif

#include <cassert>
#include <cstring>
#include <sstream>

# include <core/log/spy_logger.hpp>

// -----------------------------------------------------------------------------
constexpr static const char* strip_source_path(const char* const _path)
{
#ifndef SIGHT_SOURCE_DIR
    return path;
#else
    if(_path[0] == '.')
    {
        // If the path is relative, we return the path since it's a nightmare to deal with paths relative to the working
        // directory of the currently running compiler..
        // This is maybe doable by passing the working directory of the currently running compiler as a define and
        // resolve by hand the relative path ...in pure constexpr ! Good luck...
        return _path;
    }

    // Otherwise, we assume the path is absolute
    // We check if the path belongs to sight source
    constexpr std::size_t sight_source_length = sizeof(SIGHT_SOURCE_DIR) - 1;

    // If the path does not start with the sight source directory, we return the path
    for(std::size_t i = 0 ; i < sight_source_length ; ++i)
    {
        const auto path_c   = _path[i];
        const auto source_c = SIGHT_SOURCE_DIR[i];

        if(path_c == '\0')
        {
            // If the path is shorter than the sight source directory, we return the path
            return _path;
        }

        if((path_c == '/' || path_c == '\\') && source_c != '/' && source_c != '\\')
        {
            // If current path character is a path deliminator and the current source character is not, we return path
            return _path;
        }

        if(path_c != '/' && path_c != '\\' && (source_c == '/' || source_c == '\\'))
        {
            // If path character is not a path deliminator and the current source character is one, we return path
            return _path;
        }

        if(path_c != '/' && path_c != '\\' && source_c != '/' && source_c != '\\' && path_c != source_c)
        {
            // If path character is not a path deliminator and the current source character is also not, we return path
            // if they are different
            return _path;
        }
    }

    // Otherwise, we strip the sight source directory from the path
    // /home/user/sight/libs/core/log/spy_log.hpp => libs/core/log/spy_log.hpp
    return _path + sight_source_length + 1;
#endif
}

// This force to evaluate the constexpr at compile time, even on windows without optimization
#define SIGHT_SOURCE_FILE [] () constexpr {constexpr auto stripped = strip_source_path(__FILE__); return stripped;} ()

/**
 * @cond
 */

// -----------------------------------------------------------------------------

# ifndef FWCORE_EXPR_BLOCK
#  define FWCORE_EXPR_BLOCK(expr) do{expr}while(0)
# endif

# define FWCORE_IF(cond, code) if(cond){code}

# define SL_LOG(log, loglevel, message) FWCORE_EXPR_BLOCK( \
        std::stringstream osl_str; \
        osl_str << message; \
        log.loglevel(osl_str.str(), SIGHT_SOURCE_FILE, __LINE__); \
)

// -----------------------------------------------------------------------------

#define SL_TRACE(log, message) SL_LOG(log, trace, message);
#define SL_TRACE_IF(log, message, cond) FWCORE_IF(cond, SL_LOG(log, trace, message); )

#define SL_DEBUG(log, message) SL_LOG(log, debug, message);
#define SL_DEBUG_IF(log, message, cond) FWCORE_IF(cond, SL_LOG(log, debug, message); )

#define SL_INFO(log, message) SL_LOG(log, info, message);
#define SL_INFO_IF(log, message, cond) FWCORE_IF(cond, SL_LOG(log, info, message); )

#define SL_WARN(log, message) SL_LOG(log, warn, message);
#define SL_WARN_IF(log, message, cond) FWCORE_IF(cond, SL_LOG(log, warn, message); )

#define SL_ERROR(log, message) SL_LOG(log, error, message);
#define SL_ERROR_IF(log, message, cond) FWCORE_IF(cond, SL_LOG(log, error, message); )

#define SL_FATAL(log, message) SL_LOG(log, fatal, message); SPYLOG_ABORT();
#define SL_FATAL_IF(log, message, cond) FWCORE_IF(cond, SL_FATAL(log, message); )

// -----------------------------------------------------------------------------

# ifdef _DEBUG
#  ifdef WIN32
#  define SL_ASSERT_IMPL(log, message, cond) FWCORE_EXPR_BLOCK( \
        FWCORE_IF( \
            !(cond), \
            std::stringstream osl_str1; \
            osl_str1 << "Assertion '" \
            <<#cond << "' failed.\n" << message; \
            log.fatal(osl_str1.str(), SIGHT_SOURCE_FILE, __LINE__); \
            _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, "%s", osl_str1.str().c_str()); \
            __debugbreak(); \
        ) \
)
#  else
#  define SL_ASSERT_IMPL(log, message, cond) FWCORE_EXPR_BLOCK( \
        FWCORE_IF( \
            !(cond), \
            std::stringstream osl_str1; \
            osl_str1 << "Assertion '" \
            <<#cond << "' failed: " << message; \
            log.fatal(osl_str1.str(), SIGHT_SOURCE_FILE, __LINE__); \
            SPYLOG_ABORT(); \
        ) \
)
#  endif

#  define SL_ASSERT(log, message, cond) FWCORE_EXPR_BLOCK( \
        FWCORE_IF( \
            !(cond), \
            std::stringstream osl_str; \
            osl_str << message; \
            SL_ASSERT_IMPL(log, osl_str.str(), cond); \
        ) \
)
# else
#  define SL_ASSERT(log, message, cond) // empty
# endif

// -----------------------------------------------------------------------------

#  define SPYLOG_SPYLOGGER \
    sight::core::log::spy_logger::get()

// -----------------------------------------------------------------------------

/**
 * @endcond
 */

/** @{ */
/** Debug message macros.  */
# define SIGHT_DEBUG(message) SL_DEBUG(SPYLOG_SPYLOGGER, message)
/** @deprecated @sight22, use SIGHT_DEBUG instead. */
# define OSIGHT_DEBUG(message) SL_DEBUG(SPYLOG_SPYLOGGER, message); OSIGHT_DEPRECATED()
/** Conditional debug message macros.  */
# define SIGHT_DEBUG_IF(message, cond) SL_DEBUG_IF(SPYLOG_SPYLOGGER, message, cond)
/** @deprecated @sight22, use SIGHT_DEBUG_IF instead. */
# define OSIGHT_DEBUG_IF(message, cond) SL_DEBUG_IF(SPYLOG_SPYLOGGER, message, cond); OSIGHT_DEPRECATED()
/**  @} */

/** @{ */
/** Info message macros.  */
# define SIGHT_INFO(message) SL_INFO(SPYLOG_SPYLOGGER, message)
/** @deprecated @sight22, use SIGHT_INFO instead. */
# define OSIGHT_INFO(message) SL_INFO(SPYLOG_SPYLOGGER, message); OSIGHT_DEPRECATED()
/** Conditional info message macros.  */
# define SIGHT_INFO_IF(message, cond) SL_INFO_IF(SPYLOG_SPYLOGGER, message, cond)
/** @deprecated @sight22, use SIGHT_INFO_IF instead. */
# define OSIGHT_INFO_IF(message, cond) SL_INFO_IF(SPYLOG_SPYLOGGER, message, cond); OSIGHT_DEPRECATED()
/**  @} */

/** @{ */
/** Warning message macros.  */
# define SIGHT_WARN(message) SL_WARN(SPYLOG_SPYLOGGER, message)
/** @deprecated @sight22, use SIGHT_WARN instead.  */
# define OSIGHT_WARN(message) SL_WARN(SPYLOG_SPYLOGGER, message); OSIGHT_DEPRECATED()
/** Conditional warning message macros.  */
# define SIGHT_WARN_IF(message, cond) SL_WARN_IF(SPYLOG_SPYLOGGER, message, cond)
/** @deprecated @sight22, use SIGHT_WARN_IF instead. */
# define OSIGHT_WARN_IF(message, cond) SL_WARN_IF(SPYLOG_SPYLOGGER, message, cond); OSIGHT_DEPRECATED()
/**  @} */

/** @{ */
/** Error message macros.  */
# define SIGHT_ERROR(message) SL_ERROR(SPYLOG_SPYLOGGER, message)
/** @deprecated @sight22, use SIGHT_ERROR instead.  */
# define OSIGHT_ERROR(message) SL_ERROR(SPYLOG_SPYLOGGER, message); OSIGHT_DEPRECATED()
/** Conditional error message macros.  */
# define SIGHT_ERROR_IF(message, cond) SL_ERROR_IF(SPYLOG_SPYLOGGER, message, cond)
/** @deprecated @sight22, use SIGHT_ERROR_IF instead.  */
# define OSIGHT_ERROR_IF(message, cond) SL_ERROR_IF(SPYLOG_SPYLOGGER, message, cond); OSIGHT_DEPRECATED()
/**  @} */

/** @{ */
/** Fatal message macros.  */
# define SIGHT_FATAL(message) SL_FATAL(SPYLOG_SPYLOGGER, message)
/** @deprecated @sight22, use SIGHT_FATAL instead.  */
# define OSIGHT_FATAL(message) SL_FATAL(SPYLOG_SPYLOGGER, message); OSIGHT_DEPRECATED()
/** Conditional fatal message macros.  */
# define SIGHT_FATAL_IF(message, cond) SL_FATAL_IF(SPYLOG_SPYLOGGER, message, cond)
/** @deprecated @sight22, use SIGHT_FATAL_IF instead.  */
# define OSIGHT_FATAL_IF(message, cond) SL_FATAL_IF(SPYLOG_SPYLOGGER, message, cond); OSIGHT_DEPRECATED()
/**  @} */

/**
 * @name Special macros
 * @{ */

/** @brief work like 'assert' from 'cassert', with in addition a message logged by
 * spylog (with FATAL loglevel)  */
# define SIGHT_ASSERT(message, cond) \
    SL_ASSERT(SPYLOG_SPYLOGGER, message, cond)
# define OSIGHT_ASSERT(message, cond) \
    SL_ASSERT(SPYLOG_SPYLOGGER, message, cond)

// -----------------------------------------------------------------------------

# ifdef __GNUC__ // with GCC
#  define SIGHT_PRETTY_FUNC() __PRETTY_FUNCTION__
# elif defined(_MSC_VER) // with MSC
#  define SIGHT_PRETTY_FUNC() __FUNCSIG__
# else
#  define SIGHT_PRETTY_FUNC() __func__
# endif
/**  @} */

/**
 * @brief Use this macro when deprecating a function to warn the developer.
 */
#define FW_DEPRECATED(old_fn_name, new_fn_name, version) \
    SIGHT_ERROR( \
        "[DEPRECATED] '" << old_fn_name << "' is deprecated and will be removed in '" << version << "', use '" \
        << new_fn_name << "' instead. It is still used by '" + this->get_classname() + "'." \
    );

/**
 * @brief Use this macro when deprecating a function to warn the developer.
 */
#define FW_DEPRECATED_IF(old_fn_name, new_fn_name, version, condition) \
    SIGHT_ERROR_IF( \
        "[DEPRECATED] '" << old_fn_name << "' is deprecated and will be removed in '" << version << "', use '" \
        << new_fn_name << "' instead. It is still used by '" + this->get_classname() + "'.", \
        condition \
    );

/**
 * @brief Use this macro when deprecating a function to warn the developer.
 */
#define FW_DEPRECATED_MSG(message, version) \
    SIGHT_ERROR("[DEPRECATED] " << message << " It will be removed in '" << version << "'");

/**
 * @brief Use this macro when deprecating a service key to warn the developer.
 */
#define FW_DEPRECATED_KEY(new_key, access, version) \
    SIGHT_ERROR( \
        "[DEPRECATED] The key '" \
        << new_key << "' is not correctly set. Please correct the configuration to set an '" \
        << access << "' key named '" << new_key << "'. The support of the old key will be removed in '" \
        << version << "'." \
    );

//------------------------------------------------------------------------------

inline void osight_deprecated()
{
    // Empty function to trigger deprecation warnings
}

//------------------------------------------------------------------------------

inline void sight_trace_deprecated()
{
    // Empty function to trigger deprecation warnings
}
