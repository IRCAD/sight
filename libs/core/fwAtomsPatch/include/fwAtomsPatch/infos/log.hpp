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

#ifndef __FWATOMSPATCH_INFOS_LOG_HPP__
#define __FWATOMSPATCH_INFOS_LOG_HPP__

#include "fwAtomsPatch/infos/Logger.hpp"

/** @{ */
# ifdef _DEBUG
/** Info message macros for patch log. */
#  define fwAtomsPatchInfoLogMacro(message) \
    ::fwAtomsPatch::infos::Logger::getLogger().info(message)
/** Error message macros for patch log. */
#  define fwAtomsPatchErrorLogMacro(message) \
    ::fwAtomsPatch::infos::Logger::getLogger().error(message)
/** Bad cast message macros for patch log. */
#  define fwAtomsPatchBadCastLogMacro(message) \
    ::fwAtomsPatch::infos::Logger::getLogger().badCast(message)
/** Out of range message macros for patch log. */
#  define fwAtomsPatchOutOfRangeLogMacro(message) \
    ::fwAtomsPatch::infos::Logger::getLogger().outOfRange(message)
/** Add attribute message macros for patch log. */
#  define fwAtomsPatchAddAttributeLogMacro(message) \
    ::fwAtomsPatch::infos::Logger::getLogger().addAttribute(message)
/** Erase attribute message macros for patch log. */
#  define fwAtomsPatchEraseAttributeLogMacro(message) \
    ::fwAtomsPatch::infos::Logger::getLogger().eraseAttribute(message)
/** Replace attribute message macros for patch log. */
#  define fwAtomsPatchReplaceAttributeLogMacro(message) \
    ::fwAtomsPatch::infos::Logger::getLogger().replaceAttribute(message)
# else
/** Info message macros for patch log. */
#  define fwAtomsPatchInfoLogMacro(message) // nothing
/** Error message macros for patch log. */
#  define fwAtomsPatchErrorLogMacro(message) // nothing
/** Bad cast message macros for patch log. */
#  define fwAtomsPatchBadCastLogMacro(message) // nothing
/** Out of range message macros for patch log. */
#  define fwAtomsPatchOutOfRangeLogMacro(message) // nothing
/** Add attribute message macros for patch log. */
#  define fwAtomsPatchAddAttributeLogMacro(message) // nothing
/** Erase attribute message macros for patch log. */
#  define fwAtomsPatchEraseAttributeLogMacro(message) // nothing
/** Replace attribute message macros for patch log. */
#  define fwAtomsPatchReplaceAttributeLogMacro(message) // nothing
# endif
/**  @} */

#endif  // __FWATOMSPATCH_INFOS_LOG_HPP__
