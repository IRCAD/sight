/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKQT_PLUGIN_HPP__
#define __VISUVTKQT_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "visuVTKQt/config.hpp"

namespace visuVTKQt
{
/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 *
 * @date    2009.
 */
struct VISUVTKQT_CLASS_API Plugin : public ::fwRuntime::Plugin
{
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
