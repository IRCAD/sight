/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTOOLS_OS_HPP__
#define __FWTOOLS_OS_HPP__

#include <string>

#include "config.hpp"


namespace fwTools {

/**
 * @brief	Namespace for os tools
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 */
namespace os {

/**
 * @brief	Return the users's application data directory
 * @author	IRCAD (Research and Development Team).
 * @param company The company name
 * @param appName The application name
 * @param createDirectory if true, create the returned directory if it don't exist
 * @date	2007-2009.
 *
 * Return the application data directory. If company or appName is not empty, append them to
 * the path. Under unix, XDG conventions are repected.
 * For example, the UserDataDir under linux will be "~/.config/company/appName"
 */
FWTOOLS_API std::string  getUserDataDir(
        std::string company  = "",
        std::string appName  = "",
        bool createDirectory = false
        );


} // namespace os

} // namespace fwTools 

#endif
