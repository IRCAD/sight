/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUIDROID_PLUGIN_HPP__
#define __GUIDROID_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "guiDroid/config.hpp"

namespace guiDroid
{
/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct GUIDROID_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    GUIDROID_API ~Plugin() throw();

    // Overrides
    GUIDROID_API void start() throw(::fwRuntime::RuntimeException);

    // Overrides
    GUIDROID_API void stop() throw();

};

} // namespace guiDroid

#endif //__GUIDROID_PLUGIN_HPP__
