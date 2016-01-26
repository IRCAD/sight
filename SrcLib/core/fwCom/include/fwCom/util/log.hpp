/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOM_UTIL_LOG_HPP__
#define __FWCOM_UTIL_LOG_HPP__

#include <fwCore/spyLog.hpp>

/** @{ */
# ifdef COM_LOG
/** Communication message macros.  */
#  define SLM_COM(message)                      \
    SL_ERROR(_SPYLOG_SPYLOGGER_, message)
/** Com stringstream message macros.  */
#  define OSLM_COM(message)                     \
    OSL_ERROR(_SPYLOG_SPYLOGGER_, message)
/** Conditionnal com message macros.  */
#  define SLM_COM_IF(message, cond)                     \
    SL_ERROR_IF(_SPYLOG_SPYLOGGER_, message, cond)
/** Conditionnal com stringstream message macros.  */
#  define OSLM_COM_IF(message, cond)                    \
    OSL_ERROR_IF(_SPYLOG_SPYLOGGER_, message, cond)
# else
/** Communication message macros.  */
#  define SLM_COM(message) // nothing
/** Com stringstream message macros.  */
#  define OSLM_COM(message) // nothing
/** Conditionnal com message macros.  */
#  define SLM_COM_IF(message, cond) // nothing
/** Conditionnal com stringstream message macros.  */
#  define OSLM_COM_IF(message, cond) // nothing
# endif
/**  @} */

#endif  // __FWCOM_UTIL_LOG_HPP__
