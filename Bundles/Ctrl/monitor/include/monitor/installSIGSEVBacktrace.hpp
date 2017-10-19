/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MONITOR_INSTALLSIGSEVBACKTRACE_HPP__
#define __MONITOR_INSTALLSIGSEVBACKTRACE_HPP__

#include "monitor/config.hpp"


namespace monitor
{


/**
 * @brief Function installing  a callback which print the backtrace on a SIGSEV and SIGUSR1 signal
 * for posix only intern use only for this bundle.
 */
MONITOR_API void installSIGSEVBacktrace();


/**
 * @brief Generates segmentation fault for test.
 */
MONITOR_API void generateSIGSEV();


} // namespace monitor

#endif // __MONITOR_INSTALLSIGSEVBACKTRACE_HPP__

