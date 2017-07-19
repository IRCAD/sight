/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOQT_PLUGIN_HPP__
#define __VIDEOQT_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "videoQt/config.hpp"

namespace videoQt
{
/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct VIDEOQT_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ~Plugin() noexcept;

    // Overrides
    VIDEOQT_API void start();

    // Overrides
    VIDEOQT_API void stop() noexcept;

};

} // namespace videoQt

#endif //__VIDEOQT_PLUGIN_HPP__
