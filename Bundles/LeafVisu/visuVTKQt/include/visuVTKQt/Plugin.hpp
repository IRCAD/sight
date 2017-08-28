/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKQT_PLUGIN_HPP__
#define __VISUVTKQT_PLUGIN_HPP__

#include "visuVTKQt/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace visuVTKQt
{
/**
 * @brief   This class is started when the bundles is loaded.
 */
struct VISUVTKQT_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    Plugin() noexcept;

    /**
     * @brief   destructor
     */
    ~Plugin() noexcept;

    // Overrides
    VISUVTKQT_API void start();

    // Overrides
    VISUVTKQT_API void stop() noexcept;

};

} // namespace visuVTKQt

#endif //__VISUVTKQT_PLUGIN_HPP__
