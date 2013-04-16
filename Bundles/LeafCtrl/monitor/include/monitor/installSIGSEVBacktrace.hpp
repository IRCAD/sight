/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef MONITOR_INSTALLSIGSEVBACKTRACE_HPP_
#define MONITOR_INSTALLSIGSEVBACKTRACE_HPP_

#include "monitor/config.hpp"


namespace monitor
{


/** @brief function  installing  a callback which print the backtrace on a SIGSEV and SIGUSR1 signal
 *  for posix only intern use only for this bundle
 */
MONITOR_API void installSIGSEVBacktrace();


/**
 * @brief generate segmentation fault for test
 */
MONITOR_API void generateSIGSEV();


} // namespace monitor

#endif /* MONITOR_INSTALLSIGSEVBACKTRACE_HPP_ */
