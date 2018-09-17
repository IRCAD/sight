/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "monitor/config.hpp"

namespace monitor
{

/**
 * @brief Function installing  a callback which print the backtrace on a SIGSEV and SIGUSR1 signal
 * for posix only intern use only for this bundle.
 */
MONITOR_API void installSIGSEVBacktrace();

} // namespace monitor
