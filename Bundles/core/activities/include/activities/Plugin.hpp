/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ACTIVITIES_PLUGIN_HPP__
#define __ACTIVITIES_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>
#include "activities/config.hpp"

namespace activities
{
/**
 *
 * @date    2012.
 */
struct ACTIVITIES_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ACTIVITIES_API ~Plugin() throw();

    // Overrides
    ACTIVITIES_API void start() throw(::fwRuntime::RuntimeException);

    // Overrides
    ACTIVITIES_API void stop() throw();

};

} // namespace activities

#endif //__ACTIVITIES_PLUGIN_HPP__
