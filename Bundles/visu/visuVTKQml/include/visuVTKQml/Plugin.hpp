/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuVTKQml/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace visuVTKQml
{
/**
 * @brief   This class is started when the bundle is loaded.
 */
struct VISUVTKQML_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    Plugin() noexcept;

    /**
     * @brief   destructor
     */
    ~Plugin() noexcept;

    // Overrides
    VISUVTKQML_API void start();

    // Overrides
    VISUVTKQML_API void stop() noexcept;

};

} // namespace visuVTKQml
