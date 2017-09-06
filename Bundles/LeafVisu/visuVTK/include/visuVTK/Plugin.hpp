/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTK_PLUGIN_HPP__
#define __VISUVTK_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "visuVTK/config.hpp"

namespace visuVTK
{
/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 *
 * @date    2009.
 */
struct VISUVTK_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ~Plugin() noexcept;

    // Overrides
    VISUVTK_API void start();

    // Overrides
    VISUVTK_API void stop() noexcept;

};

} // namespace visuVTK

#endif //__VISUVTK_PLUGIN_HPP__
