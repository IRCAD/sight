/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef __VIDEOQT_HELPER_PREFERENCES_HPP__
#define __VIDEOQT_HELPER_PREFERENCES_HPP__

#include "videoQt/config.hpp"

#include <string>

namespace videoQt
{
namespace helper
{

VIDEOQT_API extern const std::string s_VIDEORENDER_PREF; ///< key of the preference composite for VideoRender system
VIDEOQT_API extern const std::string s_VIDEO_DIR_PREF; ///< key of the video directory in VideoRender preference composite

/// Returns the video dir saved in preferences (returns an empty string if it does not exist)
VIDEOQT_API std::string getVideoDir();

} // namespace helper
} // namespace videoQt



#endif /* __VIDEOQT_HELPER_PREFERENCES_HPP__ */
