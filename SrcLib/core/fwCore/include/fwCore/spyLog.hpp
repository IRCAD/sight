/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/**
 * @file spyLog.hpp
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
 * Each log level macro has two variant : SLM_<level> and OSLM_<level>.
 * - SLM_ variant is a simple log message macros, append a loglevel-tagged
 *   log message to the logger.
 *   - Example : SLM_FATAL( "This should not append" );
 * - OSLM_ variant is a stringstream log message macros, it is the same as
 *   "Simple log message macros", but accept a stream as argument.
 *   - Example : OSLM_INFO( "Count : " << i );
 *
 * FATAL macros have a particular behavior : the application is aborted after
 * the message was logged.
 *
 *
 */

#pragma once

# define SPYLOG_ABORT() std::abort()

# ifdef _DEBUG

#  ifdef WIN32
#   include <windows.h>
#   define DEBUG_BREAK() DebugBreak()
#  else
#   include <csignal>
#   define DEBUG_BREAK() std::raise(SIGTRAP)
#  endif

#  ifdef SPYLOG_ABORT
#   undef SPYLOG_ABORT
#  endif
#  define SPYLOG_ABORT() DEBUG_BREAK()

# endif

# include <cassert>
# include <sstream>

#include <boost/preprocessor/comparison/greater_equal.hpp>
#include <boost/preprocessor/control/expr_iif.hpp>

# include "fwCore/log/SpyLogger.hpp"
# include "fwCore/log/ScopedMessage.hpp"

// -----------------------------------------------------------------------------

/**
 * @cond
 */

// -----------------------------------------------------------------------------

# ifndef __FWCORE_EXPR_BLOCK
#  define __FWCORE_EXPR_BLOCK(expr) do { expr } while (0)
# endif

# define __FWCORE_IF(cond, code) if ( cond ) { code }

# define OSL_LOG(log, loglevel, message) __FWCORE_EXPR_BLOCK(   \
        std::stringstream oslStr;                               \
        oslStr << message;                                      \
        SL_ ## loglevel(log, oslStr.str());                     \
        )

#ifdef SPYLOG_LEVEL

#define __FWCORE_IF_ENABLED( level, expr ) \
    BOOST_PP_EXPR_IIF( BOOST_PP_GREATER_EQUAL(SPYLOG_LEVEL, level), expr)

#endif

// -----------------------------------------------------------------------------

#  define SL_TRACE(log, message) __FWCORE_IF_ENABLED( 6,                                        \
                                                      log.trace(message, __FILE__, __LINE__);   \
                                                      )
#  define OSL_TRACE(log, message) __FWCORE_IF_ENABLED( 6,                               \
                                                       OSL_LOG(log, TRACE, message);    \
                                                       )
#  define SL_TRACE_IF(log, message, cond) __FWCORE_IF_ENABLED( 6,                                               \
                                                               __FWCORE_IF(cond, log.trace(message, __FILE__,   \
                                                                                           __LINE__); )         \
                                                               )
#  define OSL_TRACE_IF(log, message, cond) __FWCORE_IF_ENABLED( 6,                                                  \
                                                                __FWCORE_IF(cond, OSL_LOG(log, TRACE, message); )   \
                                                                )

#  define SL_DEBUG(log, message) __FWCORE_IF_ENABLED( 5,                                        \
                                                      log.debug(message, __FILE__, __LINE__);   \
                                                      )
#  define OSL_DEBUG(log, message) __FWCORE_IF_ENABLED( 5,                             \
                                                       OSL_LOG(log, DEBUG, message);  \
                                                       )
#  define SL_DEBUG_IF(log, message, cond) __FWCORE_IF_ENABLED( 5,                                               \
                                                               __FWCORE_IF(cond, log.debug(message, __FILE__,   \
                                                                                           __LINE__); )         \
                                                               )
#define OSL_DEBUG_IF(log, message, cond) __FWCORE_IF_ENABLED( 5,                                                    \
                                                              __FWCORE_IF(cond, OSL_LOG(log, DEBUG, message); )     \
                                                              )

