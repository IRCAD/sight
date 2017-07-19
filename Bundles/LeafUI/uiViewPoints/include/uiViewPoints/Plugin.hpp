/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef __UIVIEWPOINTS_PLUGIN_HPP__
#define __UIVIEWPOINTS_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "uiViewPoints/config.hpp"

namespace uiViewPoints
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct UIVIEWPOINTS_CLASS_API Plugin : public ::fwRuntime::Plugin
{

    /// Destructor
    UIVIEWPOINTS_API ~Plugin() noexcept;

    /// Overrides
    UIVIEWPOINTS_API void start();

    /// Overrides
    UIVIEWPOINTS_API void stop() noexcept;

};


} // namespace uiViewPoints


#endif //__UIVIEWPOINTS_PLUGIN_HPP__
