/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOOPENNI_PLUGIN_HPP__
#define __VIDEOOPENNI_PLUGIN_HPP__

#include "videoOpenni/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace videoOpenni
{
/**
 * @brief   This class is started when the bundles is loaded.
 */
struct VIDEOOPENNI_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor
    VIDEOOPENNI_API ~Plugin() noexcept;

    // Overrides
    VIDEOOPENNI_API void start();

    // Overrides
    VIDEOOPENNI_API void stop() noexcept;

};

} // namespace videoOpenni

#endif //__VIDEOOPENNI_PLUGIN_HPP__
