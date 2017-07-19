/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef __UINAVIGATION_PLUGIN_HPP__
#define __UINAVIGATION_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "uiNavigation/config.hpp"

namespace uiNavigation
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct UINAVIGATION_CLASS_API Plugin : public ::fwRuntime::Plugin
{

    /// Destructor
    UINAVIGATION_API ~Plugin() noexcept;

    /// Overrides
    UINAVIGATION_API void start();

    /// Overrides
    UINAVIGATION_API void stop() noexcept;

};


} // namespace uiNavigation


#endif //__UINAVIGATION_PLUGIN_HPP__