#define SL_INFO(log, message) __FWCORE_IF_ENABLED( 4,                                       \
                                                   log.info(message, __FILE__, __LINE__);   \
                                                   )
#define OSL_INFO(log, message) __FWCORE_IF_ENABLED( 4,                              \
                                                    OSL_LOG(log, INFO, message);    \
                                                    )
#define SL_INFO_IF(log, message, cond) __FWCORE_IF_ENABLED( 4,                                                         \
                                                            __FWCORE_IF(cond, log.info(message, __FILE__, __LINE__); ) \
                                                            )
#define OSL_INFO_IF(log, message, cond) __FWCORE_IF_ENABLED( 4,                                                \
                                                             __FWCORE_IF(cond, OSL_LOG(log, INFO, message); )  \
                                                             )

#define SL_WARN(log, message) __FWCORE_IF_ENABLED( 3,                                       \
                                                   log.warn(message, __FILE__, __LINE__);   \
                                                   )
#define OSL_WARN(log, message) __FWCORE_IF_ENABLED( 3,                              \
                                                    OSL_LOG(log, WARN, message);    \
                                                    )
#define SL_WARN_IF(log, message, cond) __FWCORE_IF_ENABLED( 3,                                                         \
                                                            __FWCORE_IF(cond, log.warn(message, __FILE__, __LINE__); ) \
                                                            )
#define OSL_WARN_IF(log, message, cond) __FWCORE_IF_ENABLED( 3,                                                 \
                                                             __FWCORE_IF(cond, OSL_LOG(log, WARN, message); )   \
                                                             )

#define SL_ERROR(log, message) __FWCORE_IF_ENABLED( 2,                                      \
                                                    log.error(message, __FILE__, __LINE__); \
                                                    )
#define OSL_ERROR(log, message) __FWCORE_IF_ENABLED( 2,                             \
                                                     OSL_LOG(log, ERROR, message);  \
                                                     )
#define SL_ERROR_IF(log, message, cond) __FWCORE_IF_ENABLED( 2,                                                          \
                                                             __FWCORE_IF(cond, log.error(message, __FILE__, __LINE__); ) \
                                                             )
#define OSL_ERROR_IF(log, message, cond) __FWCORE_IF_ENABLED( 2,                                                \
                                                              __FWCORE_IF(cond, OSL_LOG(log, ERROR, message); ) \
                                                              )

#define SL_FATAL(log, message) __FWCORE_IF_ENABLED( 1,                                             \
                                                    log.fatal(message, __FILE__, __LINE__);        \
                                                    SPYLOG_ABORT();                                \
                                                    )
#define OSL_FATAL(log, message) __FWCORE_IF_ENABLED( 1,                              \
                                                     OSL_LOG(log, FATAL, message);   \
                                                     SPYLOG_ABORT();                 \
                                                     )
#define SL_FATAL_IF(log, message, cond) __FWCORE_IF_ENABLED( 1,                                             \
                                                             __FWCORE_IF(cond, SL_FATAL(log, message); )    \
                                                             )
#define OSL_FATAL_IF(log, message, cond) __FWCORE_IF_ENABLED( 1,                                            \
                                                              __FWCORE_IF(cond, OSL_FATAL(log, message); )  \
                                                              )

// -----------------------------------------------------------------------------

# ifdef _DEBUG
#  ifdef WIN32
#  define SL_ASSERT(log, message, cond) __FWCORE_EXPR_BLOCK(                                            \
        __FWCORE_IF(!(cond),                                                                            \
                    std::stringstream oslStr1;                                                          \
                    oslStr1 << "Assertion '" <<                                                         \
                    #cond << "' failed.\n" << message;                                                  \
                    log.fatal(oslStr1.str(), __FILE__, __LINE__);                                       \
                    _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, "%s", oslStr1.str().c_str());  \
                    __debugbreak();                                                                     \
                    ))
