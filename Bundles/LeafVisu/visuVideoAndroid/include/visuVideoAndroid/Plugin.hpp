/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVIDEOANDROID_PLUGIN_HPP__
#define __VISUVIDEOANDROID_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "visuVideoAndroid/config.hpp"


namespace visuVideoAndroid
{
/**
 *
 */
class VISUVIDEOANDROID_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    /**
     * @brief   destructor
     */
    VISUVIDEOANDROID_API ~Plugin() throw();

    // Overrides
    VISUVIDEOANDROID_API void start() throw(::fwRuntime::RuntimeException);

    // Overrides
    VISUVIDEOANDROID_API void stop() throw();

};

} // namespace visuVideoAndroid

#endif //__VISUVIDEOANDROID_PLUGIN_HPP__
