/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef __CTRLSPLINENAVIGATION_PLUGIN_HPP__
#define __CTRLSPLINENAVIGATION_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "ctrlSplineNavigation/config.hpp"

namespace ctrlSplineNavigation
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct CTRLSPLINENAVIGATION_CLASS_API Plugin : public ::fwRuntime::Plugin
{

    /// Destructor
    CTRLSPLINENAVIGATION_API ~Plugin() noexcept;

    /// Overrides
    CTRLSPLINENAVIGATION_API void start();

    /// Overrides
    CTRLSPLINENAVIGATION_API void stop() noexcept;

};


} // namespace ctrlSplineNavigation


#endif //__CTRLSPLINENAVIGATION_PLUGIN_HPP__