#  else
#  define SL_ASSERT(log, message, cond) __FWCORE_EXPR_BLOCK(            \
        __FWCORE_IF(!(cond),                                            \
                    std::stringstream oslStr1;                          \
                    oslStr1 << "Assertion '" <<                         \
                    #cond << "' failed: " << message;                   \
                    log.fatal(oslStr1.str(), __FILE__, __LINE__);       \
                    SPYLOG_ABORT();                                     \
                    ))
#  endif

#  define OSL_ASSERT(log, message, cond) __FWCORE_EXPR_BLOCK(   \
        __FWCORE_IF(!(cond),                                    \
                    std::stringstream oslStr;                   \
                    oslStr << message;                          \
                    SL_ASSERT(log, oslStr.str(), cond);         \
                    ))
# else
#  define SL_ASSERT(log, message, cond) // empty
#  define OSL_ASSERT(log, message, cond) // empty
# endif

// -----------------------------------------------------------------------------

#  define _SPYLOG_SPYLOGGER_            \
    ::fwCore::log::SpyLogger::getSpyLogger()

// -----------------------------------------------------------------------------

/**
 * @endcond
 */

/** @{ */
/** Trace message macros.  */
# define SLM_TRACE(message) SL_TRACE(_SPYLOG_SPYLOGGER_, message)
/** Trace stringstream message macros.  */
# define OSLM_TRACE(message) OSL_TRACE(_SPYLOG_SPYLOGGER_, message)
/** Conditionnal trace message macros.  */
# define SLM_TRACE_IF(message, cond) SL_TRACE_IF(_SPYLOG_SPYLOGGER_, message, cond)
/** Conditionnal trace stringstream message macros.  */
# define OSLM_TRACE_IF(message, cond) OSL_TRACE_IF(_SPYLOG_SPYLOGGER_, message, cond)
/**  @} */

/** @{ */
/** Debug message macros.  */
# define SLM_DEBUG(message) SL_DEBUG(_SPYLOG_SPYLOGGER_, message)
/** Debug stringstream message macros.  */
# define OSLM_DEBUG(message) OSL_DEBUG(_SPYLOG_SPYLOGGER_, message)
/** Conditionnal debug message macros.  */
# define SLM_DEBUG_IF(message, cond) SL_DEBUG_IF(_SPYLOG_SPYLOGGER_, message, cond)
/** Conditionnal debug stringstream message macros.  */
# define OSLM_DEBUG_IF(message, cond) OSL_DEBUG_IF(_SPYLOG_SPYLOGGER_, message, cond)
/**  @} */

/** @{ */
/** Info message macros.  */
# define SLM_INFO(message) SL_INFO(_SPYLOG_SPYLOGGER_, message)
/** Info stringstream message macros.  */
# define OSLM_INFO(message) OSL_INFO(_SPYLOG_SPYLOGGER_, message)
/** Conditionnal info message macros.  */
# define SLM_INFO_IF(message, cond) SL_INFO_IF(_SPYLOG_SPYLOGGER_, message, cond)
/** Conditionnal info stringstream message macros.  */
# define OSLM_INFO_IF(message, cond) OSL_INFO_IF(_SPYLOG_SPYLOGGER_, message, cond)
/**  @} */

/** @{ */
/** Warning message macros.  */
# define SLM_WARN(message) SL_WARN(_SPYLOG_SPYLOGGER_, message)
/** Warning stringstream message macros.  */
# define OSLM_WARN(message) OSL_WARN(_SPYLOG_SPYLOGGER_, message)
/** Conditionnal warning message macros.  */
# define SLM_WARN_IF(message, cond) SL_WARN_IF(_SPYLOG_SPYLOGGER_, message, cond)
/** Conditionnal warning stringstream message macros.  */
# define OSLM_WARN_IF(message, cond) OSL_WARN_IF(_SPYLOG_SPYLOGGER_, message, cond)
/**  @} */

