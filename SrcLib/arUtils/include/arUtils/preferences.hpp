/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARUTILS_PREFERENCES_HPP__
#define __ARUTILS_PREFERENCES_HPP__

#include "arUtils/config.hpp"

#include <string>

namespace arUtils
{

ARUTILS_API extern const std::string s_VIDEORENDER_PREF; ///< key of the preference composite for VideoRender system
ARUTILS_API extern const std::string s_VIDEO_DIR_PREF; ///< key of the video directory in VideoRender preference composite

/// Returns the video dir saved in preferences (returns an empty string if it does not exist)
ARUTILS_API std::string getVideoDir();

} // namespace arUtils

#endif /* __ARUTILS_PREFERENCES_HPP__ */
