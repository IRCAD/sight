/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLCAMERA_PLUGIN_HPP__
#define __CTRLCAMERA_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "ctrlCamera/config.hpp"

namespace ctrlCamera
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct CTRLCAMERA_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    CTRLCAMERA_API ~Plugin() noexcept;

    /**
     * @brief Overrides start method. Object declarations.
     */
    CTRLCAMERA_API void start();

    /// Overrides stop method. Do nothing
    CTRLCAMERA_API void stop() noexcept;

};

} // namespace ctrlCamera

#endif //__CTRLCAMERA_PLUGIN_HPP__
