/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef __VISUNAVIGATION_PLUGIN_HPP__
#define __VISUNAVIGATION_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "visuNavigation/config.hpp"

namespace visuNavigation
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct VISUNAVIGATION_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor
    VISUNAVIGATION_API ~Plugin() noexcept;

    /// Overrides
    VISUNAVIGATION_API void start();

    /// Overrides
    VISUNAVIGATION_API void stop() noexcept;
};


} // namespace visuNavigation

#endif // __VISUNAVIGATION_PLUGIN_HPP__