/** @{ */
/** Error message macros.  */
# define SLM_ERROR(message) SL_ERROR(_SPYLOG_SPYLOGGER_, message)
/** Error stringstream message macros.  */
# define OSLM_ERROR(message) OSL_ERROR(_SPYLOG_SPYLOGGER_, message)
/** Conditionnal error message macros.  */
# define SLM_ERROR_IF(message, cond) SL_ERROR_IF(_SPYLOG_SPYLOGGER_, message, cond)
/** Conditionnal error stringstream message macros.  */
# define OSLM_ERROR_IF(message, cond) OSL_ERROR_IF(_SPYLOG_SPYLOGGER_, message, cond)
/**  @} */

/** @{ */
/** Fatal message macros.  */
# define SLM_FATAL(message) SL_FATAL(_SPYLOG_SPYLOGGER_, message)
/** Fatal stringstream message macros.  */
# define OSLM_FATAL(message) OSL_FATAL(_SPYLOG_SPYLOGGER_, message)
/** Conditionnal fatal message macros.  */
# define SLM_FATAL_IF(message, cond) SL_FATAL_IF(_SPYLOG_SPYLOGGER_, message, cond)
/** Conditionnal fatal stringstream message macros.  */
# define OSLM_FATAL_IF(message, cond) OSL_FATAL_IF(_SPYLOG_SPYLOGGER_, message, cond)
/**  @} */

/** @{ */
/** Log message macros.  */
#define OSLM_LOG(message)                                          \
    __FWCORE_EXPR_BLOCK(                                      \
        std::stringstream stream;                                  \
        stream << message;                                         \
        _SPYLOG_SPYLOGGER_.log(stream.str(), __FILE__, __LINE__);  \
        )
/**  @} */

/**
 * @name Special macros
 * @{ */

/** @brief work like 'assert' from 'cassert', with in addition a message logged by
 * spylog (with FATAL loglevel)  */
# define SLM_ASSERT(message, cond)                      \
    SL_ASSERT(_SPYLOG_SPYLOGGER_, message, cond)
# define OSLM_ASSERT(message, cond)                     \
    OSL_ASSERT(_SPYLOG_SPYLOGGER_, message, cond)

// -----------------------------------------------------------------------------

# ifdef __GNUC__                // with GCC
#  define SLM_PRETTY_FUNC() __PRETTY_FUNCTION__
# elif defined(_MSC_VER)        // with MSC
#  define SLM_PRETTY_FUNC() __FUNCSIG__
# else
#  define SLM_PRETTY_FUNC() __func__
# endif

/**
 * @brief Trace contextual function signature.
 *
 * Generate a log trace message with the (contextual) function signature.
 */
# ifndef SPYLOG_TIMER
#  define SLM_TRACE_FUNC() SLM_TRACE(SLM_PRETTY_FUNC())
# else

# define SLM_TRACE_FUNC() __FWCORE_EXPR_BLOCK(                  \
        ::fwCore::log::ScopedMessage __spylog__scoped__msg__(        \
            __FILE__, __LINE__, SLM_PRETTY_FUNC());             \
        __spylog__scoped__msg__.use();                          \
        )

# endif
/**  @} */

/**
 * @brief Use this macro when deprecating a function to warn the developer.
 */
#define FW_DEPRECATED(oldFnName, newFnName) \
    OSLM_ERROR(  "[DEPRECATED] '" << oldFnName << "' is deprecated and will be removed, use '" \
                                  << newFnName << "' instead. It is still used by '" + this->getClassname() + "'." \
                 );

/**
 * @brief Use this macro when deprecating a function to warn the developer.
 */
#define FW_DEPRECATED_IF(oldFnName, newFnName, condition) \
    OSLM_ERROR_IF("[DEPRECATED] '" << oldFnName << "' is deprecated and will be removed, use '" \
                                   << newFnName << "' instead. It is still used by '" + this->getClassname() + "'.", \
                  condition);

/**
 * @brief Use this macro when deprecating a function to warn the developer.
 */
#define FW_DEPRECATED_MSG(message) \
    OSLM_ERROR(  "[DEPRECATED] '" << message);
